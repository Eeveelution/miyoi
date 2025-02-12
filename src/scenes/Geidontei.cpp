#include "Geidontei.hpp"
#include "EASTL/array.h"
#include "EASTL/vector.h"
#include "psyqo/fixed-point.hh"
#include "psyqo/gte-kernels.hh"
#include "psyqo/gte-registers.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/primitives/control.hh"
#include "psyqo/primitives/sprites.hh"
#include "psyqo/vector.hh"

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

using namespace psyqo::fixed_point_literals;
using namespace psyqo::trig_literals;

mi::Scenes::Geidontei::Geidontei(GameBase& game) 
    : _game(game)
{
    m_cubeObj.position = { 0.0, 0.0, 1.0 };
    m_Camera.position = { 0.0, 0.0, 0.0 };

    psyqo::Rect region = {.pos = {{.x = 512, .y = 0}}, .size = {{.w = 32, .h = 48}}};
    psyqo::Prim::VRAMUpload upload;
    upload.region = region;

    gpu().sendPrimitive(upload);

    psyqo::Color color;
    color.g = 255;

    eastl::vector<uint8_t> data(BLACKMARI, BLACKMARI + sizeof BLACKMARI / sizeof BLACKMARI[0]);

    TimFile tim = readTimFile(data);

    for(int i = 0; i != (32 * 48); i += 1) {
        auto current = *( ((uint32_t*)tim.pixels.data()) + i);
        
        // gpu().sendRaw(marisaImage[i]);
        gpu().sendRaw(current);
    }

    // for(int x = 0; x != 64; x++) {
    //     for(int y = 0; y != 64; y++) {
    //         if(x + y % 2 == 0) {
    //             gpu().sendRaw(0xFF0000FF);
    //         } else {
    //             gpu().sendRaw(0x000000FF);
    //         }
    //     }
    // }

    psyqo::Prim::FlushCache fc;
    gpu().sendPrimitive(fc);
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

    static constexpr psyqo::Vec3 cubeVerts[] = {
        { .x = -0.05, .y = -0.05, .z = -0.05 }, { .x =  0.05, .y = -0.05, .z = -0.05 },  
        { .x = -0.05, .y =  0.05, .z = -0.05 }, { .x =  0.05, .y =  0.05, .z = -0.05 }, 
        { .x = -0.05, .y = -0.05, .z =  0.05 }, { .x =  0.05, .y = -0.05, .z =  0.05 },
        { .x = -0.05, .y =  0.05, .z =  0.05 }, { .x =  0.05, .y =  0.05, .z =  0.05 },
    };

    constexpr mi::gpu::IndexedColoredQuadFace cubeFaces[] = {
        { .vertexIndicies = {0, 1, 2, 3}, .color = {0,   0,   255 }}, 
        { .vertexIndicies = {6, 7, 4, 5}, .color = {0,   255, 0   }},
        { .vertexIndicies = {4, 5, 0, 1}, .color = {0,   255, 255 }}, 
        { .vertexIndicies = {7, 6, 3, 2}, .color = {255, 0,   0   }},
        { .vertexIndicies = {6, 4, 2, 0}, .color = {255, 0,   255 }}, 
        { .vertexIndicies = {5, 7, 1, 3}, .color = {255, 255, 0   }}
    };

    //mi::gte::setCameraObjectMatricies(m_Camera, m_cubeObj, true);
    //mi::gpu::drawIndexedColoredQuads(ot, pb, cubeFaces, 6, cubeVerts);

    psyqo::Prim::TPage tpage;

    tpage.attr.setPageX(8).setPageY(0).set(psyqo::Prim::TPageAttr::Tex16Bits).set(psyqo::Prim::TPageAttr::SemiTrans::FullBackAndFullFront).enableDisplayArea();
    gpu().sendPrimitive(tpage);

    psyqo::Prim::Sprite sprite {};

    sprite.position = {{ .x = 0, .y = 0 }};
    sprite.size = {{ .x = 32, .y = 48 }};
    sprite.texInfo = { .u = 0, .v = 0 };
    sprite.setSemiTrans();

    gpu().sendPrimitive(sprite);
    

    // auto& quad = pb.allocateFragment<psyqo::Prim::Quad>();

    // quad.primitive
    //     .setPointA(psyqo::Vertex{.x = 0, .y = 0})
    //     .setPointB(psyqo::Vertex{.x = 50, .y = 0})
    //     .setPointC(psyqo::Vertex{.x = 0, .y = 50})
    //     .setPointD(psyqo::Vertex{.x = 50, .y = 50})
    //     .setColor(psyqo::Color{.r = 255, .g = 128})
    //     .setOpaque();

    // ot.insert(quad, 0);

    //place to store our transformed vertices
    /*
    eastl::array<psyqo::Vertex, 4> projectedVerts;

    for(int i = 0; i < 6; i++) {
        const auto currentFace = cubeFaces[i];
        //Load vertices into GTE
        mi::gte::setInputVertices(
            cubeVerts[currentFace.vertices[0]], 
            cubeVerts[currentFace.vertices[1]], 
            cubeVerts[currentFace.vertices[2]]
        );

        //use the matricies loaded into the GTE (translation, rotation, etc.) to do perspective transformation
        psyqo::GTE::Kernels::rtpt();

        //check winding to see if we can skip drawing this face
        psyqo::GTE::Kernels::nclip();

        uint32_t nClipResult;
        psyqo::GTE::read<psyqo::GTE::Register::MAC0>(&nClipResult);

        //face isn't facing us, can skip
        if(nClipResult <= 0) {
            continue;
        }

        //so, the GTE can only work on 3 vertices at a time, a quad has 4
        //so we first save the first vertex, then we can load the 4th one in its place
        //and project a single vertex.
        psyqo::GTE::read<psyqo::GTE::Register::SXY0>(&projectedVerts[0].packed);

        //load in last vertex
        psyqo::GTE::writeSafe<psyqo::GTE::PseudoRegister::V0>( cubeVerts[currentFace.vertices[3]] );

        //perform perspective transformation on single vertex
        //fun fact, this takes in V0 and returns into Registers::SXY2 /shrug
        psyqo::GTE::Kernels::rtps();

        //get the average Z, to put into our ordering table
        psyqo::GTE::Kernels::avsz4();

        uint32_t zIndex;
        psyqo::GTE::read<psyqo::GTE::Register::OTZ>(&zIndex);

        //make sure the Z index can fit into our ordering table
        //if not (i.e this returns true) skip drawing this face
        if(zIndex < 0 || zIndex >= GameBase::OT_SIZE) {
            continue;
        }

        //read remaining vertices
        psyqo::GTE::read<psyqo::GTE::Register::SXY0>(&projectedVerts[1].packed);
        psyqo::GTE::read<psyqo::GTE::Register::SXY1>(&projectedVerts[2].packed);
        psyqo::GTE::read<psyqo::GTE::Register::SXY2>(&projectedVerts[3].packed);

        auto& currentQuad = m_quadFragments[i];

        currentQuad.primitive.setPointA(projectedVerts[0]);
        currentQuad.primitive.setPointB(projectedVerts[1]);
        currentQuad.primitive.setPointC(projectedVerts[2]);
        currentQuad.primitive.setPointD(projectedVerts[3]);

        currentQuad.primitive.setColor(currentFace.color);
        currentQuad.primitive.setOpaque();

        ot.insert(currentQuad, zIndex);
    }
    */

    //send all the fragments and the ordering table to the gpu
    gpu().chain(ot);
}