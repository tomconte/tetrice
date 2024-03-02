/************************************************************/
/* Tetromino shapes                                         */
/* Each shape is defined by a 4-element array containing    */
/* (x,y) coordinates of the four blocks for the shape,      */
/* relative to the top left block of coordinates (0,0).     */
/* For each shape we have an array containing the           */
/* possible rotations.                                      */
/************************************************************/

typedef uint8_t tetromino[4][2];

tetromino tetromino_O = {
    {0, 0},
    {0, 1},
    {1, 0},
    {1, 1},
};

tetromino *tetromino_O_shapes[] = {&tetromino_O};

uint8_t tetromino_O_widths[] = {2};

tetromino tetromino_I_1 = {
    {0, 0},
    {1, 0},
    {2, 0},
    {3, 0},
};

tetromino tetromino_I_2 = {
    {0, 0},
    {0, 1},
    {0, 2},
    {0, 3},
};

tetromino *tetromino_I_shapes[] = {&tetromino_I_1, &tetromino_I_2};

uint8_t tetromino_I_widths[] = {4, 1};

tetromino tetromino_T_1 = {
    {0, 0},
    {1, 0},
    {2, 0},
    {1, 1},
};

tetromino tetromino_T_2 = {
    {0, 0},
    {0, 1},
    {0, 2},
    {1, 1},
};

tetromino tetromino_T_3 = {
    {0, 1},
    {1, 1},
    {2, 1},
    {1, 0},
};

tetromino tetromino_T_4 = {
    {1, 0},
    {1, 1},
    {1, 2},
    {0, 1},
};

uint8_t tetromino_T_widths[] = {3, 2, 3, 2};

tetromino *tetromino_T_shapes[] = {&tetromino_T_1, &tetromino_T_2, &tetromino_T_3, &tetromino_T_4};

tetromino tetromino_S_1 = {
    {1, 0},
    {2, 0},
    {0, 1},
    {1, 1},
};

tetromino tetromino_S_2 = {
    {0, 0},
    {0, 1},
    {1, 1},
    {1, 2},
};

tetromino *tetromino_S_shapes[] = {&tetromino_S_1, &tetromino_S_2};

uint8_t tetromino_S_widths[] = {3, 2};

tetromino tetromino_Z_1 = {
    {0, 0},
    {1, 0},
    {1, 1},
    {2, 1},
};

tetromino tetromino_Z_2 = {
    {1, 0},
    {1, 1},
    {0, 1},
    {0, 2},
};

tetromino *tetromino_Z_shapes[] = {&tetromino_Z_1, &tetromino_Z_2};

uint8_t tetromino_Z_widths[] = {3, 2};

tetromino tetromino_J_1 = {
    {1, 0},
    {1, 1},
    {1, 2},
    {0, 2},
};

tetromino tetromino_J_2 = {
    {0, 0},
    {1, 0},
    {2, 0},
    {2, 1},
};

tetromino tetromino_J_3 = {
    {0, 0},
    {0, 1},
    {0, 2},
    {1, 0},
};

tetromino tetromino_J_4 = {
    {0, 1},
    {1, 1},
    {2, 1},
    {0, 0},
};

tetromino *tetromino_J_shapes[] = {&tetromino_J_1, &tetromino_J_2, &tetromino_J_3, &tetromino_J_4};

uint8_t tetromino_J_widths[] = {2, 3, 2, 3};

tetromino tetromino_L_1 = {
    {1, 0},
    {1, 1},
    {1, 2},
    {0, 0},
};

tetromino tetromino_L_2 = {
    {0, 1},
    {1, 1},
    {2, 1},
    {2, 0},
};

tetromino tetromino_L_3 = {
    {0, 0},
    {0, 1},
    {0, 2},
    {1, 2},
};

tetromino tetromino_L_4 = {
    {0, 0},
    {1, 0},
    {2, 0},
    {0, 1},
};

tetromino *tetromino_L_shapes[] = {&tetromino_L_1, &tetromino_L_2, &tetromino_L_3, &tetromino_L_4};

uint8_t tetromino_L_widths[] = {2, 3, 2, 3};

// All tetrominos gathered in one array
tetromino **tetrominos[] = {
    tetromino_O_shapes,
    tetromino_I_shapes,
    tetromino_T_shapes,
    tetromino_S_shapes,
    tetromino_Z_shapes,
    tetromino_J_shapes,
    tetromino_L_shapes};

// All widths
uint8_t *tetrominos_widths[] = {
    tetromino_O_widths,
    tetromino_I_widths,
    tetromino_T_widths,
    tetromino_S_widths,
    tetromino_Z_widths,
    tetromino_J_widths,
    tetromino_L_widths};

// Number of rotations shapes per tetromino
char tetrominos_nb_shapes[] = {1, 2, 4, 2, 2, 4, 4};
