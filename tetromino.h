/************************************************************/
/* Tetromino shapes                                         */
/* Each shape is defined by a 4-element array containing    */
/* (x,y) coordinates of the four blocks for the shape,      */
/* relative to the top left block of coordinates (0,0).     */
/* The array also has a third value which is a bit array    */
/* that indicate which side of the shape a block is located */
/* for the purpose of collision detection.                  */
/* For each shape we have an array containing the           */
/* possible rotations.                                      */
/************************************************************/

#define SIDE_LEFT 0x01
#define SIDE_RIGHT 0x02
#define SIDE_BOTTOM 0x04

typedef uint8_t tetromino[4][3];

/*
* XX
* XX
*/
tetromino tetromino_O = {
    {0, 0, SIDE_LEFT},
    {0, 1, SIDE_LEFT | SIDE_BOTTOM},
    {1, 0, SIDE_RIGHT},
    {1, 1, SIDE_RIGHT | SIDE_BOTTOM}
};

tetromino *tetromino_O_shapes[] = {&tetromino_O};

/*
* XXXX
*/
tetromino tetromino_I_1 = {
    {0, 0, SIDE_BOTTOM | SIDE_LEFT},
    {1, 0, SIDE_BOTTOM},
    {2, 0, SIDE_BOTTOM},
    {3, 0, SIDE_BOTTOM | SIDE_RIGHT}
};

/*
* X
* X
* X
* X
*/
tetromino tetromino_I_2 = {
    {0, 0, SIDE_LEFT | SIDE_RIGHT},
    {0, 1, SIDE_LEFT | SIDE_RIGHT},
    {0, 2, SIDE_LEFT | SIDE_RIGHT},
    {0, 3, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM}
};

tetromino *tetromino_I_shapes[] = {&tetromino_I_1, &tetromino_I_2};

/*
* XXX
*  X
*/
tetromino tetromino_T_1 = {
    {0, 0, SIDE_LEFT | SIDE_BOTTOM},
    {1, 0, 0},
    {2, 0, SIDE_RIGHT | SIDE_BOTTOM},
    {1, 1, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM},
};

/*
*  X
* XX
*  X
*/
tetromino tetromino_T_2 = {
    {1, 0, SIDE_LEFT | SIDE_RIGHT},
    {1, 1, SIDE_RIGHT},
    {1, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM},
    {0, 1, SIDE_LEFT | SIDE_BOTTOM},
};

/*
*  X
* XXX
*/
tetromino tetromino_T_3 = {
    {0, 1, SIDE_LEFT | SIDE_BOTTOM},
    {1, 1, SIDE_BOTTOM},
    {2, 1, SIDE_RIGHT | SIDE_BOTTOM},
    {1, 0, SIDE_LEFT | SIDE_RIGHT},
};


/*
* X
* XX
* X
*/
tetromino tetromino_T_4 = {
    {0, 0, SIDE_LEFT | SIDE_RIGHT},
    {0, 1, SIDE_LEFT},
    {0, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM},
    {1, 1, SIDE_RIGHT | SIDE_BOTTOM}
};

tetromino *tetromino_T_shapes[] = {&tetromino_T_1, &tetromino_T_2, &tetromino_T_3, &tetromino_T_4};

/*
*  XX
* XX
*/
tetromino tetromino_S_1 = {
    {1, 0, SIDE_LEFT},
    {2, 0, SIDE_RIGHT | SIDE_BOTTOM},
    {0, 1, SIDE_LEFT | SIDE_BOTTOM},
    {1, 1, SIDE_RIGHT | SIDE_BOTTOM}
};

/*
* X
* XX
*  X
*/
tetromino tetromino_S_2 = {
    {0, 0, SIDE_LEFT | SIDE_RIGHT},
    {0, 1, SIDE_LEFT | SIDE_BOTTOM},
    {1, 1, SIDE_RIGHT},
    {1, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM}
};

tetromino *tetromino_S_shapes[] = {&tetromino_S_1, &tetromino_S_2};

