#pragma once

#include "EASTL/array.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/vector.h"
#include "psyqo/ordering-table.hh"
#include "psyqo/vector.hh"
#include "src/game/Bullet.hpp"
#include "src/game/BulletList.hpp"

enum ActionType {
    Movement,
    BezierMovement,
    ShootPattern
};

class ActionElement {
    public:
    uint32_t time;
    uint32_t endTime;
    uint32_t repeatEvery;

    ActionType type;

    psyqo::Vec2 newVelocity;
    eastl::vector<Bullet> patternToShoot;
    eastl::array<psyqo::Vec2, 3> bezierCurve;

    ActionElement(uint32_t time, eastl::vector<Bullet> pattern) 
    : patternToShoot(pattern),
      time(time),
      type(ActionType::ShootPattern) {}

    ActionElement(uint32_t time, psyqo::Vec2 newVelocity) 
    : type(ActionType::Movement), 
      time(time),
      newVelocity(newVelocity) {}

    ActionElement(uint32_t time, uint32_t endTime, eastl::array<psyqo::Vec2, 3> curve) 
      : type(ActionType::BezierMovement), 
        time(time),
        endTime(endTime),
        bezierCurve(curve) {}
};

class Enemy {
    public:
    psyqo::Vec2 position;
    psyqo::Vec2 spriteSize;

    eastl::vector<ActionElement> elements;

    //how much gets added to position each frame
    psyqo::Vec2 velocity;
    bool doingBezierMovement;

    uint8_t tpageX;
    uint8_t tpageY;
    uint32_t timeAlive;

    Enemy() : position({}), spriteSize({}), elements({}), velocity({}), tpageX(0), tpageY(0), timeAlive(0){}
    Enemy(psyqo::Vec2 position, psyqo::Vec2 initialVelocity, uint8_t tpageX, uint8_t tpageY);

    UpdateAction update(BulletList& bulletList);
    void draw(psyqo::GPU& gpu);
};