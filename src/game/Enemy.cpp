#include "Enemy.hpp"
#include "psyqo/fixed-point.hh"
#include "psyqo/ordering-table.hh"
#include "psyqo/primitives/control.hh"
#include "psyqo/primitives/sprites.hh"
#include "psyqo/vector.hh"
#include "psyqo/gpu.hh"
#include "src/game/Bullet.hpp"

psyqo::FixedPoint<> fplerp(psyqo::FixedPoint<> a,
    psyqo::FixedPoint<> b,
    psyqo::FixedPoint<> factor)
{
    return a + (b - a) * factor;
}

Enemy::Enemy(psyqo::Vec2 position, psyqo::Vec2 initialVelocity, uint8_t tpageX, uint8_t tpageY) {
    this->position = position;
    this->velocity = initialVelocity;
    this->timeAlive = 0;
    this->tpageX = tpageX;
    this->tpageY = tpageY;
    this->doingBezierMovement = false;
}

void Enemy::draw(psyqo::GPU& gpu) {
    psyqo::Prim::TPage tpage;

    tpage.attr
        .setPageX(this->tpageX)
        .setPageY(this->tpageY)
        .set(psyqo::Prim::TPageAttr::Tex16Bits)
        .set(psyqo::Prim::TPageAttr::SemiTrans::FullBackAndFullFront)
        .enableDisplayArea();

    gpu.sendPrimitive(tpage);

    psyqo::Prim::Sprite sprite {};

    psyqo::Vertex spriteVertex{};

    spriteVertex.x = (position.x - spriteSize.x).integer();
    spriteVertex.y = (position.y - spriteSize.y).integer();

    sprite.position = spriteVertex;

    sprite.size = {};
    sprite.size.x = spriteSize.x.integer();
    sprite.size.y = spriteSize.y.integer();
    sprite.setColor(psyqo::Color{.r = 120});

    sprite.texInfo = { .u = 0, .v = 0 };
    sprite.setSemiTrans();

    gpu.sendPrimitive(sprite);
}

UpdateAction Enemy::update(eastl::vector<Bullet>& bulletList) {
    if(!doingBezierMovement) {
        position += velocity;
    }
    
    timeAlive += 1;

    for(int i = 0; i != elements.size(); i++) {
        auto current = elements[i];

        if(current.type == ActionType::BezierMovement) {
        // if(current.type == ActionType::BezierMovement && timeAlive >= current.time && timeAlive < current.endTime) {
            psyqo::FixedPoint<> t = psyqo::FixedPoint<>(
                psyqo::FixedPoint<>(timeAlive, 0) / psyqo::FixedPoint<>(current.endTime, 0)
            );

            if(t > 0 && t < 1) {
                doingBezierMovement = 1;
                
                auto ax = fplerp(current.bezierCurve[0].x, current.bezierCurve[1].x, t);
                auto bx = fplerp(current.bezierCurve[1].x, current.bezierCurve[2].x, t);
                position.x = fplerp(ax, bx, t);

                auto ay = fplerp(current.bezierCurve[0].y, current.bezierCurve[1].y, t);
                auto by = fplerp(current.bezierCurve[1].y, current.bezierCurve[2].y, t);
                position.y = fplerp(ay, by, t);
            } else {
                doingBezierMovement = false;
            }
            
        } else {
            uint32_t relativeToActionAlive = timeAlive - current.time;
            
            if(current.time == timeAlive ||
                //make sure repeating is on
                (current.repeatEvery != 0 && 
                //see if its time to repeat
                (relativeToActionAlive % current.repeatEvery == 0) && 
                //make sure that the action is started
                (timeAlive > current.time)
            )) {
                switch(current.type) {
                    case ActionType::Movement:
                        velocity = current.newVelocity;
                        break;
                    case ActionType::ShootPattern:
                        uint32_t patternSize = current.patternToShoot.size();
                        for(int i = 0; i != current.patternToShoot.size(); i++) {
                            psyqo::Vec2 relativeToEnemy = (current.patternToShoot[i].position + position) - (spriteSize / 2);
    
                            current.patternToShoot[i].position = relativeToEnemy;
    
                            bulletList.push_back(current.patternToShoot[i]);
                        } 
                        break;
                    // case ActionType::BezierMovement:
                        
                }
            }
        }

    }

    return UpdateAction::Nothing;
}