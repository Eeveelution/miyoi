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
#include "psyqo/trigonometry.hh"
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
#include "../resources/PELLET.H"
#include "../resources/HITBOX.H"
#include "../resources/BACKGROUND.H"
#include "../resources/FAIRIES.H"

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
    psyqo::Rect fairiesRegion = {.pos = {{.x = 320, .y = 256}}, .size = {{.w = 62, .h = 58}}};

    loadByteArrayTexture(gpu(), BLACKMARI, 3096, mariRegion);
    loadByteArrayTexture(gpu(), PELLET, 152, pelletRegion);
    loadByteArrayTexture(gpu(), HITBOX, 152, hitboxRegion);
    loadByteArrayTexture(gpu(), BACKGROUND, 131096, bgRegion);
    loadByteArrayTexture(gpu(), FAIRIES, 7216, fairiesRegion);

    pad.initialize();

    m_playerPosition = { .x = 200, .y = 200 };

    m_playerScore = 0;
    m_playerLives = 3;
    m_immuneFrames = 0;

    //Stage creation
    createStage();

    background1y = 0;
    background2y = 256;

    time = 0;
    paused = false;
}

void mi::Scenes::Geidontei::createStage() {
    eastl::vector<Bullet> allDirPattern{};

    const psyqo::FixedPoint<> unitVelocity = .25;

    for(int i = 0; i != 360; i += 60) {
        Bullet bullet(psyqo::Vec2{0, 0}, 0, psyqo::Vec2{1, 1});
        bullet.rotation = psyqo::Angle(i, 0);
        bullet.rotationChangeOverTime = psyqo::Angle(0, 75);
        allDirPattern.push_back(bullet);
    }

    //Creating the 2 fairies going across the stage
    Enemy enemy = Enemy(psyqo::Vec2{.x = -64, .y = -64}, psyqo::Vec2{.x = 0, .y = 0}, 5, 1);
    enemy.spriteSize.x = 30;
    enemy.spriteSize.y = 30;
    {    
        auto pattern = ActionElement(60, allDirPattern);
        pattern.repeatEvery = 30;
    
        enemy.elements.push_back(pattern);
    
        psyqo::Vec2 p1 = {.x = 0, .y = 120};
        psyqo::Vec2 p2 = {.x = 160, .y = 180};
        psyqo::Vec2 p3 = {.x = 320, .y = 120};
    
        auto bezierMovementLr = ActionElement(60, 600, eastl::array<psyqo::Vec2, 3>{{ p1, p2, p3 }});
    
        enemy.elements.push_back(bezierMovementLr);
        enemy.elements.push_back(ActionElement(ActionType::Deactivate, 610));
        
        enemies.push_back(enemy);
        
        auto bezierMovementRl= ActionElement(60, 600, eastl::array<psyqo::Vec2, 3>{{ p3, p2, p1 }});
        
        enemy.elements.clear();
        
        enemy.elements.push_back(pattern);
        enemy.elements.push_back(bezierMovementRl);
        enemy.elements.push_back(ActionElement(ActionType::Deactivate, 610));
    
        enemies.push_back(enemy);
    }

    enemy.elements.clear();
    
    //creating 2 fairies coming from the top
    {
        psyqo::Vec2 p1 = {.x = 96, .y = -40};
        psyqo::Vec2 p2 = {.x = 96, .y = 64};
        psyqo::Vec2 p3 = {.x = 96, .y = 64};

        ActionElement deactivate = ActionElement(ActionType::Deactivate, 1200);

        auto offscreenAppearance = ActionElement(800, 60, eastl::array<psyqo::Vec2, 3>{{ p1, p2, p3 }});
        auto disappearOffscreen = ActionElement(1040, 120, eastl::array<psyqo::Vec2, 3>{{ p3, p2, p1 }});
        
        enemy.elements.push_back(offscreenAppearance);
        enemy.elements.push_back(disappearOffscreen);
        enemy.elements.push_back(deactivate);

        eastl::vector<Bullet> straightDownPattern;

        Bullet b = Bullet(psyqo::Vec2{0, 0}, 110, psyqo::Vec2{.x = 1.5, .y = 1.5});
        b.rotationChangeOverTime = psyqo::Angle(0, 75);
        straightDownPattern.push_back(b);

        b.rotation = 90;
        b.rotationChangeOverTime = psyqo::Angle(0, 0);
        straightDownPattern.push_back(b);

        b.rotation = 70;
        b.rotationChangeOverTime = psyqo::Angle(0, -75);
        straightDownPattern.push_back(b);

        auto patternShoot = ActionElement(735, straightDownPattern);
        patternShoot.repeatEvery = 24;
        patternShoot.length = 500;

        enemy.elements.push_back(patternShoot);

        enemies.push_back(enemy);

        enemy.elements.clear();

        p1.x = p2.x = p3.x = 224;

        offscreenAppearance = ActionElement(800, 60, eastl::array<psyqo::Vec2, 3>{{ p1, p2, p3 }});
        disappearOffscreen = ActionElement(1040, 120, eastl::array<psyqo::Vec2, 3>{{ p3, p2, p1 }});

        enemy.elements.push_back(offscreenAppearance);
        enemy.elements.push_back(disappearOffscreen);
        enemy.elements.push_back(patternShoot);
        enemy.elements.push_back(deactivate);

        enemies.push_back(enemy);
    }

    //many fairies both sides circular pattern
    {
        eastl::vector<Bullet> straightDownPattern;
        eastl::vector<Bullet> straightDownPatternR;

        Bullet b = Bullet(psyqo::Vec2{0, 0}, 60, psyqo::Vec2{.x = 1.5, .y = 1.5});
        // b.rotationChangeOverTime = psyqo::Angle(-1, 25);
        straightDownPattern.push_back(b);

        b = Bullet(psyqo::Vec2{0, 0}, 120, psyqo::Vec2{.x = 1.5, .y = 1.5});
        // b.rotationChangeOverTime = psyqo::Angle(1, 25);
        straightDownPatternR.push_back(b);

        auto patternShoot = ActionElement(1300, straightDownPattern);
        patternShoot.repeatEvery = 30;
        patternShoot.length = 800;

        auto patternShootR = ActionElement(1300, straightDownPatternR);
        patternShootR.repeatEvery = 30;
        patternShootR.length = 800;

        psyqo::Vec2 p1 = {.x = -64, .y = 120};
        psyqo::Vec2 p2 = {.x = 150, .y = 120};
        psyqo::Vec2 p3 = {.x = 150, .y = -64};

        psyqo::Vec2 rp1 = {.x = 384, .y = 120};
        psyqo::Vec2 rp2 = {.x = 170, .y = 120};
        psyqo::Vec2 rp3 = {.x = 170, .y = -64};

        for(int i = 0; i != 16; i++) {
            if(i % 2 == 0) {
                enemy.uv = {.u = 32, .v = 0};
            } else {
                enemy.uv = {.u = 0, .v = 0};
            }
            
            auto disappearOffscreen = ActionElement(1200 + (i * 30), 360, eastl::array<psyqo::Vec2, 3>{{ p1, p2, p3 }});
            auto disappearOffscreenR = ActionElement(1200 + (i * 30), 360, eastl::array<psyqo::Vec2, 3>{{ rp1, rp2, rp3 }});
            
            enemy.elements.clear();

            enemy.elements.push_back(disappearOffscreen);
            enemy.elements.push_back(patternShoot);
            enemy.elements.push_back(ActionElement(ActionType::Deactivate, 1200 + (i * 30) + 600));

            enemies.push_back(enemy);

            enemy.elements.clear();

            enemy.elements.push_back(disappearOffscreenR);
            enemy.elements.push_back(patternShootR);
            enemy.elements.push_back(ActionElement(ActionType::Deactivate, 1200 + (i * 30) + 600));

            enemies.push_back(enemy);
        }
    }

    const int positionList[] = { 71 ,94 ,60 ,112 ,117 ,114 ,64, 31 ,86, 34 ,97 ,11 ,48, 7 ,44 ,1 ,111 ,78, 82 ,28, 17 ,51 ,5 ,107 ,76 ,93 ,102 ,3, 119 ,115, 57 ,106, 26, 55, 11, 90 };

    for(int i = 0; i != 32; i++) {
        if(i % 2 == 0) {
            eastl::vector<Bullet> straightDownPattern;

            Bullet b0 = Bullet(psyqo::Vec2{0, 0}, 40, psyqo::Vec2{.x = 1.5, .y = 1.5}); straightDownPattern.push_back(b0);
            Bullet b1 = Bullet(psyqo::Vec2{0, 0}, 50, psyqo::Vec2{.x = 1.5, .y = 1.5}); straightDownPattern.push_back(b1);
            Bullet b2 = Bullet(psyqo::Vec2{0, 0}, 60, psyqo::Vec2{.x = 1.5, .y = 1.5}); straightDownPattern.push_back(b2);
            Bullet b3 = Bullet(psyqo::Vec2{0, 0}, 70, psyqo::Vec2{.x = 1.5, .y = 1.5}); straightDownPattern.push_back(b3);
            Bullet b4 = Bullet(psyqo::Vec2{0, 0}, 80, psyqo::Vec2{.x = 1.5, .y = 1.5}); straightDownPattern.push_back(b4);
    
            enemy.elements.clear();
            enemy.elements.push_back(ActionElement(1900 + (i * 30), straightDownPattern));

            enemy.position = {.x = -64, .y = psyqo::FixedPoint<>(positionList[i], 0) };
            enemy.elements.push_back(ActionElement(1800 + (i * 30), psyqo::Vec2{.x = 1, .y = 0}));
        } else {
            eastl::vector<Bullet> straightDownPattern;

            Bullet b0 = Bullet(psyqo::Vec2{0, 0}, 100, psyqo::Vec2{.x = 1.5, .y = 1.5}); straightDownPattern.push_back(b0);
            Bullet b1 = Bullet(psyqo::Vec2{0, 0}, 110, psyqo::Vec2{.x = 1.5, .y = 1.5}); straightDownPattern.push_back(b1);
            Bullet b2 = Bullet(psyqo::Vec2{0, 0}, 120, psyqo::Vec2{.x = 1.5, .y = 1.5}); straightDownPattern.push_back(b2);
            Bullet b3 = Bullet(psyqo::Vec2{0, 0}, 130, psyqo::Vec2{.x = 1.5, .y = 1.5}); straightDownPattern.push_back(b3);
            Bullet b4 = Bullet(psyqo::Vec2{0, 0}, 140, psyqo::Vec2{.x = 1.5, .y = 1.5}); straightDownPattern.push_back(b4);
    
            enemy.elements.clear();
            enemy.elements.push_back(ActionElement(1900 + (i * 30), straightDownPattern));
    
            enemy.position = {.x = 384, .y = psyqo::FixedPoint<>(positionList[i], 0) };
            enemy.elements.push_back(ActionElement(1800 + (i * 30), psyqo::Vec2{.x = -1, .y = 0}));
        }

        enemy.elements.push_back(ActionElement(ActionType::Deactivate, 3300));

        enemies.push_back(enemy);
    }
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
    if(pad.isButtonPressed(psyqo::AdvancedPad::Pad1a, psyqo::AdvancedPad::Button::Start) && (time - lastStartPress) > 30) {
        paused = !paused;
        lastStartPress = time;
    }

    if(!paused) {
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
    
        for(int i = 0; i != enemies.size(); i++) {
            enemies[i].update(m_bulletList);
        }
    
        m_bulletList.update();
    
        const uint32_t hitboxSize = 4;
    
        for(int i = 0; i != BULLET_LIST_SIZE; i++) {
            auto current = m_bulletList.m_bullets[i];
    
            if(!current.alive) {
                continue;
            }
    
            psyqo::Vec2 hitboxVertex{};
    
            hitboxVertex.x = (m_playerPosition.x - 4);
            hitboxVertex.y = (m_playerPosition.y - 4);
    
            psyqo::Vec2 bulletVertex{};
    
            bulletVertex.x = (current.position.x - 4);
            bulletVertex.y = (current.position.y - 4);
    
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
    }
}

void mi::Scenes::Geidontei::inputHandling() {

}

void mi::Scenes::Geidontei::render() {
    int parity = gpu().getParity();

    auto& currentClear = m_clearFragment[parity];

    gpu().getNextClear(currentClear.primitive, m_clearColor);
    gpu().chain(currentClear);

    if(!paused) {
        background1y++;
        background2y++;
    
        if(background1y >= 256) {
            background1y = -255;
        }
    
        if(background2y >= 256) {
            background2y = -255;
        }
    }


    psyqo::Prim::TPage tpage;

    //background tpage
    tpage.attr
        .setPageX(12)
        .setPageY(0)
        .set(psyqo::Prim::TPageAttr::Tex16Bits)
        .set(psyqo::Prim::TPageAttr::SemiTrans::FullBackAndFullFront)
        .enableDisplayArea();

    gpu().sendPrimitive(tpage);

    psyqo::Prim::TexturedQuad backgroundSprite {};

    backgroundSprite.pointA = {.x = 0, .y = static_cast<int16_t>(0 + background1y)};
    backgroundSprite.pointB = {.x = 320, .y = static_cast<int16_t>(0 + background1y)};
    backgroundSprite.pointC = {.x = 0, .y = static_cast<int16_t>(256 + background1y)};
    backgroundSprite.pointD = {.x = 320, .y = static_cast<int16_t>(256 + background1y)};
    backgroundSprite.tpage = tpage.attr;
    backgroundSprite.uvA = psyqo::PrimPieces::UVCoords{0, 0};
    backgroundSprite.uvB = psyqo::PrimPieces::UVCoords{255, 0};

    auto uvCD = psyqo::PrimPieces::UVCoordsPadded{};
    uvCD.u = 0;
    uvCD.v = 255;

    backgroundSprite.uvC = uvCD;

    uvCD.u = 255;
    backgroundSprite.uvD = uvCD;

    gpu().sendPrimitive(backgroundSprite);

    //2nd scrolling part of the background
    backgroundSprite.pointA = {.x = 0, .y = static_cast<int16_t>(0 + background2y)};
    backgroundSprite.pointB = {.x = 320, .y = static_cast<int16_t>(0 + background2y)};
    backgroundSprite.pointC = {.x = 0, .y = static_cast<int16_t>(256 + background2y)};
    backgroundSprite.pointD = {.x = 320, .y = static_cast<int16_t>(256 + background2y)};

    gpu().sendPrimitive(backgroundSprite);

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

    for(int i = 0; i != enemies.size(); i++) {
        enemies[i].draw(gpu());
    }

    Bullet::setupBulletDrawing(gpu());

    m_bulletList.draw(gpu());

    psyqo::Vertex textVertex{};
    textVertex.x = 0;
    textVertex.y = 2;

    _game.getSystemFont().printf(gpu(), textVertex, psyqo::Color{.r = 255,  .g = 255, .b = 255}, "SCORE: %d; LIVES: %d; TIME: %d", m_playerScore, m_playerLives, time);

    if(paused) {
        _game.getSystemFont().print(gpu(), "PAUSED", psyqo::Vertex{.x = 140, .y = 100}, psyqo::Color{.r = 255,  .g = 255, .b = 255});
    }

    time++;
}