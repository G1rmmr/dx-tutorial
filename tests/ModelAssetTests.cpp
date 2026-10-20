#include "../dx-tutorial/ModelAsset.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace
{
    using namespace dx;
    using namespace DirectX;

    void Require(bool condition, const std::string& message)
    {
        if (!condition) throw std::runtime_error(message);
    }

    bool Near(float a, float b, float tolerance = 0.0001f)
    {
        return std::abs(a - b) <= tolerance;
    }

    void CheckMesh(const ModelAsset& model)
    {
        Require(!model.Vertices.empty() && !model.Indices.empty(), "empty imported buffers");
        Require(model.Indices.size() % 3 == 0, "index buffer is not triangulated");
        XMFLOAT3 actualMin{ (std::numeric_limits<float>::max)(),
            (std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)() };
        XMFLOAT3 actualMax{ -actualMin.x, -actualMin.y, -actualMin.z };
        for (const auto& vertex : model.Vertices)
        {
            const auto p = vertex.Pos;
            Require(std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z),
                "nonfinite vertex position");
            const auto color = vertex.Color;
            for (float channel : { color.x, color.y, color.z, color.w })
            {
                Require(std::isfinite(channel) && channel >= 0.f && channel <= 1.f,
                    "invalid baked vertex color");
            }
            actualMin.x = std::min(actualMin.x, p.x);
            actualMin.y = std::min(actualMin.y, p.y);
            actualMin.z = std::min(actualMin.z, p.z);
            actualMax.x = std::max(actualMax.x, p.x);
            actualMax.y = std::max(actualMax.y, p.y);
            actualMax.z = std::max(actualMax.z, p.z);
        }
        for (auto index : model.Indices)
        {
            Require(index < model.Vertices.size(), "index outside vertex buffer");
        }
        Require(Near(actualMin.x, model.Min.x) && Near(actualMin.y, model.Min.y) &&
            Near(actualMin.z, model.Min.z) && Near(actualMax.x, model.Max.x) &&
            Near(actualMax.y, model.Max.y) && Near(actualMax.z, model.Max.z),
            "reported bounds differ from actual geometry");
        Require(Near(model.Min.x + model.Max.x, 0.f) &&
            Near(model.Min.z + model.Max.z, 0.f), "model is not centered on XZ");
    }

    template<typename Action>
    void ExpectFailure(Action action, const std::string& pathFragment)
    {
        try
        {
            action();
        }
        catch (const std::exception& error)
        {
            const std::string message = error.what();
            Require(message.find(pathFragment) != std::string::npos,
                "failure omitted asset path: " + message);
            Require(message.find("FBX import") != std::string::npos,
                "failure omitted import context: " + message);
            return;
        }
        throw std::runtime_error("expected asset import to fail: " + pathFragment);
    }

    struct ScratchDirectory
    {
        std::filesystem::path Path;
        ScratchDirectory()
        {
            const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
            Path = std::filesystem::temp_directory_path() /
                ("dx-tutorial-model-tests-" + std::to_string(stamp));
            Require(std::filesystem::create_directory(Path),
                "cannot create a unique test directory");
        }
        ~ScratchDirectory()
        {
            std::error_code error;
            std::filesystem::remove_all(Path, error);
        }
    };

    void WriteFile(const std::filesystem::path& path, const std::string& text)
    {
        std::ofstream file(path, std::ios::binary);
        file << text;
        Require(static_cast<bool>(file), "cannot create test fixture");
    }

    // A quad is instanced twice under a translated/nonuniformly scaled parent.
    // The first instance has a geometric translation; both mirror local X.
    // Normals and RGBA are intentionally separate from control point positions.
    const char* Fixture = R"FBX(; FBX 7.4.0 project file
