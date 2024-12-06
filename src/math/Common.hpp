#pragma once

#include "psyqo/matrix.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/vector.hh"

namespace mi::math {
    static psyqo::Trig<> TrigTable;

    typedef psyqo::Vector<2, 10> Rotation;

    constexpr psyqo::Matrix33 IdentityMtx = psyqo::Matrix33{{
        {1, 0, 0},
        {0, 1, 0}, 
        {0, 0, 1}
    }};
}