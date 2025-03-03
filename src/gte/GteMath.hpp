#pragma once

#include <psyqo/gte-kernels.hh>
#include <psyqo/gte-registers.hh>

namespace mi::gte {
    template<psyqo::GTE::PseudoRegister reg>
    constexpr psyqo::GTE::Kernels::MX getMX()
    {
        if constexpr (reg == psyqo::GTE::PseudoRegister::Rotation) {
            return psyqo::GTE::Kernels::MX::RT;
        } else if constexpr (reg == psyqo::GTE::PseudoRegister::Light) {
            return psyqo::GTE::Kernels::MX::LL;
        } else if constexpr (reg == psyqo::GTE::PseudoRegister::Color) {
            return psyqo::GTE::Kernels::MX::LC;
        } else {
            //static_assert(false, "Not a matrix pseudo register (should be Rotation, Light or Color)");
        }
    }

    template<psyqo::GTE::PseudoRegister reg>
    constexpr psyqo::GTE::Kernels::MV getMV()
    {
        if constexpr (reg == psyqo::GTE::PseudoRegister::V0) {
            return psyqo::GTE::Kernels::MV::V0;
        } else if constexpr (reg == psyqo::GTE::PseudoRegister::V1) {
            return psyqo::GTE::Kernels::MV::V1;
        } else if constexpr (reg == psyqo::GTE::PseudoRegister::V2) {
            return psyqo::GTE::Kernels::MV::V2;
        } else {
            //static_assert(false, "Not a valid pseudo register (should be V0, V1 or V2)");
        }
    }
    template<psyqo::GTE::PseudoRegister mreg, psyqo::GTE::PseudoRegister vreg>
    void multiplyMatrix33(const psyqo::Matrix33& m2, psyqo::Matrix33* out)
    {
        constexpr auto mx = getMX<mreg>();
        constexpr auto mv = getMV<vreg>();

        psyqo::Vec3 t;

        psyqo::GTE::writeSafe<vreg>(psyqo::Vec3{m2.vs[0].x, m2.vs[1].x, m2.vs[2].x});

        psyqo::GTE::Kernels::mvmva<mx, mv>();

        t = psyqo::GTE::readSafe<psyqo::GTE::PseudoRegister::SV>();
        out->vs[0].x = t.x;
        out->vs[1].x = t.y;
        out->vs[2].x = t.z;

        psyqo::GTE::writeSafe<vreg>(psyqo::Vec3{m2.vs[0].y, m2.vs[1].y, m2.vs[2].y});

        psyqo::GTE::Kernels::mvmva<mx, mv>();

        t = psyqo::GTE::readSafe<psyqo::GTE::PseudoRegister::SV>();
        out->vs[0].y = t.x;
        out->vs[1].y = t.y;
        out->vs[2].y = t.z;

        psyqo::GTE::writeSafe<vreg>(psyqo::Vec3{m2.vs[0].z, m2.vs[1].z, m2.vs[2].z});

        psyqo::GTE::Kernels::mvmva<mx, mv>();

        t = psyqo::GTE::readSafe<psyqo::GTE::PseudoRegister::SV>();
        out->vs[0].z = t.x;
        out->vs[1].z = t.y;
        out->vs[2].z = t.z;
    }

    template<psyqo::GTE::PseudoRegister mreg, psyqo::GTE::PseudoRegister vreg>
    void multiplyMatrix33(const psyqo::Matrix33& m1, const psyqo::Matrix33& m2, psyqo::Matrix33* out)
    {
        psyqo::GTE::writeUnsafe<mreg>(m1);
        multiplyMatrix33<mreg, vreg>(m2, out);
    }
    
}