FBXHeaderExtension: {
    FBXHeaderVersion: 1003
    FBXVersion: 7400
}
GlobalSettings: {
    Version: 1000
    Properties70: {
        P: "UpAxis", "int", "Integer", "", 1
        P: "UpAxisSign", "int", "Integer", "", 1
        P: "FrontAxis", "int", "Integer", "", 2
        P: "FrontAxisSign", "int", "Integer", "", 1
        P: "CoordAxis", "int", "Integer", "", 0
        P: "CoordAxisSign", "int", "Integer", "", 1
        P: "UnitScaleFactor", "double", "Number", "", 100
    }
}
Objects: {
    Geometry: 1, "Geometry::Quad", "Mesh" {
        Vertices: *12 { a: 0,0,0,2,0,0,2,1,0,0,1,0 }
        PolygonVertexIndex: *4 { a: 0,1,2,-4 }
        LayerElementNormal: 0 {
            Version: 101
            Name: ""
            MappingInformationType: "ByPolygonVertex"
            ReferenceInformationType: "Direct"
            Normals: *12 { a: 0,0,1,0,0,1,0,0,1,0,0,1 }
        }
        LayerElementColor: 0 {
            Version: 101
            Name: "Color"
            MappingInformationType: "ByPolygonVertex"
            ReferenceInformationType: "Direct"
            Colors: *16 { a: 0.5,1,0.25,1,0.5,1,0.25,1,0.5,1,0.25,1,0.5,1,0.25,1 }
        }
        LayerElementMaterial: 0 {
            Version: 101
            Name: ""
            MappingInformationType: "AllSame"
            ReferenceInformationType: "IndexToDirect"
            Materials: *1 { a: 0 }
        }
        Layer: 0 {
            Version: 100
            LayerElement: { Type: "LayerElementNormal" TypedIndex: 0 }
            LayerElement: { Type: "LayerElementColor" TypedIndex: 0 }
            LayerElement: { Type: "LayerElementMaterial" TypedIndex: 0 }
        }
    }
    Model: 2, "Model::First", "Mesh" {
        Version: 232
        Properties70: {
            P: "Lcl Translation", "Lcl Translation", "", "A", 10,20,30
            P: "Lcl Scaling", "Lcl Scaling", "", "A", -1,1,1
            P: "GeometricTranslation", "Vector3D", "Vector", "", 0,0,3
        }
    }
    Model: 3, "Model::Parent", "Null" {
        Version: 232
        Properties70: {
            P: "Lcl Translation", "Lcl Translation", "", "A", 2,3,4
            P: "Lcl Scaling", "Lcl Scaling", "", "A", 1,2,1
        }
    }
    Model: 5, "Model::Second", "Mesh" {
        Version: 232
        Properties70: {
            P: "Lcl Translation", "Lcl Translation", "", "A", 14,20,30
            P: "Lcl Scaling", "Lcl Scaling", "", "A", -1,1,1
        }
    }
    Material: 4, "Material::Red", "" {
        Version: 102
        ShadingModel: "lambert"
        Properties70: {
            P: "DiffuseColor", "Color", "", "A", 0.8,0.4,0.2
            P: "DiffuseFactor", "Number", "", "A", 1
        }
    }
    Material: 6, "Material::Blue", "" {
        Version: 102
        ShadingModel: "lambert"
        Properties70: {
            P: "DiffuseColor", "Color", "", "A", 0.2,0.4,0.8
            P: "DiffuseFactor", "Number", "", "A", 1
        }
    }
}
Connections: {
    C: "OO",1,2
    C: "OO",1,5
    C: "OO",4,2
    C: "OO",6,5
    C: "OO",2,3
    C: "OO",5,3
    C: "OO",3,0
}
)FBX";

    void CheckFixture(const std::filesystem::path& path)
    {
        ModelImportOptions options;
        options.Size = 2.f;
        options.BakeLighting = false;
        const auto model = LoadFbxModel(path, options);
        CheckMesh(model);
        Require(model.SourceMeshCount == 2 && model.MaterialCount == 2,
            "mesh instances or per-instance materials were lost");
        Require(model.Indices.size() == 12, "two quads must yield four triangles");
        Require(Near(model.Max.x - model.Min.x, 6.f) &&
            Near(model.Max.y - model.Min.y, 2.f) &&
            Near(model.Max.z - model.Min.z, 3.f),
            "hierarchy, geometric translation or source aspect ratio was lost");
        bool red = false, blue = false;
        for (const auto& vertex : model.Vertices)
        {
            red |= Near(vertex.Color.x, 0.4f) && Near(vertex.Color.y, 0.4f) &&
                Near(vertex.Color.z, 0.05f);
            blue |= Near(vertex.Color.x, 0.1f) && Near(vertex.Color.y, 0.4f) &&
                Near(vertex.Color.z, 0.2f);
        }
        Require(red && blue, "diffuse color and vertex color were not multiplied");
        // The RH source +Z outward normal becomes -Z in the LH target.
        // Negative local X must not invert the outward face orientation.
        for (std::size_t index = 0; index < model.Indices.size(); index += 3)
        {
            const auto a = model.Vertices[model.Indices[index]].Pos;
            const auto b = model.Vertices[model.Indices[index + 1]].Pos;
            const auto c = model.Vertices[model.Indices[index + 2]].Pos;
            const float crossZ = (b.x - a.x) * (c.y - a.y) -
                (b.y - a.y) * (c.x - a.x);
            Require(crossZ < 0.f, "mirrored instance has incorrect D3D CW winding");
        }
        options.BakeLighting = true;
        const auto shaded = LoadFbxModel(path, options);
        Require(shaded.Vertices.front().Color.x < model.Vertices.front().Color.x,
            "normal-based lighting was not baked");
    }
}

