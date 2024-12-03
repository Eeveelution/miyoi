#pragma once

#include "psyqo/matrix.hh"
#include "psyqo/vector.hh"
#include "src/math/Common.hpp"
namespace mi {
    namespace math {
        struct Camera {
            psyqo::Vec3 position;
            mi::math::Rotation rotation;

            psyqo::Matrix33 viewRotationMtx;
        };
    }
}