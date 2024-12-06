#pragma once

#include "psyqo/primitives/common.hh"
#include "psyqo/vector.hh"

namespace mi::gpu {
    struct ColoredFace {
        uint8_t indices[4];

        psyqo::Color color;
    };

    struct VertexPosition {
        psyqo::Vec3 position;
    };

    struct VertexPositionColor {
        psyqo::Vec3 position;
        psyqo::Color color;
    }
}