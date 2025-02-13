#include "Geidontei.hpp"
#include "EASTL/algorithm.h"
#include "EASTL/array.h"
#include "EASTL/vector.h"
#include "psyqo/fixed-point.hh"
#include "psyqo/gte-kernels.hh"
#include "psyqo/gte-registers.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/primitives/control.hh"
#include "psyqo/primitives/quads.hh"
#include "psyqo/primitives/sprites.hh"
#include "psyqo/vector.hh"

#include "src/game/Enemy.hpp"
#include "src/gpu/Rendering.hpp"
#include "src/gte/GteShortcuts.hpp"
#include "src/math/Common.hpp"
#include "src/math/Camera.hpp"
#include "src/math/Object.hpp"


#include <psyqo/font.hh>
#include <psyqo/soft-math.hh>

#include "Marisa.h"



// #include <EASTL/ctype.h>
#include <EASTL/memory.h>
// #include <EASTL/stdint.h>
#include <EASTL/string.h>

#include "../resources/TimFile.h"
#include "../resources/PELLET.h"
#include "../resources/HITBOX.h"
#include "../resources/BACKGROUND.h"

#include "../game/Bullet.hpp"


using namespace psyqo::fixed_point_literals;
using namespace psyqo::trig_literals;

void loadByteArrayTexture(psyqo::GPU& gpu, const uint8_t* data, size_t length, psyqo::Rect region) {
    psyqo::Prim::VRAMUpload upload;
    upload.region = region;

    gpu.sendPrimitive(upload);

    eastl::vector<uint8_t> asVector(data, data + length);

    TimFile tim = readTimFile(asVector);

    for(int i = 0; i != (tim.pixW * tim.pixH) / 2; i += 1) {
        auto current = *( ((uint32_t*)tim.pixels.data()) + i);
        
        gpu.sendRaw(current);
    }

    psyqo::Prim::FlushCache fc;

    gpu.sendPrimitive(fc);
    gpu.waitReady();
} 

mi::Scenes::Geidontei::Geidontei(GameBase& game) 
    : _game(game)
{
    m_cubeObj.position = { 0.0, 0.0, 1.0 };
    m_Camera.position = { 0.0, 0.0, 0.0 };

    psyqo::Rect mariRegion = {.pos = {{.x = 320, .y = 0}}, .size = {{.w = 32, .h = 48}}};
    psyqo::Rect pelletRegion = {.pos = {{.x = 384, .y = 0}}, .size = {{.w = 8, .h = 8}}};
    psyqo::Rect hitboxRegion = {.pos = {{.x = 448, .y = 0}}, .size = {{.w = 8, .h = 8}}};
    psyqo::Rect bgRegion = {.pos = {{.x = 768, .y = 0}}, .size = {{.w = 256, .h = 256}}};

    loadByteArrayTexture(gpu(), BLACKMARI, 3096, mariRegion);
    loadByteArrayTexture(gpu(), PELLET, 152, pelletRegion);
    loadByteArrayTexture(gpu(), HITBOX, 152, hitboxRegion);
    loadByteArrayTexture(gpu(), BACKGROUND, 131096, bgRegion);

    pad.initialize();

    m_playerPosition = { .x = 200, .y = 200 };

    m_bullets.reserve(1024);

    eastl::vector<Bullet> allDirPattern{};

    const psyqo::FixedPoint<> unitVelocity = .25;

    // Bullet bullet{
    //     .position = { .x = 0, .y = 0 },
    //     .velocity = { .x = unitVelocity, .y = unitVelocity }
    // };
    // allDirPattern.push_back(bullet);

    // bullet.velocity = { .x = 0, .y = unitVelocity };
    // allDirPattern.push_back(bullet);

    // bullet.velocity = { .x = -unitVelocity, .y = unitVelocity };
    // allDirPattern.push_back(bullet);

    // bullet.velocity = { .x = -unitVelocity, .y = 0 };
    // allDirPattern.push_back(bullet);

    // bullet.velocity = { .x = -unitVelocity, .y = -unitVelocity };
    // allDirPattern.push_back(bullet);

    // bullet.velocity = { .x = 0, .y = -unitVelocity };
    // allDirPattern.push_back(bullet);

    // bullet.velocity = { .x = 1, .y = -unitVelocity };
    // allDirPattern.push_back(bullet);

    // bullet.velocity = { .x = unitVelocity, .y = 0 };
    // allDirPattern.push_back(bullet);

    
    // allDirPattern.push_back(bullet);

    for(int i = 0; i != 360; i += 15) {
        Bullet bullet(psyqo::Vec2{0, 0}, 0, psyqo::Vec2{1, 1});
        bullet.rotation = psyqo::Angle(i, 0);
        allDirPattern.push_back(bullet);
    }

    uint32_t patternBullets = allDirPattern.size();

    m_playerScore = 0;
    m_playerLives = 3;
    m_immuneFrames = 0;

    //Stage creation

    enemy = Enemy(psyqo::Vec2{.x = 64, .y = 64}, psyqo::Vec2{.x = .5, .y = 0}, 5, 0);

    enemy.spriteSize.x = 32;
    enemy.spriteSize.y = 48;

    auto pattern = ActionElement(60, allDirPattern);
    pattern.repeatEvery = 30;

    enemy.elements.push_back(pattern);

    psyqo::Vec2 p1 = {.x = 0, .y = 120};
    psyqo::Vec2 p2 = {.x = 160, .y = 180};
    psyqo::Vec2 p3 = {.x = 320, .y = 120};

    auto bezierMovement = ActionElement(60, 600, eastl::array<psyqo::Vec2, 3>{{ p1, p2, p3 }});

    enemy.elements.push_back(bezierMovement);
}

