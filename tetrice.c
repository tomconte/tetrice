#include <stdint.h>
#include <stdio.h>

#include "platform.h"
#include "tetromino.h"

// Playfield dimensions
#define PLAYFIELD_WIDTH 12
#define PLAYFIELD_HEIGHT 22

// Game state structure
typedef struct {
    uint8_t playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];
    uint8_t score;
    uint8_t level;
    uint8_t speed;
    uint8_t piece;
    uint8_t x, y;
    uint8_t rotation;
} game_state_t;

// Colors for each tetromino
char tetrominos_colors[] = {
    yellow, cyan, pink, green, red, blue, orange};

// Playfield bounds etc.
#define START_X 20
#define START_Y 2
#define BOUNDS_X1 15
#define BOUNDS_X2 26

/************************************************************/
/* Platform specific functions are implemented elsewhere     */
/************************************************************/

uint8_t timeout_ticks = 0;

/************************************************************/
/* Pieces                                                   */
/************************************************************/

// Display a piece
void display_piece(unsigned char piece, unsigned char x, unsigned char y, unsigned char rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    unsigned char i;
    color(tetrominos_colors[piece], black);
    for (i = 0; i < 4; i++)
    {
        printc(x + (*tetromino)[i][0], y + (*tetromino)[i][1], '\x7F');
    }
}

// Erase a piece
void erase_piece(unsigned char piece, unsigned char x, unsigned char y, unsigned char rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    unsigned char i;
    color(black, black);
    for (i = 0; i < 4; i++)
    {
        printc(x + (*tetromino)[i][0], y + (*tetromino)[i][1], ' ');
    }
}

// Detect collision left
uint8_t collision_left(uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    uint8_t i;

    for (i = 0; i < 4; i++)
    {
        if ((*tetromino)[i][2] & SIDE_LEFT)
        {
            if (charatxy(x + (*tetromino)[i][0] - 1, y + (*tetromino)[i][1]) != ' ')
                return 1;
        }
    }

    return 0;
}

// Detect collision right
uint8_t collision_right(uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    uint8_t i;

    for (i = 0; i < 4; i++)
    {
        if ((*tetromino)[i][2] & SIDE_RIGHT)
        {
            if (charatxy(x + (*tetromino)[i][0] + 1, y + (*tetromino)[i][1]) != ' ')
                return 1;
        }
    }

    return 0;
}

// Detect collision bottom
uint8_t collision_bottom(uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    uint8_t i;

    for (i = 0; i < 4; i++)
    {
        if ((*tetromino)[i][2] & SIDE_BOTTOM)
        {
            if (charatxy(x + (*tetromino)[i][0], y + (*tetromino)[i][1] + 1) != ' ')
                return 1;
        }
    }

    return 0;
}

// Check if a piece can rotate by checking collisions
uint8_t check_rotation(uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation, uint8_t direction)
{
    uint8_t new_rotation;
    tetromino *tetromino;
    uint8_t i;

    // Erase piece
    erase_piece(piece, x, y, rotation);

    // Rotate piece
    if (direction == 0)
    {
        if (rotation < tetrominos_nb_shapes[piece] - 1)
            new_rotation = rotation + 1;
        else
            new_rotation = 0;
    }
    else
    {
        if (rotation > 0)
            new_rotation = rotation - 1;
        else
            new_rotation = tetrominos_nb_shapes[piece] - 1;
    }

    // Check collision
    tetromino = tetrominos[piece][new_rotation];
    for (i = 0; i < 4; i++)
    {
        if (charatxy(x + (*tetromino)[i][0], y + (*tetromino)[i][1]) != ' ')
        {
            // Collision
            // Restore piece
            display_piece(piece, x, y, rotation);
            return rotation;
        }
    }

    // No collision
    return new_rotation;
}

