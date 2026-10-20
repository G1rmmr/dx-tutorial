#include "ModelAsset.h"

#include "../third_party/ufbx/ufbx.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace
{
    using namespace DirectX;

    std::string Utf8Path(const std::filesystem::path& path)
    {
        const auto text = path.u8string();
        return std::string(reinterpret_cast<const char*>(text.data()), text.size());
    }

    [[noreturn]] void Fail(const std::string& path, const std::string& reason)
    {
        throw std::runtime_error("FBX import '" + path + "': " + reason);
    }

    std::string DescribeError(const ufbx_error& error)
    {
        char message[2048]{};
        ufbx_format_error(message, sizeof(message), &error);
        return message;
    }

    bool Finite(ufbx_vec3 value)
    {
        return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
    }

    ufbx_vec3 Sub(ufbx_vec3 a, ufbx_vec3 b)
    {
        return { a.x - b.x, a.y - b.y, a.z - b.z };
    }

    ufbx_vec3 Cross(ufbx_vec3 a, ufbx_vec3 b)
    {
        return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x };
    }

    double Dot(ufbx_vec3 a, ufbx_vec3 b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    ufbx_vec3 Normalize(ufbx_vec3 value, ufbx_vec3 fallback = { 0.0, 1.0, 0.0 })
    {
        const double length = std::sqrt(Dot(value, value));
        if (!std::isfinite(length) || length <= 1e-20)
        {
            return fallback;
        }
        return { value.x / length, value.y / length, value.z / length };
    }

    float UnitColor(double value)
    {
        return std::isfinite(value) ? static_cast<float>(std::clamp(value, 0.0, 1.0)) : 1.f;
    }

    XMFLOAT4 DiffuseColor(const ufbx_material* material)
    {
        if (!material)
        {
            return { 0.7f, 0.7f, 0.7f, 1.f };
        }
        const auto& diffuse = material->fbx.diffuse_color;
        const double factor = material->fbx.diffuse_factor.has_value
            ? material->fbx.diffuse_factor.value_real : 1.0;
        if (!diffuse.has_value)
        {
            return { 0.7f, 0.7f, 0.7f, 1.f };
        }
        return { UnitColor(diffuse.value_vec3.x * factor),
            UnitColor(diffuse.value_vec3.y * factor),
            UnitColor(diffuse.value_vec3.z * factor), 1.f };
    }

    const ufbx_material* FaceMaterial(const ufbx_node& node, std::size_t faceIndex)
    {
        const auto& mesh = *node.mesh;
        const auto index = faceIndex < mesh.face_material.count
            ? mesh.face_material.data[faceIndex] : 0U;
        return index < node.materials.count ? node.materials.data[index] : nullptr;
    }
}