struct Face {
    uint8_t vertices[4];
    psyqo::Color color;
};

void mi::Scenes::Geidontei::frame() {
    this->update();
    this->render();
}

void mi::Scenes::Geidontei::update() {
    mi::math::Rotation fp{};

    fp.y = 0.00025; 
    fp.y *= gpu().getFrameCount();

    m_cubeObj.rotation = { m_currentAngle, m_currentAngle };
    m_cubeObj.recalculateWorldMatrix();

    m_Camera.rotation = fp;
    m_Camera.recalculateViewRotationMatrix();

    m_currentAngle += 0.005_pi;

    uint32_t speed = 2;

    if(m_immuneFrames > 0 ) {
        m_immuneFrames--;
    }

    if(
        pad.isButtonPressed(psyqo::AdvancedPad::Pad1a, psyqo::AdvancedPad::L1) ||
        pad.isButtonPressed(psyqo::AdvancedPad::Pad1a, psyqo::AdvancedPad::R1) 
    ) {
        speed = 1;
    }

    if(pad.isButtonPressed(psyqo::AdvancedPad::Pad1a, psyqo::AdvancedPad::Button::Left)) {
        m_playerPosition.x -= speed;
    }
    if(pad.isButtonPressed(psyqo::AdvancedPad::Pad1a, psyqo::AdvancedPad::Button::Right)) {
        m_playerPosition.x += speed;
    }
    if(pad.isButtonPressed(psyqo::AdvancedPad::Pad1a, psyqo::AdvancedPad::Button::Up)) {
        m_playerPosition.y -= speed;
    }
    if(pad.isButtonPressed(psyqo::AdvancedPad::Pad1a, psyqo::AdvancedPad::Button::Down)) {
        m_playerPosition.y += speed;
    }

    enemy.update(m_bullets);

    const uint32_t hitboxSize = 8;

    eastl::vector<Bullet> stillAlive{};

    for(int i = 0; i != m_bullets.size(); i++) {
        UpdateAction updateAction = m_bullets[i].update(m_playerPosition, m_playerScore);

        if(updateAction == UpdateAction::DeleteFromList) {
            continue;
        } else {
            stillAlive.push_back(m_bullets[i]);
        }

        psyqo::Vec2 hitboxVertex{};

        hitboxVertex.x = (m_playerPosition.x - 4);
        hitboxVertex.y = (m_playerPosition.y - 4);

        psyqo::Vec2 bulletVertex{};

        bulletVertex.x = (m_bullets[i].position.x - 4);
        bulletVertex.y = (m_bullets[i].position.y - 4);

#define abs(x)  (x<0)?-x:x

        auto distanceX = abs((hitboxVertex.x - bulletVertex.x));
        auto distanceY = abs((hitboxVertex.y - bulletVertex.y));

        if (distanceX < 6 && distanceY < 6 && m_immuneFrames == 0) {
            // Collision
            m_playerLives--;
            m_immuneFrames = 60;
        }

        if(distanceX < 30 && distanceY < 30) {
            m_playerScore++;
        }
    }

    //remove offscreen bullets
    if(stillAlive.size() > 0) {
        m_bullets.clear();
        m_bullets.assign(stillAlive.begin(), stillAlive.end());
    }
}

