#pragma once

#include "psyqo/fixed-point.hh"
#include "psyqo/ordering-table.hh"
#include "psyqo/vector.hh"

class Bullet {
    public:
        psyqo::Vec2 position;
        //how much gets added to position each frame
        psyqo::Vec2 velocity;

        void update();
        void draw(psyqo::GPU& gpu);

        static void setupBulletDrawing(psyqo::GPU &gpu);
};

