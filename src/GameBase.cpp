#include "GameBase.hpp"
#include "scenes/Geidontei.hpp"

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

    pushScene(m_currentScene = new mi::Scenes::Geidontei(m_Font));
}