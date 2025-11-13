/************************************************************/
/* Tetromino shapes                                         */
/* Each shape is defined by a 4-element array containing    */
/* bit-packed coordinates and side flags for the four       */
/* blocks. Format: bits 0-1=X, bits 2-3=Y, bits 4-6=sides */
/* Relative to top left block coordinates (0,0).            */
/* All rotations stored consecutively, accessed by offset.  */
/************************************************************/

#define SIDE_LEFT 0x01
#define SIDE_RIGHT 0x02
#define SIDE_BOTTOM 0x04

// Bit-packed tetromino format (4 bytes per shape vs 12 bytes)
typedef uint8_t packed_tetromino[4];

// Bit manipulation macros for packed format
#define GET_BLOCK_X(block) ((block) & 0x03)
#define GET_BLOCK_Y(block) (((block) >> 2) & 0x03)
#define GET_BLOCK_SIDES(block) (((block) >> 4) & 0x07)
#define PACK_BLOCK(x,y,sides) ((x) | ((y)<<2) | ((sides)<<4))

// All tetromino rotations stored consecutively
packed_tetromino all_tetrominos[] = {
    // O piece (1 rotation) - offset 0
    { 
        PACK_BLOCK(0, 0, SIDE_LEFT),
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(1, 0, SIDE_RIGHT),
        PACK_BLOCK(1, 1, SIDE_RIGHT | SIDE_BOTTOM)
    },
    // I piece (2 rotations) - offset 1
    { 
        PACK_BLOCK(0, 0, SIDE_BOTTOM | SIDE_LEFT),
        PACK_BLOCK(1, 0, SIDE_BOTTOM),
        PACK_BLOCK(2, 0, SIDE_BOTTOM),
        PACK_BLOCK(3, 0, SIDE_BOTTOM | SIDE_RIGHT)
    },
    { 
        PACK_BLOCK(0, 0, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(0, 2, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(0, 3, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM)
    },

    // T piece (4 rotations) - offset 3  
    { 
        PACK_BLOCK(0, 0, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(1, 0, 0),
        PACK_BLOCK(2, 0, SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(1, 1, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM)
    },
    { 
        PACK_BLOCK(1, 0, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(1, 1, SIDE_RIGHT),
        PACK_BLOCK(1, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_BOTTOM)
    },
    { 
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(1, 1, SIDE_BOTTOM),
        PACK_BLOCK(2, 1, SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(1, 0, SIDE_LEFT | SIDE_RIGHT)
    },
    { 
        PACK_BLOCK(0, 0, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(0, 1, SIDE_LEFT),
        PACK_BLOCK(0, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(1, 1, SIDE_RIGHT | SIDE_BOTTOM)
    },

    // S piece (2 rotations) - offset 7
    { 
        PACK_BLOCK(1, 0, SIDE_LEFT),
        PACK_BLOCK(2, 0, SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(1, 1, SIDE_RIGHT | SIDE_BOTTOM)
    },
    { 
        PACK_BLOCK(0, 0, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(1, 1, SIDE_RIGHT),
        PACK_BLOCK(1, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM)
    },

    // Z piece (2 rotations) - offset 9
    { 
        PACK_BLOCK(0, 0, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(1, 0, SIDE_RIGHT),
        PACK_BLOCK(1, 1, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(2, 1, SIDE_RIGHT | SIDE_BOTTOM)
    },
    { 
        PACK_BLOCK(1, 0, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(1, 1, SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(0, 1, SIDE_LEFT),
        PACK_BLOCK(0, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM)
    },

    // J piece (4 rotations) - offset 11
    { 
        PACK_BLOCK(1, 0, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(1, 1, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(1, 2, SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(0, 2, SIDE_LEFT | SIDE_BOTTOM)
    },
    { 
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(1, 1, SIDE_BOTTOM),
        PACK_BLOCK(2, 1, SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(0, 0, SIDE_LEFT | SIDE_RIGHT)
    },
    { 
        PACK_BLOCK(0, 0, SIDE_LEFT),
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(0, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(1, 0, SIDE_RIGHT | SIDE_BOTTOM)
    },
    { 
        PACK_BLOCK(0, 0, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(1, 0, SIDE_BOTTOM),
        PACK_BLOCK(2, 0, SIDE_RIGHT),
        PACK_BLOCK(2, 1, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM)
    },

    // L piece (4 rotations) - offset 15
    { 
        PACK_BLOCK(1, 0, SIDE_RIGHT),
        PACK_BLOCK(1, 1, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(1, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(0, 0, SIDE_LEFT | SIDE_BOTTOM)
    },
    {
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(1, 1, SIDE_BOTTOM),
        PACK_BLOCK(2, 1, SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(2, 0, SIDE_RIGHT | SIDE_LEFT)
    },
    { 
        PACK_BLOCK(0, 0, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_RIGHT),
        PACK_BLOCK(0, 2, SIDE_LEFT | SIDE_BOTTOM),
        PACK_BLOCK(1, 2, SIDE_RIGHT | SIDE_BOTTOM)
    },
    { 
        PACK_BLOCK(0, 0, SIDE_LEFT),
        PACK_BLOCK(1, 0, SIDE_BOTTOM),
        PACK_BLOCK(2, 0, SIDE_RIGHT | SIDE_BOTTOM),
        PACK_BLOCK(0, 1, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM)
    }
};

// Offset table for accessing tetromino rotations (replaces pointer arrays)
uint8_t tetromino_offsets[] = {0, 1, 3, 7, 9, 11, 15};

// Number of rotations per tetromino
uint8_t tetrominos_nb_shapes[] = {1, 2, 4, 2, 2, 4, 4};

// Macro to access a specific tetromino rotation
#define GET_TETROMINO(piece, rotation) (&all_tetrominos[tetromino_offsets[piece] + (rotation)])
