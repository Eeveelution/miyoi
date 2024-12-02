#pragma once

#include <psyqo/font.hh>

#include <psyqo/gpu.hh>
#include <psyqo/scene.hh>

#include "../GameBase.hpp"
#include "EASTL/array.h"
#include "psyqo/fragment-concept.hh"
#include "psyqo/fragments.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/primitives/misc.hh"
#include "psyqo/primitives/quads.hh"
#include "psyqo/trigonometry.hh"

namespace mi {
    namespace Scenes {
        class Geidontei final : public psyqo::Scene {
        private:
            // We'll have some simple animation going on, so we
            // need to keep track of our state here.
            uint8_t _anim = 0;
            bool _direction = true;

            GameBase& _game; 

            psyqo::Fragments::SimpleFragment<psyqo::Prim::FastFill> m_clearFragment[2];
            psyqo::Color m_clearColor = { .r = 0, .g = 0, .b = 0 };

            eastl::array< psyqo::Fragments::SimpleFragment<psyqo::Prim::Quad>, 6 > m_quadFragments;
            psyqo::Angle m_currentAngle;
        public:
            Geidontei(GameBase& game);

            void frame() override;
        };
    }
}