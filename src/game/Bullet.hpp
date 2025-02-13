#pragma once

#include "psyqo/fixed-point.hh"
#include "psyqo/ordering-table.hh"
#include "psyqo/primitives/sprites.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/vector.hh"

enum UpdateAction {
    Nothing,
    DeleteFromList,
};

class Bullet {
    public:
        bool alive;
        psyqo::Prim::Sprite8x8 sprite;

        psyqo::Vec2 position;
        //how much gets added to position each frame
        psyqo::Angle rotation;
        psyqo::Vec2 speed;

        Bullet();
        Bullet(psyqo::Vec2 position, psyqo::Angle angle, psyqo::Vec2 speed);

        void update();
        void draw(psyqo::GPU& gpu);

        static void setupBulletDrawing(psyqo::GPU &gpu);
};