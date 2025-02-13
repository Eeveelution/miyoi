#pragma once

#include "psyqo/fixed-point.hh"
#include "psyqo/ordering-table.hh"
#include "psyqo/primitives/sprites.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/vector.hh"

enum class UpdateAction {
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
        psyqo::Angle rotationChangeOverTime;
        psyqo::Vec2 speed;

        Bullet();
        Bullet(psyqo::Vec2 position, psyqo::Angle angle, psyqo::Vec2 speed);
        Bullet(Bullet &&) = default;
        Bullet(const Bullet &) = default;
        Bullet& operator=(const Bullet &) = default;
        Bullet& operator=(Bullet &&) = default;

        UpdateAction update();
        void draw(psyqo::GPU& gpu);

        static void setupBulletDrawing(psyqo::GPU &gpu);
};