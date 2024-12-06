#pragma once

#include "psyqo/bump-allocator.h"
#include "psyqo/ordering-table.hh"

namespace mi::gpu {
    static constexpr int OT_SIZE = 1024;
    static constexpr int PRIMBUFFLEN = 1000;

    using OrderingTableType = psyqo::OrderingTable<OT_SIZE>;
    using PrimBufferAllocatorType = psyqo::BumpAllocator<PRIMBUFFLEN>;
}