// Check full lines and return score
uint8_t check_full_lines()
{
    uint8_t x, y, z;
    uint8_t full;
    uint8_t nlines = 0;

    for (y = 2; y < 24; y++)
    {
        full = 1;
        for (x = BOUNDS_X1; x <= BOUNDS_X2; x++)
        {
            if (charatxy(x, y) == ' ')
            {
                full = 0;
                break;
            }
        }
        if (full)
        {
            // Erase line
            for (x = BOUNDS_X1; x <= BOUNDS_X2; x++)
            {
                printc(x, y, ' ');
            }
            // Move lines down
            for (x = BOUNDS_X1; x <= BOUNDS_X2; x++)
            {
                for (z = y; z > 2; z--)
                {
                    printc(x, z, charatxy(x, z - 1));
                }
            }
            // Increment nulber of lines
            nlines++;
        }
    }

    // Return score based on value of nlines
    // 1 line = 1 point
    // 2 lines = 3 points
    // 3 lines = 5 points
    // 4 lines = 8 points
    switch (nlines)
    {
    case 1:
        return 1;
    case 2:
        return 3;
    case 3:
        return 5;
    case 4:
        return 8;
    default:
        return 0;
    }
}

// Convert int to a three char string with leading zeros
void int_to_string(uint8_t score, char *str)
{
    str[0] = '0' + (score / 100);
    str[1] = '0' + ((score % 100) / 10);
    str[2] = '0' + (score % 10);
    str[3] = '\0';
}

/************************************************************/
/* Game state initialization                                */
/************************************************************/

void init_game_state(game_state_t* state)
{
    uint8_t x, y;
    
    // Clear playfield
    for (y = 0; y < PLAYFIELD_HEIGHT; y++) {
        for (x = 0; x < PLAYFIELD_WIDTH; x++) {
            state->playfield[y][x] = 0;
        }
    }
    
    // Initialize game variables
    state->score = 0;
    state->level = 1;
    state->speed = 15;
    state->piece = platform_random() % 7;
    state->x = START_X;
    state->y = START_Y;
    state->rotation = 0;
}

/************************************************************/
/* Game loop                                                */
/************************************************************/

void gameloop()
{
    // All variable declarations must be at the top for C89 compatibility
    game_state_t state;
    unsigned char px, py, protation;
    unsigned char line_score;
    input_action_t input;
    unsigned char print_str[4];
    input_action_t prev_input;
    unsigned char bounce;
    
    // Initialize game state
    init_game_state(&state);
    
    // Initialize other variables
    px = state.x;
    py = state.y;
    protation = 0;
    line_score = 0;
    prev_input = INPUT_NONE;
    bounce = 0;

    // Set initial timer
    timeout_ticks = state.speed;

    // Display initial score
    int_to_string(state.score, print_str);
    prints(BOUNDS_X2+3, 3, print_str);

    // Display initial level
    int_to_string(state.level, print_str);
    prints(BOUNDS_X2+3, 6, print_str);

    // Loop until game over
    while (1)
    {
        // Keep previous position
        px = state.x;
        py = state.y;
        protation = state.rotation;

        // Get input action
        input = platform_get_input();

        // Piece falls
        if (input == INPUT_TIMEOUT || input == INPUT_DROP)
        {
            // No fall in the first lines
            if (input == INPUT_DROP && state.y < 3)
                continue;

            // Piece has reached the bottom or another piece
            if (collision_bottom(state.piece, state.x, state.y, state.rotation))
            {
                // Check for full lines
                line_score = check_full_lines();
                if (line_score > 0)
                {
                    // Accelerate speed every 10 points
                    // The score can increase by more than 1 point
                    // so we need to check if the score passed a multiple of 10
                    if ((state.score / 10) != ((state.score + line_score) / 10))
                    {
                        state.level++;
                        if (state.speed > 1)
                            state.speed -= 1;
                    }

                    // Update score
                    state.score += line_score;

                    // Display score
                    int_to_string(state.score, print_str);
                    color(tetrominos_colors[state.piece], black);
                    prints(BOUNDS_X2+3, 3, print_str);

                    // Display level
                    int_to_string(state.level, print_str);
                    color(tetrominos_colors[state.piece], black);
                    prints(BOUNDS_X2+3, 6, print_str);
                }

                // Reset position
                state.x = START_X;
                state.y = START_Y;
                state.rotation = 0;
                px = state.x;
                py = state.y;
                protation = state.rotation;

                // Select a random piece
                state.piece = platform_random() % 7;

                // Set initial timer
                timeout_ticks = state.speed;

                // Check for game over
                if (collision_bottom(state.piece, state.x, state.y, state.rotation))
                {
                    // Game over
                    color(white, black);
                    prints(BOUNDS_X1+1, 10, "GAME  OVER");
                    ticks(15);
                    // Wait for key
                    wait_key();
                    ticks(5);
                    return;
                }

                // Continue loop
                continue;
            }
            // Move piece down
            state.y++;
            timeout_ticks = state.speed;
        }
        else
        {
            // Anti-bounce checks
            // If the same input is pressed, ignore it for a number of iterations
            #define LATERAL_SKIP 40
            #define ROTATION_SKIP 90
            if (input == prev_input)
            {
                if (((input == INPUT_MOVE_LEFT || input == INPUT_MOVE_RIGHT) && bounce > LATERAL_SKIP) || 
                    ((input == INPUT_ROTATE_CW || input == INPUT_ROTATE_CCW) && bounce > ROTATION_SKIP))
                    bounce = 0;
                else
                {
                    bounce++;
                    continue;
                }
            }
            else
            {
                prev_input = input;
                bounce = 0;
            }
        }

        // Move piece
        switch (input)
        {
        case INPUT_MOVE_LEFT:
            if (collision_left(state.piece, state.x, state.y, state.rotation) == 0)
                state.x--;
            break;
        case INPUT_MOVE_RIGHT:
            if (collision_right(state.piece, state.x, state.y, state.rotation) == 0)
                state.x++;
            break;
        case INPUT_ROTATE_CW:
            state.rotation = check_rotation(state.piece, state.x, state.y, state.rotation, 0);
            break;
        case INPUT_ROTATE_CCW:
            state.rotation = check_rotation(state.piece, state.x, state.y, state.rotation, 1);
            break;
        default:
            break;
        }

        // Erase piece
        erase_piece(state.piece, px, py, protation);

        // Display piece
        display_piece(state.piece, state.x, state.y, state.rotation);
    }
}

