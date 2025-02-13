#pragma once

#include "psyqo/fixed-point.hh"
#include "psyqo/ordering-table.hh"
#include "psyqo/vector.hh"

enum UpdateAction {
    Nothing,
    DeleteFromList,
};

class Bullet {
    public:
        psyqo::Vec2 position;
        //how much gets added to position each frame
        psyqo::Vec2 velocity;

        UpdateAction update(psyqo::Vec2& playerPosition, uint32_t& playerScore);
        void draw(psyqo::GPU& gpu);

        static void setupBulletDrawing(psyqo::GPU &gpu);
};