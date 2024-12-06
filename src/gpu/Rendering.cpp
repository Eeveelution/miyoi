#include "Rendering.hpp"
#include "psyqo/gte-kernels.hh"
#include "psyqo/gte-registers.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/primitives/quads.hh"
#include "src/gpu/Common.hpp"
#include "src/gte/GteShortcuts.hpp"

void mi::gpu::drawIndexedColoredQuads(
    mi::gpu::OrderingTableType& ot,
    mi::gpu::PrimBufferAllocatorType& pb,

    const IndexedColoredQuadFace* quadFaces, 
    uint32_t quadFaceCount,

    const psyqo::Vec3* vertices
) {
    for(int i = 0; i < quadFaceCount; i++) {
        //storage for our finished vertices
        psyqo::Vertex transformedVerts[4];

        const IndexedColoredQuadFace& current = quadFaces[i];

        //first load in the first 3 vertices for transforming
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V0>( vertices[ current.vertexIndicies[0] ] );
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V1>( vertices[ current.vertexIndicies[1] ] );
        psyqo::GTE::writeUnsafe<psyqo::GTE::PseudoRegister::V2>( vertices[ current.vertexIndicies[2] ] );

        //transform the vertices, it is assumed that the matricies required for this to be correct are already loaded
        psyqo::GTE::Kernels::rtpt();

        //check for normal clipping
        psyqo::GTE::Kernels::nclip();

        //read back the clipping result, and clip out if necessary
        if( psyqo::GTE::readRaw<psyqo::GTE::Register::MAC0, psyqo::GTE::Safe>() < 0 ) {
            continue;
        }

        //load back the first vertex, since we need to overwrite it with the 4th vertex of this quad
        psyqo::GTE::read<psyqo::GTE::Register::SXY0>( &transformedVerts[0].packed );

        //load in the 4th vertex to transform.
        psyqo::GTE::writeSafe<psyqo::GTE::PseudoRegister::V0>( vertices[ current.vertexIndicies[3] ] );

        //transform the 4th verties. fun fact: this returns into the SXY2 register
        psyqo::GTE::Kernels::rtps();

        //run the average Z calculation over the last 4 vertices
        psyqo::GTE::Kernels::avsz4();

        //retrieve the average Z over the last 4 vertices
        auto avgZ = psyqo::GTE::readRaw<psyqo::GTE::Register::OTZ, psyqo::GTE::Safe>();

        //check if the Z can fit into our ordering table, if not we have to skip it.
        if(avgZ < 0 || avgZ >= OT_SIZE) {
            continue;
        }

        //read back the remaining vertices
        psyqo::GTE::read<psyqo::GTE::Register::SXY0>( &transformedVerts[1].packed );
        psyqo::GTE::read<psyqo::GTE::Register::SXY1>( &transformedVerts[2].packed );
        psyqo::GTE::read<psyqo::GTE::Register::SXY2>( &transformedVerts[3].packed );

        //allocate quad fragment for rendering

        auto& fragment = pb.allocateFragment<psyqo::Prim::Quad>();

        fragment.primitive
            .setPointA(transformedVerts[0])
            .setPointB(transformedVerts[1])
            .setPointC(transformedVerts[2])
            .setPointD(transformedVerts[3])
            .setColor(current.color)
            .setOpaque();

        //insert this fragment into the ordering table
        ot.insert(fragment, avgZ);
    }
}