/************************************************************/
/* Main loop                                                */
/************************************************************/

void main()
{
    unsigned char y;

    while (1)
    {

        // Clear screen
        color(white, black);
        for (y = 0; y < 25; y++)
        {
            prints(0, y, "                                        ");
        }

        // Print message
        color(yellow, black);
        prints(9, 0, "Tetris + Alice = TETRICE");

        // Title
        color(yellow, black);
        printsg(0, 2, "\x6b\x41\x77\x41\x6b\x41\x66\x55\x55\x57\x41\x77\x41");
        printsg(0, 3, "\x4a\x40\x4d\x44\x4a\x40\x45\x45\x45\x4d\x44\x4d\x44");

        // Instructions
        color(pink, black);
        prints(2, 11, "O: LEFT");
        prints(2, 12, "P: RIGHT");
        prints(2, 13, "Z: ROTATE");
        prints(2, 14, "A: UNROTATE");
        prints(2, 15, "SPACE: DROP");

        // Draw background
        color(magenta, black);
        for (y = 0; y < 23; y++)
        {
            printcg(BOUNDS_X1 - 1, 2+y, '\x6A');
            printcg(BOUNDS_X2 + 1, 2+y, '\x55');
        }
        prints(BOUNDS_X1 - 1, 24, "\x42\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x41");
        prints(BOUNDS_X1 - 1, 1, "\x60\x70\x70\x70\x70\x70\x70\x70\x70\x70\x70\x70\x70\x50");

        color(white, black);
        prints(BOUNDS_X2+3, 2, "SCORE");
        prints(BOUNDS_X2+3, 5, "LEVEL");

        // Welcome message and wait to start game
        color(white, black);
        prints(BOUNDS_X1+1, 10, "PRESS  KEY");
        wait_key();
        ticks(5);
        prints(BOUNDS_X1+1, 10, "          ");

        // Call game loop
        gameloop();
    }
}
