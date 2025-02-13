#pragma once

#include "EASTL/array.h"
#include "psyqo/ordering-table.hh"
#include "src/game/Bullet.hpp"

class BulletList {
    public:
        eastl::array<Bullet, 1024> m_bullets;

        BulletList();

        void addBullet(const Bullet& newBullet);
        void update();
        void draw(psyqo::GPU& gpu);
};