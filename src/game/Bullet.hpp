#pragma once

#include "psyqo/fixed-point.hh"
#include "psyqo/ordering-table.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/vector.hh"

enum UpdateAction {
    Nothing,
    DeleteFromList,
};

class Bullet {
    public:
        psyqo::Vec2 position;
        //how much gets added to position each frame
        psyqo::Angle rotation;
        psyqo::Vec2 speed;

        Bullet(psyqo::Vec2 position, psyqo::Angle angle, psyqo::Vec2 speed);

        UpdateAction update(psyqo::Vec2& playerPosition, uint32_t& playerScore);
        void draw(psyqo::GPU& gpu);

        static void setupBulletDrawing(psyqo::GPU &gpu);
};