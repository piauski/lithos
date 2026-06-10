#include "block.h"

const Block block_definitions[__count_block_kind] = {
    [BLOCK_AIR] = { 0 },
    [BLOCK_STONE] = {
        .name = "Stone",
        .flags = BLOCK_FLAG_SOLID | BLOCK_FLAG_OCCLUDES,
        .volume = volume_cube_all,
        .textures = {
            ATLAS_INDEX_STONE,
        }
    },
    [BLOCK_STONE_SLAB] = {
        .name = "Stone Slab",
        .flags = 0,
        .volume = volume_slab_bottom,
        .textures = {
            ATLAS_INDEX_STONE,
        }
    },
    [BLOCK_TORCH] = {
        .name = "Torch",
        .flags = 0,
        .volume = {{7,0,7}, {9,10,9}},
        .textures = {
            ATLAS_INDEX_STONE,
        }
    },
    [BLOCK_DIRT] = {
        .name = "Dirt",
        .flags = BLOCK_FLAG_SOLID | BLOCK_FLAG_OCCLUDES,
        .volume = volume_cube_all,
        .textures = {
            ATLAS_INDEX_DIRT,
        }
    },
    [BLOCK_GRASS] = {
        .name = "Grass",
        .flags = BLOCK_FLAG_SOLID | BLOCK_FLAG_OCCLUDES,
        .volume = volume_cube_all,
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

