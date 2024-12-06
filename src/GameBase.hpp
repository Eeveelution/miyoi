#pragma once

#include "psyqo/scene.hh"
#include "psyqo/trigonometry.hh"
#include "third_party/nugget/psyqo/application.hh"
#include "third_party/nugget/psyqo/font.hh"

#include <psyqo/bump-allocator.h>

class GameBase final : public psyqo::Application {
public:
    static constexpr int OT_SIZE = 1024;
    static constexpr int PRIMBUFFLEN = 1000;
private:
    psyqo::Font<> m_Font;
    psyqo::Scene* m_currentScene;

    using OrderingTableType = psyqo::OrderingTable<OT_SIZE>;
    using PrimBufferAllocatorType = psyqo::BumpAllocator<PRIMBUFFLEN>;

    eastl::array<OrderingTableType, 2> orderingTables;
    eastl::array<PrimBufferAllocatorType, 2> primBuffers;
public:
    void prepare() override;
    void createScene() override;

    OrderingTableType& getOrderingTable() { return orderingTables[gpu().getParity()]; }
    PrimBufferAllocatorType& getPrimBuffer() { return primBuffers[gpu().getParity()]; }
    psyqo::Font<>& getSystemFont() { return m_Font; }
};