void mi::Scenes::Geidontei::inputHandling() {

}

void mi::Scenes::Geidontei::render() {
    auto& ot = _game.getOrderingTable();
    auto& pb = _game.getPrimBuffer();

    pb.reset();

    int parity = gpu().getParity();

    auto& currentClear = m_clearFragment[parity];

    gpu().getNextClear(currentClear.primitive, m_clearColor);
    gpu().chain(currentClear);

    psyqo::Prim::TPage tpage;

    //background tpage
    tpage.attr
        .setPageX(12)
        .setPageY(0)
        .set(psyqo::Prim::TPageAttr::Tex16Bits)
        .set(psyqo::Prim::TPageAttr::SemiTrans::FullBackAndFullFront)
        .enableDisplayArea();

    gpu().sendPrimitive(tpage);

    psyqo::Prim::TexturedQuad backgroundSpriteFirst {};

    backgroundSpriteFirst.pointA = {.x = 0, .y = 0};
    backgroundSpriteFirst.pointB = {.x = 320, .y = 0};
    backgroundSpriteFirst.pointC = {.x = 320, .y = 256};
    backgroundSpriteFirst.pointD = {.x = 0, .y = 256};
    backgroundSpriteFirst.tpage = tpage.attr;
    backgroundSpriteFirst.uvA = psyqo::PrimPieces::UVCoords{0, 0};
    backgroundSpriteFirst.uvB = psyqo::PrimPieces::UVCoords{255, 0};

    auto uvCD = psyqo::PrimPieces::UVCoordsPadded{};
    uvCD.u = 255;
    uvCD.v = 255;

    backgroundSpriteFirst.uvC = uvCD;

    uvCD.u = 0;
    backgroundSpriteFirst.uvD = uvCD;

    gpu().sendPrimitive(backgroundSpriteFirst);

    tpage.attr
        .setPageX(5)
        .setPageY(0)
        .set(psyqo::Prim::TPageAttr::Tex16Bits)
        .set(psyqo::Prim::TPageAttr::SemiTrans::FullBackAndFullFront)
        .enableDisplayArea();

    gpu().sendPrimitive(tpage);

    psyqo::Prim::Sprite sprite {};

    psyqo::Vertex playerVertex{};

    playerVertex.x = (m_playerPosition.x - (MARI_SIZE_W / 2)).integer();
    playerVertex.y = (m_playerPosition.y - (MARI_SIZE_H / 2)).integer();

    sprite.position = playerVertex;

    sprite.size = {{ .x = 32, .y = 48 }};
    sprite.texInfo = { .u = 0, .v = 0 };
    sprite.setSemiTrans();

    if(m_immuneFrames >= 0 && m_immuneFrames % 8 == 0)
    gpu().sendPrimitive(sprite);

    if(
        pad.isButtonPressed(psyqo::AdvancedPad::Pad1a, psyqo::AdvancedPad::L1) ||
        pad.isButtonPressed(psyqo::AdvancedPad::Pad1a, psyqo::AdvancedPad::R1) 
    ) {
        psyqo::Vertex hitboxVertex{};

        hitboxVertex.x = (m_playerPosition.x - 4).integer();
        hitboxVertex.y = (m_playerPosition.y - 4).integer();

        sprite.position = hitboxVertex;
        sprite.size = {{ .x = 8, .y = 8 }};
        sprite.texInfo = { .u = 0, .v = 0 };

        tpage.attr
            .setPageX(7)
            .setPageY(0)
            .set(psyqo::Prim::TPageAttr::Tex16Bits)
            .set(psyqo::Prim::TPageAttr::SemiTrans::FullBackAndFullFront)
            .enableDisplayArea();
        
        gpu().sendPrimitive(tpage);
        gpu().sendPrimitive(sprite);
    }

    enemy.draw(gpu());

    Bullet::setupBulletDrawing(gpu());

    uint32_t bulletCount = m_bullets.size();

    for(int i = 0; i != bulletCount; i++) {
        m_bullets[i].draw(gpu());
    }

    psyqo::Vertex textVertex{};
    textVertex.x = 0;
    textVertex.y = 2;

    _game.getSystemFont().printf(gpu(), textVertex, psyqo::Color{.b = 255}, "SCORE: %d; LIVES: %d; BULLETS: %d", m_playerScore, m_playerLives, bulletCount);

    //send all the fragments and the ordering table to the gpu
    gpu().chain(ot);
}