#pragma once

#include "Vertex.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace dx
{
    enum class ModelOrigin { BottomCenter, Center };
    enum class ModelSizeAxis { Longest, Height };

    struct ModelImportOptions
    {
        float Size = 2.f;
        ModelSizeAxis SizeAxis = ModelSizeAxis::Height;
        ModelOrigin Origin = ModelOrigin::BottomCenter;
        bool BakeLighting = true;
    };

    struct ModelAsset
    {
        std::vector<Vertex> Vertices;
        std::vector<std::uint32_t> Indices;
        DirectX::XMFLOAT3 Min{};
        DirectX::XMFLOAT3 Max{};
        std::size_t SourceMeshCount = 0;
        std::size_t MaterialCount = 0;
    };

    // Imports all mesh instances at their authored static pose. Animation,
    // skin deformation and texture sampling are not part of this color-only path.
    // Hierarchy and geometric transforms are baked into LH, Y-up geometry.
    // Size is the final height or longest dimension, preserving source proportions.
    ModelAsset LoadFbxModel(const std::filesystem::path& path,
        const ModelImportOptions& options = {});
}
