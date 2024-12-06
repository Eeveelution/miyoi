#pragma once

#include "EASTL/array.h"
#include "EASTL/span.h"
#include "psyqo/primitives/common.hh"
#include "psyqo/vector.hh"
#include "src/gpu/Common.hpp"
#include "src/math/Camera.hpp"
#include "src/math/Object.hpp"

namespace mi::gpu {
    struct IndexedColoredQuadFace {
        uint8_t vertexIndicies[4];
        psyqo::Color color;
    };
}

namespace mi::gpu {
    void drawIndexedColoredQuads(
        mi::gpu::OrderingTableType& ot,
        mi::gpu::PrimBufferAllocatorType& pb,

        const IndexedColoredQuadFace* quadFaces, 
        uint32_t quadFaceCount,

        const psyqo::Vec3* vertices
    );
}