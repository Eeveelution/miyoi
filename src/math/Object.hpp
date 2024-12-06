#pragma once

#include "psyqo/matrix.hh"
#include "psyqo/soft-math.hh"
#include "psyqo/vector.hh"
#include "src/gte/GteMath.hpp"
#include "src/math/Common.hpp"
namespace mi::math {
    struct Object {
        psyqo::Vec3 position;
        mi::math::Rotation rotation;

        psyqo::Matrix33 worldMatrix;
        
        void recalculateWorldMatrix() {
            if(rotation.x == 0.0 && rotation.y == 0.0) {
                worldMatrix = mi::math::IdentityMtx;

                return;
            }

            worldMatrix = psyqo::SoftMath::generateRotationMatrix33(rotation.y, psyqo::SoftMath::Axis::Y, mi::math::TrigTable);

            if(rotation.x == 0) {
                return;
            }

            const auto pitch = psyqo::SoftMath::generateRotationMatrix33(rotation.x, psyqo::SoftMath::Axis::X, mi::math::TrigTable);

            mi::gte::multiplyMatrix33<
                psyqo::GTE::PseudoRegister::Rotation,
                psyqo::GTE::PseudoRegister::V0>(worldMatrix, pitch, &worldMatrix);
        }
    };
}