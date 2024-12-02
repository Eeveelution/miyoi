#include "GameBase.hpp"
#include "psyqo/fixed-point.hh"
#include "scenes/Geidontei.hpp"

#include <psyqo/gte-kernels.hh>
#include <psyqo/gte-registers.hh>
#include <psyqo/gpu.hh>
#include <psyqo/primitives/quads.hh>
#include <psyqo/primitives/triangles.hh>
#include <psyqo/scene.hh>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

void GameBase::prepare() {
    psyqo::GPU::Configuration config;

    config
        .set(psyqo::GPU::Resolution::W320)
        .set(psyqo::GPU::VideoMode::AUTO)
        .set(psyqo::GPU::ColorMode::C15BITS)
        .set(psyqo::GPU::Interlace::PROGRESSIVE);

    gpu().initialize(config);
}

void GameBase::createScene() {
    m_Font.uploadSystemFont(gpu());

    auto centerWidth = psyqo::FixedPoint<16>(160.0);
    auto centerHeight = psyqo::FixedPoint<16>(120.0f);

    //Write screen offset registers to point to the center of the screen
    psyqo::GTE::write<psyqo::GTE::Register::OFX, psyqo::GTE::Unsafe>(centerWidth.raw());
    psyqo::GTE::write<psyqo::GTE::Register::OFY, psyqo::GTE::Unsafe>(centerWidth.raw());

    //clear translation registers, to have a predictable state
    psyqo::GTE::clear<psyqo::GTE::Register::TRX, psyqo::GTE::Unsafe>();
    psyqo::GTE::clear<psyqo::GTE::Register::TRY, psyqo::GTE::Unsafe>();
    psyqo::GTE::clear<psyqo::GTE::Register::TRZ, psyqo::GTE::Unsafe>();

    //projection plane distance (todo: play around with this)
    psyqo::GTE::write<psyqo::GTE::Register::H, psyqo::GTE::Unsafe>(120);

    //specify scaling for Z averaging on the GTE
    psyqo::GTE::write<psyqo::GTE::Register::ZSF3, psyqo::GTE::Unsafe>(OT_SIZE / 3);
    psyqo::GTE::write<psyqo::GTE::Register::ZSF4, psyqo::GTE::Unsafe>(OT_SIZE / 4);

    pushScene(m_currentScene = new mi::Scenes::Geidontei(*this));
}