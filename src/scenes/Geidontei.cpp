#include "Geidontei.hpp"
#include "EASTL/array.h"
#include "psyqo/gte-kernels.hh"
#include "psyqo/gte-registers.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/vector.hh"

#include <psyqo/font.hh>
#include <psyqo/soft-math.hh>

using namespace psyqo::fixed_point_literals;
using namespace psyqo::trig_literals;

mi::Scenes::Geidontei::Geidontei(GameBase& game) 
    : _game(game)
{
}

struct Face {
    uint8_t vertices[4];
    psyqo::Color color;
};

void mi::Scenes::Geidontei::frame() {
    

    // _game
    //     .getSystemFont()
    //     .print(gpu(), "Hello World!", {{.x = 16, .y = 32}}, c);

    auto& ot = _game.getOrderingTable();
    auto& pb = _game.getPrimBuffer();

    int parity = gpu().getParity();

    auto& currentClear = m_clearFragment[parity];

    gpu().getNextClear(currentClear.primitive, m_clearColor);
    gpu().chain(currentClear);

    static constexpr psyqo::Vec3 cubeVerts[8] = {
        {.x = -0.05, .y = -0.05, .z = -0.05}, {.x =  0.05, .y = -0.05, .z = -0.05},  {.x = -0.05, .y =  0.05, .z = -0.05},
        {.x =  0.05, .y =  0.05, .z = -0.05}, {.x = -0.05, .y = -0.05, .z =  0.05},  {.x =  0.05, .y = -0.05, .z =  0.05},
        {.x = -0.05, .y =  0.05, .z =  0.05}, {.x =  0.05, .y =  0.05, .z =  0.05},
    };

    static constexpr struct Face cubeFaces[6] = {
        {.vertices = {0, 1, 2, 3}, .color = {0,   0,   255}}, 
        {.vertices = {6, 7, 4, 5}, .color = {0,   255, 0}},
        {.vertices = {4, 5, 0, 1}, .color = {0,   255, 255}}, 
        {.vertices = {7, 6, 3, 2}, .color = {255, 0,   0}},
        {.vertices = {6, 4, 2, 0}, .color = {255, 0,   255}}, 
        {.vertices = {5, 7, 1, 3}, .color = {255, 255, 0}}
    };

    //translate the cube 512 to the Z axis
    psyqo::GTE::write<psyqo::GTE::Register::TRZ, psyqo::GTE::Unsafe>(1024);

    auto transform = psyqo::SoftMath::generateRotationMatrix33(m_currentAngle, psyqo::SoftMath::Axis::X, _game.m_Trig);
    auto rot = psyqo::SoftMath::generateRotationMatrix33(m_currentAngle, psyqo::SoftMath::Axis::Y, _game.m_Trig);

    psyqo::SoftMath::multiplyMatrix33(transform, rot, &transform);

    psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::Rotation>(transform);

    //place to store our transformed vertices
    eastl::array<psyqo::Vertex, 4> projectedVerts;

    for(int i = 0; i < 6; i++) {
        const auto currentFace = cubeFaces[i];
        //Load vertices into GTE
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V0>( cubeVerts[currentFace.vertices[0]] );
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V1>( cubeVerts[currentFace.vertices[1]] );
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V2>( cubeVerts[currentFace.vertices[2]] );

        //use the matricies loaded into the GTE (translation, rotation, etc.) to do perspective transformation
        psyqo::GTE::Kernels::rtpt();

        //check winding to see if we can skip drawing this face
        // psyqo::GTE::Kernels::nclip();

        // uint32_t nClipResult;
        // psyqo::GTE::read<psyqo::GTE::Register::MAC0>(&nClipResult);

        //face isn't facing us, can skip
        // if(nClipResult <= 0) {
        //     //continue;
        // }

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

    gpu().chain(ot);

    m_currentAngle += 0.005_pi;
}