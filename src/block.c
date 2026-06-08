#include "block.h"

const Block block_definitions[__count_block_type] = {
    [BLOCK_AIR] = { .is_transparent = true },
    [BLOCK_STONE] = {
        .volume = volume_full_cube,
        .is_solid = true,
    },
    [BLOCK_STONE_SLAB_BOTTOM] = {
        .volume = volume_slab_bottom,
        .is_solid = false,
    },
};

