#pragma once

#include "psyqo/scene.hh"
#include "psyqo/trigonometry.hh"
#include "src/gpu/Common.hpp"
#include "third_party/nugget/psyqo/application.hh"
#include "third_party/nugget/psyqo/font.hh"

#include <psyqo/bump-allocator.h>

class GameBase final : public psyqo::Application {
public:

private:
    psyqo::Font<> m_Font;
    psyqo::Scene* m_currentScene;

    eastl::array<mi::gpu::OrderingTableType, 2> orderingTables;
    eastl::array<mi::gpu::PrimBufferAllocatorType, 2> primBuffers;
public:
    void prepare() override;
    void createScene() override;

    mi::gpu::OrderingTableType& getOrderingTable() { return orderingTables[gpu().getParity()]; }
    mi::gpu::PrimBufferAllocatorType& getPrimBuffer() { return primBuffers[gpu().getParity()]; }
    psyqo::Font<>& getSystemFont() { return m_Font; }
};