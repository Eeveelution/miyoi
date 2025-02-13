#include "Bullet.hpp"
#include "psyqo/fixed-point.hh"
#include "psyqo/gpu.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/primitives/sprites.hh"
#include "psyqo/trigonometry.hh"
#include "psyqo/vector.hh"
#include "src/math/Common.hpp"

// #include "psyqo/literal

Bullet::Bullet(psyqo::Vec2 position, psyqo::Angle angle, psyqo::Vec2 speed) {
    this->position = position;
    this->rotation = angle;
    this->speed = speed;
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
    psyqo::Prim::Sprite sprite {};

    hitboxVertex.x = (position.x - 4).integer();
    hitboxVertex.y = (position.y - 4).integer();

    int xInt = position.x.integer();
    int yInt = position.y.integer();

    sprite.position = hitboxVertex;
    sprite.size = {{ .x = 8, .y = 8 }};
    sprite.texInfo = { .u = 0, .v = 0 };
    sprite.setSemiTrans();

    gpu.sendPrimitive(sprite);
}
//0.87841 = 90
//0.54101
void test(char c) {
    if(c == 0) {
        return;
    }
}

UpdateAction Bullet::update(psyqo::Vec2& playerPosition, uint32_t& playerScore) {
    uint32_t rotationInt = rotation.integer();

    // psyqo::Angle rot1 = psyqo::Angle(90.0 / 3.1415 / 180);
    // psyqo::Angle rot = this->rotation / 3.1415 / 180;

    int32_t what = (mi::math::TrigTable.cos(0.0 * (3.1415926535 / 180.0))).integer();
    int32_t what1 = (mi::math::TrigTable.cos(90.0 * (3.1415926535 / 180.0))).integer();
    int32_t what2 = (mi::math::TrigTable.cos(180.0 * (3.1415926535 / 180.0))).integer();
    int32_t what3 = (mi::math::TrigTable.cos(270.0 * (3.1415926535 / 180.0))).integer();

    int32_t wwhat = (mi::math::TrigTable.sin(0.0 * (3.1415926535 / 180.0))).integer();
    int32_t wwhat1 = (mi::math::TrigTable.sin(90.0 * (3.1415926535 / 180.0))).integer();
    int32_t wwhat2 = (mi::math::TrigTable.sin(180.0 * (3.1415926535 / 180.0))).integer();
    int32_t wwhat3 = (mi::math::TrigTable.sin(270.0 * (3.1415926535 / 180.0))).integer();


    ( mi::math::TrigTable.cos(90*3.1415/180).print(test) );
    ( mi::math::TrigTable.cos(180*3.1415/180).print(test) );

    psyqo::Vec2 velocity{
        .x = mi::math::TrigTable.cos(this->rotation / 180.0) * speed.x,
        .y = mi::math::TrigTable.sin(this->rotation / 180.0) * speed.y
    };

    

    int velX = velocity.x.integer();
    int velY = velocity.y.integer();

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