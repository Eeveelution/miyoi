#pragma once

#include "psyqo/gte-registers.hh"
#include "psyqo/matrix.hh"
#include "psyqo/soft-math.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/vector.hh"

#include "src/math/Common.hpp"
#include "src/math/Camera.hpp"
#include "src/math/Object.hpp"

namespace mi::gte {
    /// loads pseudoregisters V0 to V3 with these vectors.
    void setInputVertices(const psyqo::Vec3& v0, const psyqo::Vec3& v1, const psyqo::Vec3& v2) {
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V0>( v0 );
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V1>( v1 );
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V2>( v2 );
    }

    void setCameraObjectMatricies(const mi::math::Camera& cam, const mi::math::Object& obj, bool setRotation) {
        if(setRotation) {
            //do `view * model`
            psyqo::Matrix33 viewModelMatrix;

            mi::gte::multiplyMatrix33<
                psyqo::GTE::PseudoRegister::Rotation,
                psyqo::GTE::PseudoRegister::V0>(cam.viewRotationMtx, obj.worldMatrix, &viewModelMatrix);

            psyqo::GTE::writeSafe<psyqo::GTE::PseudoRegister::Rotation>(viewModelMatrix);
        }

        auto posCamSpace = obj.position - cam.position;

        psyqo::SoftMath::matrixVecMul3(cam.viewRotationMtx, posCamSpace, &posCamSpace);
        psyqo::GTE::writeSafe<psyqo::GTE::PseudoRegister::Translation>(posCamSpace);
    }
}