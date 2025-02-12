#include "Geidontei.hpp"
#include "EASTL/array.h"
#include "EASTL/vector.h"
#include "psyqo/advancedpad.hh"
#include "psyqo/fixed-point.hh"
#include "psyqo/gte-kernels.hh"
#include "psyqo/gte-registers.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/primitives/control.hh"
#include "psyqo/primitives/sprites.hh"
#include "psyqo/vector.hh"

#include "src/game/Bullet.hpp"
#include "src/gpu/Rendering.hpp"
#include "src/gte/GteShortcuts.hpp"
#include "src/math/Common.hpp"
#include "src/math/Camera.hpp"
#include "src/math/Object.hpp"


#include <psyqo/font.hh>
#include <psyqo/soft-math.hh>

#include "Marisa.h"
#include "../resources/HITBOX.H"
#include "../resources/PELLET.H"



// #include <EASTL/ctype.h>
#include <EASTL/memory.h>
// #include <EASTL/stdint.h>
#include <EASTL/string.h>

#include "../resources/TimFile.h"

using namespace psyqo::fixed_point_literals;
using namespace psyqo::trig_literals;

void loadByteArrayTexture(psyqo::GPU& gpu, const uint8_t* data, size_t length, psyqo::Rect region) {
    psyqo::Prim::VRAMUpload upload;
    upload.region = region;

    gpu.sendPrimitive(upload);

    eastl::vector<uint8_t> asVector(data, data + length);

    TimFile tim = readTimFile(asVector);

    for(int i = 0; i != (tim.pixW * tim.pixH); i += 1) {
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
    // psyqo::Prim::VRAMUpload upload;
    // upload.region = region;

    // gpu().sendPrimitive(upload);

    psyqo::Color color;
    color.g = 255;

    loadByteArrayTexture(gpu(), BLACKMARI, 3096, mariRegion);
    loadByteArrayTexture(gpu(), PELLET, 152, pelletRegion);
    loadByteArrayTexture(gpu(), HITBOX, 152, hitboxRegion);

    

    pad.initialize();
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

#define PLAYER_SPEED 2

    uint32_t speed = 2;

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

    //Draw Marisa
    psyqo::Prim::TPage tpage;

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

    Bullet bullet{
        .position = { .x = 64, .y = 64 },
        .velocity = { .x = 1, .y = 1}
    };

    Bullet::setupBulletDrawing(gpu());

    bullet.draw(gpu());
    
    //send all the fragments and the ordering table to the gpu
    gpu().chain(ot);
}