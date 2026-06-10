#include "block.h"

const Block block_definitions[__count_block_type] = {
    [BLOCK_AIR] = { .is_transparent = true },
    [BLOCK_STONE] = {
        .volume = volume_full_cube,
        .is_solid = true,
        .simple_texture = true,
        .textures = {
            ATLAS_INDEX_STONE,
        }
    },
    [BLOCK_STONE_SLAB] = {
        .volume = volume_slab_bottom,
        .is_transparent = true,
        .simple_texture = true,
        .textures = {
            ATLAS_INDEX_STONE,
        }
    },
    [BLOCK_TORCH] = {
        .volume = {{7,0,7}, {9,10,9}},
        .is_transparent = true,
        .simple_texture = true,
        .textures = {
            ATLAS_INDEX_STONE,
        }
    },
    [BLOCK_DIRT] = {
        .volume = volume_full_cube,
        .is_solid = true,
        .simple_texture = true,
        .textures = {
            ATLAS_INDEX_DIRT,
        }
    },
    [BLOCK_GRASS] = {
        .volume = volume_full_cube,
        .is_solid = true,
        .textures = {
            [BLOCK_FACE_POS_Y] = ATLAS_INDEX_GRASS_TOP,
            [BLOCK_FACE_NEG_Y] = ATLAS_INDEX_DIRT,
            [BLOCK_FACE_POS_X] = ATLAS_INDEX_GRASS_SIDE,
            [BLOCK_FACE_NEG_X] = ATLAS_INDEX_GRASS_SIDE,
            [BLOCK_FACE_POS_Z] = ATLAS_INDEX_GRASS_SIDE,
            [BLOCK_FACE_NEG_Z] = ATLAS_INDEX_GRASS_SIDE,
        }
    },
};

