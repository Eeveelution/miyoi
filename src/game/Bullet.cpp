#include "Bullet.hpp"
#include "psyqo/fixed-point.hh"
#include "psyqo/gpu.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/primitives/sprites.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/vector.hh"
#include "src/math/Common.hpp"

// #include "psyqo/literal

Bullet::Bullet() {
    alive = false;
}

Bullet::Bullet(psyqo::Vec2 position, psyqo::Angle angle, psyqo::Vec2 speed) {
    this->position = position;
    this->rotation = angle;
    this->speed = speed;

    alive = true;

    sprite = psyqo::Prim::Sprite8x8{};
    sprite.texInfo = { .u = 0, .v = 0 };
    sprite.setSemiTrans();
}

void Bullet::setupBulletDrawing(psyqo::GPU &gpu) {
    psyqo::Prim::TPage tpage;

    tpage.attr
        .setPageX(6)
        .setPageY(0)
        .set(psyqo::Prim::TPageAttr::Tex16Bits)
        .set(psyqo::Prim::TPageAttr::SemiTrans::FullBackAndFullFront)
        .enableDisplayArea();

    gpu.sendPrimitive(tpage);
}

void Bullet::draw(psyqo::GPU &gpu) {
    psyqo::Vertex hitboxVertex{};

    hitboxVertex.x = (position.x - 4).integer();
    hitboxVertex.y = (position.y - 4).integer();

    sprite.position = hitboxVertex;

    gpu.sendPrimitive(sprite);
}

void Bullet::update() {
    psyqo::Vec2 velocity{
        .x = mi::math::TrigTable.cos(this->rotation / 180.0) * speed.x,
        .y = mi::math::TrigTable.sin(this->rotation / 180.0) * speed.y
    };    

    position += velocity;

    int xInt = position.x.integer();
    int yInt = position.y.integer();

    if(xInt <= -50 || xInt >= 340 || yInt >= 250 || yInt <= -50) {
        alive = false;
    }
}