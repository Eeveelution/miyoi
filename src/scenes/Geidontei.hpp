#pragma once

#include <psyqo/font.hh>

#include <psyqo/gpu.hh>
#include <psyqo/scene.hh>

#include "../GameBase.hpp"
#include "EASTL/array.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/vector.h"
#include "psyqo/advancedpad.hh"
#include "psyqo/fixed-point.hh"
#include "psyqo/fragment-concept.hh"
#include "psyqo/fragments.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/primitives/misc.hh"
#include "psyqo/primitives/quads.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/vector.hh"
#include "src/game/Bullet.hpp"
#include "src/game/BulletList.hpp"
#include "src/game/Enemy.hpp"
#include "src/math/Camera.hpp"
#include "src/math/Object.hpp"


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
            psyqo::Color m_clearColor = { .r = 128, .g = 0, .b = 128 };

            eastl::array< psyqo::Fragments::SimpleFragment<psyqo::Prim::Quad>, 6 > m_quadFragments;
            psyqo::Angle m_currentAngle;

            mi::math::Camera m_Camera{};
            mi::math::Object m_cubeObj;

            psyqo::AdvancedPad pad;
            psyqo::Vec2 m_playerPosition;

            BulletList m_bulletList;

            uint32_t m_playerScore;

            uint32_t m_playerLives;
            uint32_t m_immuneFrames;

            eastl::vector<Enemy> enemies;

            int16_t background1y;
            int16_t background2y;
        public:
            Geidontei(GameBase& game);

            void frame() override;
        private:
            void render();
            void inputHandling();
            void update();
        };
    }
}