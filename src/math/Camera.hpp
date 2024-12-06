#pragma once

#include "psyqo/matrix.hh"
#include "psyqo/soft-math.hh"
#include "psyqo/vector.hh"
#include "src/math/Common.hpp"

namespace mi::math {
    struct Camera {
        psyqo::Vec3 position;
        mi::math::Rotation rotation;

        psyqo::Matrix33 viewRotationMtx;

        void recalculateViewRotationMatrix() {
            this->viewRotationMtx = psyqo::SoftMath::generateRotationMatrix33(this->rotation.y, psyqo::SoftMath::Axis::Y, mi::math::TrigTable);
            auto rotX   = psyqo::SoftMath::generateRotationMatrix33(this->rotation.x, psyqo::SoftMath::Axis::X, mi::math::TrigTable);

            psyqo::SoftMath::multiplyMatrix33(viewRotationMtx, rotX, &viewRotationMtx);
        }
    };
}