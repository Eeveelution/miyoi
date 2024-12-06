#include "Geidontei.hpp"
#include "EASTL/array.h"
#include "psyqo/fixed-point.hh"
#include "psyqo/gte-kernels.hh"
#include "psyqo/gte-registers.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/vector.hh"

#include "src/gte/GteShortcuts.hpp"
#include "src/math/Common.hpp"
#include "src/math/Camera.hpp"
#include "src/math/Object.hpp"

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
    this->update();
    this->render();

    mi::math::Rotation fp{};

    fp.y = 0.0005; 
    fp.y *= gpu().getFrameCount();

    m_Camera.position = {0.25, 0.0, 0.0};
    m_Camera.rotation = fp;

    m_Camera.viewRotationMtx = psyqo::SoftMath::generateRotationMatrix33(m_Camera.rotation.y, psyqo::SoftMath::Axis::Y, mi::math::TrigTable);
    const auto xRot = psyqo::SoftMath::generateRotationMatrix33(m_Camera.rotation.x, psyqo::SoftMath::Axis::X, mi::math::TrigTable);

    psyqo::SoftMath::multiplyMatrix33(m_Camera.viewRotationMtx, xRot, &m_Camera.viewRotationMtx);

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
    // psyqo::GTE::write<psyqo::GTE::Register::TRX, psyqo::GTE::Unsafe>(1);
    // psyqo::GTE::write<psyqo::GTE::Register::TRY, psyqo::GTE::Unsafe>(1);
    // psyqo::GTE::write<psyqo::GTE::Register::TRZ, psyqo::GTE::Unsafe>(1024);

    //model matrix
    // auto transform = psyqo::SoftMath::generateRotationMatrix33(m_currentAngle, psyqo::SoftMath::Axis::X, mi::math::TrigTable);
    // auto rot = psyqo::SoftMath::generateRotationMatrix33(m_currentAngle, psyqo::SoftMath::Axis::Y, mi::math::TrigTable);

    //apply camera
    // psyqo::SoftMath::multiplyMatrix33(transform, m_Camera.viewRotationMtx, &transform);
    //apply object transform
    // psyqo::SoftMath::multiplyMatrix33(transform, rot, &transform);

    // psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::Rotation>(transform);

    //CAMERA POS

    // auto objPos = psyqo::Vec3{0.00, 0.01, 1};

    // auto diffCamPosToObj= objPos - m_Camera.position;

    // auto test = objPos.z - m_Camera.position.z;
    // auto test2 = test.integer();

    // psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::Translation>(diffCamPosToObj);

    // mi::gte::setPerspectiveCameraObjectMatricies(_game.m_Trig, m_Camera.position, m_Camera.viewRotationMtx, objPos, mi::math::Rotation());

    //END CAMERA POS

    mi::math::Object cube = {
        .position = {0, 0, 1},
        .rotation = {m_currentAngle , m_currentAngle},
    };

    cube.recalculateWorldMatrix();

    mi::gte::setCameraObjectMatricies(m_Camera, cube, true);

    //place to store our transformed vertices
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
        // psyqo::GTE::Kernels::nclip();

        // uint32_t nClipResult;
        // psyqo::GTE::read<psyqo::GTE::Register::MAC0>(&nClipResult);

        //face isn't facing us, can skip
        // if(nClipResult <= 0) {
            // continue;
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

void mi::Scenes::Geidontei::update() {

}

void mi::Scenes::Geidontei::render() {
    
}