#pragma once

#include "EASTL/array.h"
#include <psyqo/gte-registers.hh>

namespace mi {
    namespace gpu {
        void DrawDistanceSubdividedPlane(eastl::array<psyqo::GTE::PackedVec3, 4> quad);
    }
}