int main(int argc, char** argv)
{
    try
    {
        const std::filesystem::path models = argc > 1 ? argv[1] : "dx-tutorial/assets/models";
        struct ExpectedAsset
        {
            const char* Name;
            std::size_t Meshes, Materials, Vertices, Triangles;
        };
        const std::array<ExpectedAsset, 5> expectedAssets{{
            { "elite.fbx", 28, 0, 2413, 2624 },
            { "enemy.fbx", 7, 0, 1786, 3352 },
            { "grenades.fbx", 6, 0, 2862, 5704 },
            { "machine-gun.fbx", 1, 3, 6381, 6997 },
            { "pistol.fbx", 7, 7, 2118, 2243 },
        }};
        for (const auto& expected : expectedAssets)
        {
            const auto* name = expected.Name;
            const auto model = LoadFbxModel(models / name);
            CheckMesh(model);
            Require(Near(model.Min.y, 0.f) && Near(model.Max.y, 2.f),
                std::string(name) + " height/bottom normalization failed");
            Require(model.SourceMeshCount == expected.Meshes &&
                model.MaterialCount == expected.Materials &&
                model.Vertices.size() == expected.Vertices &&
                model.Indices.size() / 3 == expected.Triangles,
                std::string(name) + " imported mesh/material/topology statistics changed");
            ModelImportOptions centeredOptions;
            centeredOptions.Size = 3.f;
            centeredOptions.SizeAxis = ModelSizeAxis::Longest;
            centeredOptions.Origin = ModelOrigin::Center;
            centeredOptions.BakeLighting = false;
            const auto centered = LoadFbxModel(models / name, centeredOptions);
            CheckMesh(centered);
            Require(Near(centered.Min.y + centered.Max.y, 0.f), "center origin failed");
            Require(Near(std::max({ centered.Max.x - centered.Min.x,
                centered.Max.y - centered.Min.y, centered.Max.z - centered.Min.z }), 3.f),
                "longest-side normalization failed");
            const float ratio = (centered.Max.y - centered.Min.y) /
                (model.Max.y - model.Min.y);
            Require(Near((model.Max.x - model.Min.x) * ratio,
                centered.Max.x - centered.Min.x) &&
                Near((model.Max.z - model.Min.z) * ratio,
                    centered.Max.z - centered.Min.z), "uniform scale changed aspect ratio");
            std::cout << name << ": meshes=" << model.SourceMeshCount
                << " materials=" << model.MaterialCount << " vertices=" << model.Vertices.size()
                << " triangles=" << model.Indices.size() / 3 << " bounds=("
                << model.Min.x << ',' << model.Min.y << ',' << model.Min.z << ")..("
                << model.Max.x << ',' << model.Max.y << ',' << model.Max.z << ")\n";
        }
        ScratchDirectory scratch;
        const auto invalid = scratch.Path / "invalid.fbx";
        WriteFile(invalid, "This is not an FBX document.\n");
        ExpectFailure([&] { LoadFbxModel(invalid); }, "invalid.fbx");
        ExpectFailure([&] { LoadFbxModel(scratch.Path / "missing.fbx"); }, "missing.fbx");
        ModelImportOptions invalidSize;
        invalidSize.Size = 0.f;
        ExpectFailure([&] { LoadFbxModel(models / expectedAssets[0].Name, invalidSize); },
            expectedAssets[0].Name);
        invalidSize.Size = std::numeric_limits<float>::quiet_NaN();
        ExpectFailure([&] { LoadFbxModel(models / expectedAssets[0].Name, invalidSize); },
            expectedAssets[0].Name);
        const auto fixture = scratch.Path / "hierarchy-mirrored-quad.fbx";
        WriteFile(fixture, Fixture);
        CheckFixture(fixture);
        std::cout << "PASS: 5 source assets, normalization, materials, hierarchy, mirrored winding, "
            "triangulation, lighting and invalid input checks\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
