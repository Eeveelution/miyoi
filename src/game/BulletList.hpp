#pragma once

#include "EASTL/array.h"
#include "EASTL/fixed_vector.h"
#include "psyqo/ordering-table.hh"
#include "src/game/Bullet.hpp"

#define BULLET_LIST_SIZE 384

class BulletList {
    public:
        eastl::array<Bullet, BULLET_LIST_SIZE> m_bullets;
        eastl::fixed_vector<uint16_t, 1024> m_dead;

        BulletList();

        void addBullet(Bullet&& newBullet);
        void addBullet(const Bullet& newBullet);
        void update();
        void draw(psyqo::GPU& gpu);
};