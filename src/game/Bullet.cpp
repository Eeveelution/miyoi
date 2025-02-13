#include "Bullet.hpp"
#include "psyqo/gpu.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/primitives/sprites.hh"
#include "psyqo/vector.hh"
// #include "psyqo/literal

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

UpdateAction Bullet::update(psyqo::Vec2& playerPosition, uint32_t& playerScore) {
    position += velocity;
    // position = psyqo::Vec2{.x= 310, .y=230};

    int xInt = position.x.integer();
    int yInt = position.y.integer();

    if(xInt <= -50) {
        return UpdateAction::DeleteFromList;
    }

    if(xInt >= 340) {
        return UpdateAction::DeleteFromList;
    }

    if(yInt >= 250) {
        return UpdateAction::DeleteFromList;
    }

    if(yInt <= -50) {
        return UpdateAction::DeleteFromList;
    }

    return UpdateAction::Nothing;
}