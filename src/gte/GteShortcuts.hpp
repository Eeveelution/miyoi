#pragma once

#include "psyqo/gte-registers.hh"
#include "psyqo/matrix.hh"
#include "psyqo/soft-math.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/vector.hh"
#include "src/gte/GteMath.hpp"
#include "src/math/Common.hpp"

namespace mi::gte {
    /// loads pseudoregisters V0 to V3 with these vectors.
    inline void setInputVertices(const psyqo::Vec3& v0, const psyqo::Vec3& v1, const psyqo::Vec3& v2) {
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V0>( v0 );
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V1>( v1 );
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V2>( v2 );
    }

    inline void setPerspectiveCameraObjectMatricies(const psyqo::Trig<>& trig, const psyqo::Vec3& camPos, const psyqo::Matrix33& camPosRotMtx, const psyqo::Vec3& objectPos, const mi::math::Rotation& objectRot, bool setViewRot = true) {
        if(objectRot.x == 0.0 && objectRot.y == 0 && setViewRot) {
            //don't need to combine the camera rotation and object rotation
            psyqo::GTE::writeSafe<psyqo::GTE::PseudoRegister::Rotation>(camPosRotMtx);
        } else {
            auto objectRotationMtx = psyqo::SoftMath::generateRotationMatrix33(objectRot.y, psyqo::SoftMath::Axis::Y, trig);

            if(objectRot.x != 0.0) {
                auto pitchRotMtx = psyqo::SoftMath::generateRotationMatrix33(objectRot.x, psyqo::SoftMath::Axis::X, trig);

                mi::gte::multiplyMatrix33<
                    psyqo::GTE::PseudoRegister::Rotation, 
                    psyqo::GTE::PseudoRegister::V0>
                (
                    objectRotationMtx, 
                    &objectRotationMtx
                );
            }

            //combine with camera position and rotation
            mi::gte::multiplyMatrix33<psyqo::GTE::PseudoRegister::Rotation, psyqo::GTE::PseudoRegister::V0>(camPosRotMtx, objectRotationMtx, &objectRotationMtx);

            psyqo::GTE::writeSafe<psyqo::GTE::PseudoRegister::Rotation>(objectRotationMtx);
        }

        auto camPosSpace = objectPos - camPos;
        psyqo::SoftMath::matrixVecMul3(camPosRotMtx, camPosSpace, &camPosSpace);

        psyqo::GTE::writeSafe<psyqo::GTE::PseudoRegister::Translation>(camPosSpace);
    }
}