/*
* XX
*  XX
*/
tetromino tetromino_Z_1 = {
    {0, 0, SIDE_LEFT | SIDE_BOTTOM},
    {1, 0, SIDE_RIGHT},
    {1, 1, SIDE_LEFT | SIDE_BOTTOM},
    {2, 1, SIDE_RIGHT | SIDE_BOTTOM}
};

/*
*  X
* XX
* X
*/
tetromino tetromino_Z_2 = {
    {1, 0, SIDE_LEFT | SIDE_RIGHT},
    {1, 1, SIDE_RIGHT | SIDE_BOTTOM},
    {0, 1, SIDE_LEFT},
    {0, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM}
};

tetromino *tetromino_Z_shapes[] = {&tetromino_Z_1, &tetromino_Z_2};

/*
*  X
*  X
* XX
*/
tetromino tetromino_J_1 = {
    {1, 0, SIDE_LEFT | SIDE_RIGHT},
    {1, 1, SIDE_LEFT | SIDE_RIGHT},
    {1, 2, SIDE_RIGHT | SIDE_BOTTOM},
    {0, 2, SIDE_LEFT | SIDE_BOTTOM}
};

/*
* X
* XXX
*/
tetromino tetromino_J_2 = {
    {0, 1, SIDE_LEFT | SIDE_BOTTOM},
    {1, 1, SIDE_BOTTOM},
    {2, 1, SIDE_RIGHT | SIDE_BOTTOM},
    {0, 0, SIDE_LEFT | SIDE_RIGHT}
};

/*
* XX
* X
* X
*/
tetromino tetromino_J_3 = {
    {0, 0, SIDE_LEFT},
    {0, 1, SIDE_LEFT | SIDE_RIGHT},
    {0, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM},
    {1, 0, SIDE_RIGHT | SIDE_BOTTOM},
};

/*
* XXX
*   X
*/
tetromino tetromino_J_4 = {
    {0, 0, SIDE_LEFT | SIDE_BOTTOM},
    {1, 0, SIDE_BOTTOM},
    {2, 0, SIDE_RIGHT},
    {2, 1, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM},
};

tetromino *tetromino_J_shapes[] = {&tetromino_J_1, &tetromino_J_2, &tetromino_J_3, &tetromino_J_4};

/*
* XX
*  X
*  X
*/
tetromino tetromino_L_1 = {
    {1, 0, SIDE_RIGHT},
    {1, 1, SIDE_LEFT | SIDE_RIGHT},
    {1, 2, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM},
    {0, 0, SIDE_LEFT | SIDE_BOTTOM},
};

/*
*   X
* XXX
*/
tetromino tetromino_L_2 = {
    {0, 1, SIDE_LEFT | SIDE_BOTTOM},
    {1, 1, SIDE_BOTTOM},
    {2, 1, SIDE_RIGHT | SIDE_BOTTOM},
    {2, 0, SIDE_RIGHT},
};

/*
* X
* X
* XX
*/
tetromino tetromino_L_3 = {
    {0, 0, SIDE_LEFT | SIDE_RIGHT},
    {0, 1, SIDE_LEFT | SIDE_RIGHT},
    {0, 2, SIDE_LEFT | SIDE_BOTTOM},
    {1, 2, SIDE_RIGHT | SIDE_BOTTOM}
};

/*
* XXX
* X
*/
tetromino tetromino_L_4 = {
    {0, 0, SIDE_LEFT},
    {1, 0, SIDE_BOTTOM},
    {2, 0, SIDE_RIGHT | SIDE_BOTTOM},
    {0, 1, SIDE_LEFT | SIDE_RIGHT | SIDE_BOTTOM}
};

tetromino *tetromino_L_shapes[] = {&tetromino_L_1, &tetromino_L_2, &tetromino_L_3, &tetromino_L_4};

// All tetrominos gathered in one array
tetromino **tetrominos[] = {
    tetromino_O_shapes,
    tetromino_I_shapes,
    tetromino_T_shapes,
    tetromino_S_shapes,
    tetromino_Z_shapes,
    tetromino_J_shapes,
    tetromino_L_shapes};

// Number of rotations shapes per tetromino
char tetrominos_nb_shapes[] = {1, 2, 4, 2, 2, 4, 4};