dx::ModelAsset dx::LoadFbxModel(const std::filesystem::path& path,
    const ModelImportOptions& options)
{
    using namespace DirectX;
    const std::string filename = Utf8Path(path);
    if (!std::isfinite(options.Size) || options.Size <= 0.f)
    {
        Fail(filename, "target size must be finite and positive");
    }

    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        Fail(filename, "cannot open file");
    }
    const std::vector<char> bytes{ std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>() };
    if (file.bad())
    {
        Fail(filename, "cannot read file");
    }

    ufbx_load_opts load{};
    load.file_format = UFBX_FILE_FORMAT_FBX;
    load.filename = { filename.data(), filename.size() };
    load.target_axes = ufbx_axes_left_handed_y_up;
    load.target_unit_meters = 1.0;
    load.generate_missing_normals = true;
    load.ignore_animation = true;
    load.ignore_embedded = true;
    load.load_external_files = false;
    ufbx_error error{};
    const std::unique_ptr<ufbx_scene, decltype(&ufbx_free_scene)> scene(
        ufbx_load_memory(bytes.data(), bytes.size(), &load, &error), &ufbx_free_scene);
    if (!scene)
    {
        Fail(filename, DescribeError(error));
    }

    ModelAsset result;
    std::vector<ufbx_vec3> positionsToNormalize;
    std::unordered_set<const ufbx_material*> materials;
    ufbx_vec3 minimum{ std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
    ufbx_vec3 maximum{ -minimum.x, -minimum.y, -minimum.z };
    // Light direction is fixed in the imported model's local game space.
    const ufbx_vec3 light = Normalize({ -0.35, 0.8, -0.5 });

    for (std::size_t nodeIndex = 0; nodeIndex < scene->nodes.count; ++nodeIndex)
    {
        const ufbx_node& node = *scene->nodes.data[nodeIndex];
        if (!node.mesh || node.mesh->num_triangles == 0)
        {
            continue;
        }
        const ufbx_mesh& mesh = *node.mesh;
        ++result.SourceMeshCount;
        const ufbx_matrix normalMatrix = ufbx_matrix_for_normals(&node.geometry_to_world);
        if (mesh.max_face_triangles > (std::numeric_limits<std::size_t>::max)() / 3)
        {
            Fail(filename, "polygon exceeds supported size");
        }
        std::vector<std::uint32_t> triangleIndices(mesh.max_face_triangles * 3);
        for (std::size_t faceIndex = 0; faceIndex < mesh.faces.count; ++faceIndex)
        {
            const ufbx_face face = mesh.faces.data[faceIndex];
            if (face.num_indices < 3 ||
                (faceIndex < mesh.face_hole.count && mesh.face_hole.data[faceIndex]))
            {
                continue;
            }
            const auto triangleCount = ufbx_triangulate_face(triangleIndices.data(),
                triangleIndices.size(), &mesh, face);
            const ufbx_material* material = FaceMaterial(node, faceIndex);
            if (material)
            {
                materials.insert(material);
            }
            const XMFLOAT4 diffuse = DiffuseColor(material);
            for (std::uint32_t triangle = 0; triangle < triangleCount; ++triangle)
            {
                std::array<ufbx_vec3, 3> positions{};
                std::array<ufbx_vec3, 3> normals{};
                std::array<std::uint32_t, 3> corners{};
                for (std::size_t corner = 0; corner < 3; ++corner)
                {
                    corners[corner] = triangleIndices[triangle * 3 + corner];
                    positions[corner] = ufbx_transform_position(&node.geometry_to_world,
                        ufbx_get_vertex_vec3(&mesh.vertex_position, corners[corner]));
                    normals[corner] = ufbx_transform_direction(&normalMatrix,
                        ufbx_get_vertex_vec3(&mesh.vertex_normal, corners[corner]));
                    if (!Finite(positions[corner]))
                    {
                        Fail(filename, "mesh contains a nonfinite transformed position");
                    }
                }
                const ufbx_vec3 faceNormal = Cross(Sub(positions[1], positions[0]),
                    Sub(positions[2], positions[0]));
                const ufbx_vec3 meanNormal{ normals[0].x + normals[1].x + normals[2].x,
                    normals[0].y + normals[1].y + normals[2].y,
                    normals[0].z + normals[1].z + normals[2].z };
                // D3D's CW fronts in LH space have an outward cross product.
                // Compare transformed normals to handle mirrored mesh instances too.
                std::array<std::size_t, 3> order{ 0, 1, 2 };
                if (Dot(faceNormal, meanNormal) < 0.0)
                {
                    std::swap(order[1], order[2]);
                }
                if (result.Vertices.size() >
                    (std::numeric_limits<std::uint32_t>::max)() / sizeof(Vertex) - 3)
                {
                    Fail(filename, "mesh exceeds D3D11 buffer limits");
                }
                for (const auto corner : order)
                {
                    const ufbx_vec3 position = positions[corner];
                    minimum.x = std::min(minimum.x, position.x);
                    minimum.y = std::min(minimum.y, position.y);
                    minimum.z = std::min(minimum.z, position.z);
                    maximum.x = std::max(maximum.x, position.x);
                    maximum.y = std::max(maximum.y, position.y);
                    maximum.z = std::max(maximum.z, position.z);
                    const double shade = options.BakeLighting ?
                        0.35 + 0.65 * std::max(0.0,
                            Dot(Normalize(normals[corner], Normalize(faceNormal)), light)) : 1.0;
                    ufbx_vec4 vertexColor{ 1.0, 1.0, 1.0, 1.0 };
                    if (mesh.vertex_color.exists)
                    {
                        vertexColor = ufbx_get_vertex_vec4(&mesh.vertex_color, corners[corner]);
                    }
                    Vertex vertex{};
                    // Normalize in double precision below before converting positions to float.
                    vertex.Color = XMFLOAT4(UnitColor(diffuse.x * vertexColor.x * shade),
                        UnitColor(diffuse.y * vertexColor.y * shade),
                        UnitColor(diffuse.z * vertexColor.z * shade),
                        UnitColor(diffuse.w * vertexColor.w));
                    result.Vertices.push_back(vertex);
                    positionsToNormalize.push_back(position);
                }
            }
        }
    }
    if (result.Vertices.empty())
    {
        Fail(filename, "file contains no triangle meshes");
    }

    const ufbx_vec3 extent = Sub(maximum, minimum);
    const double reference = options.SizeAxis == ModelSizeAxis::Height ? extent.y :
        std::max({ extent.x, extent.y, extent.z });
    if (!std::isfinite(reference) || reference <= 1e-20)
    {
        Fail(filename, "model has no extent along the requested size axis");
    }
    const double scale = options.Size / reference;
    const ufbx_vec3 origin{ minimum.x + extent.x * 0.5,
        options.Origin == ModelOrigin::Center ? minimum.y + extent.y * 0.5 : minimum.y,
        minimum.z + extent.z * 0.5 };

    for (std::size_t vertexIndex = 0; vertexIndex < result.Vertices.size(); ++vertexIndex)
    {
        const auto normalized = Sub(positionsToNormalize[vertexIndex], origin);
        const ufbx_vec3 scaled{ normalized.x * scale, normalized.y * scale, normalized.z * scale };
        if (!Finite(scaled) || std::max({ std::abs(scaled.x), std::abs(scaled.y),
            std::abs(scaled.z) }) > (std::numeric_limits<float>::max)())
        {
            Fail(filename, "normalized position exceeds finite float range");
        }
        result.Vertices[vertexIndex].Pos = XMFLOAT3(static_cast<float>(scaled.x),
            static_cast<float>(scaled.y), static_cast<float>(scaled.z));
    }

    result.Indices.resize(result.Vertices.size());
    ufbx_vertex_stream stream{ result.Vertices.data(), result.Vertices.size(), sizeof(Vertex) };
    const auto uniqueVertices = ufbx_generate_indices(&stream, 1, result.Indices.data(),
        result.Indices.size(), nullptr, &error);
    if (uniqueVertices == 0)
    {
        Fail(filename, "cannot create indexed mesh: " + DescribeError(error));
    }
    result.Vertices.resize(uniqueVertices);
    result.Min = XMFLOAT3(static_cast<float>((minimum.x - origin.x) * scale),
        static_cast<float>((minimum.y - origin.y) * scale),
        static_cast<float>((minimum.z - origin.z) * scale));
    result.Max = XMFLOAT3(static_cast<float>((maximum.x - origin.x) * scale),
        static_cast<float>((maximum.y - origin.y) * scale),
        static_cast<float>((maximum.z - origin.z) * scale));
    result.MaterialCount = materials.size();
    return result;
}
