#include "Bullet.hpp"
#include "psyqo/gpu.hh"
#include "psyqo/primitives/sprites.hh"

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
    psyqo::Prim::Sprite sprite {};

    hitboxVertex.x = (position.x - 4).integer();
    hitboxVertex.y = (position.y - 4).integer();

    sprite.position = hitboxVertex;
    sprite.size = {{ .x = 8, .y = 8 }};
    sprite.texInfo = { .u = 0, .v = 0 };
    sprite.setSemiTrans();

    gpu.sendPrimitive(sprite);
}

void Bullet::update() {
    position += velocity;
}