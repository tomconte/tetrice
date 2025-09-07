#include <stdint.h>
#include <stdio.h>

#include "platform.h"
#include "tetromino.h"
#include "game_state.h"

// Function declarations
void playfield_set_cell(game_state_t* state, uint8_t x, uint8_t y, uint8_t color);
uint8_t playfield_get_cell(game_state_t* state, uint8_t x, uint8_t y);
uint8_t playfield_is_empty_cell(game_state_t* state, uint8_t x, uint8_t y);
void playfield_clear(game_state_t* state);
void playfield_place_piece(game_state_t* state, unsigned char piece, unsigned char x, unsigned char y, unsigned char rotation);
void playfield_remove_piece(game_state_t* state, unsigned char piece, unsigned char x, unsigned char y, unsigned char rotation);
uint8_t collision_left(game_state_t* state, uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation);
uint8_t collision_right(game_state_t* state, uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation);
uint8_t collision_bottom(game_state_t* state, uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation);
uint8_t check_rotation(game_state_t* state, uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation, uint8_t direction);
uint8_t check_full_lines(game_state_t* state);
void init_game_state(game_state_t* state);
void display_piece(unsigned char piece, unsigned char x, unsigned char y, unsigned char rotation);
void erase_piece(unsigned char piece, unsigned char x, unsigned char y, unsigned char rotation);

// External reference to platform-specific color mapping
extern char tetrominos_colors[];


/************************************************************/
/* Platform specific functions are implemented elsewhere     */
/************************************************************/

uint8_t timeout_ticks = 0;

/************************************************************/
/* Pieces                                                   */
/************************************************************/

// Place a piece in the playfield
void playfield_place_piece(game_state_t* state, unsigned char piece, unsigned char x, unsigned char y, unsigned char rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    unsigned char i;
    unsigned char px, py;
    
    for (i = 0; i < 4; i++)
    {
        px = x + (*tetromino)[i][0] - PLAYFIELD_START_X;
        py = y + (*tetromino)[i][1] - PLAYFIELD_START_Y;
        playfield_set_cell(state, px, py, CELL_PIECE_1 + piece);
    }
}

// Display a piece (for rendering)
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

// Remove a piece from the playfield
void playfield_remove_piece(game_state_t* state, unsigned char piece, unsigned char x, unsigned char y, unsigned char rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    unsigned char i;
    unsigned char px, py;
    
    for (i = 0; i < 4; i++)
    {
        px = x + (*tetromino)[i][0] - PLAYFIELD_START_X;
        py = y + (*tetromino)[i][1] - PLAYFIELD_START_Y;
        playfield_set_cell(state, px, py, CELL_EMPTY);
    }
}

// Erase a piece (for rendering)
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
uint8_t collision_left(game_state_t* state, uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    uint8_t i;
    uint8_t px, py;

    for (i = 0; i < 4; i++)
    {
        if ((*tetromino)[i][2] & SIDE_LEFT)
        {
            px = x + (*tetromino)[i][0] - 1 - PLAYFIELD_START_X;
            py = y + (*tetromino)[i][1] - PLAYFIELD_START_Y;
            
            if (px >= PLAYFIELD_WIDTH || !playfield_is_empty_cell(state, px, py))
                return 1;
        }
    }

    return 0;
}

// Detect collision right
uint8_t collision_right(game_state_t* state, uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    uint8_t i;
    uint8_t px, py;

    for (i = 0; i < 4; i++)
    {
        if ((*tetromino)[i][2] & SIDE_RIGHT)
        {
            px = x + (*tetromino)[i][0] + 1 - PLAYFIELD_START_X;
            py = y + (*tetromino)[i][1] - PLAYFIELD_START_Y;
            
            if (px >= PLAYFIELD_WIDTH || !playfield_is_empty_cell(state, px, py))
                return 1;
        }
    }

    return 0;
}

// Detect collision bottom
uint8_t collision_bottom(game_state_t* state, uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    uint8_t i;
    uint8_t px, py;

    for (i = 0; i < 4; i++)
    {
        if ((*tetromino)[i][2] & SIDE_BOTTOM)
        {
            px = x + (*tetromino)[i][0] - PLAYFIELD_START_X;
            py = y + (*tetromino)[i][1] + 1 - PLAYFIELD_START_Y;
            
            if (py >= PLAYFIELD_HEIGHT || !playfield_is_empty_cell(state, px, py))
                return 1;
        }
    }

    return 0;
}

// Check if a piece can rotate by checking collisions
uint8_t check_rotation(game_state_t* state, uint8_t piece, uint8_t x, uint8_t y, uint8_t rotation, uint8_t direction)
{
    uint8_t new_rotation;
    tetromino *tetromino;
    uint8_t i;
    uint8_t px, py;

    // Remove piece from playfield
    playfield_remove_piece(state, piece, x, y, rotation);
    
    // Also erase from display
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
        px = x + (*tetromino)[i][0] - PLAYFIELD_START_X;
        py = y + (*tetromino)[i][1] - PLAYFIELD_START_Y;
        
        if (px >= PLAYFIELD_WIDTH || py >= PLAYFIELD_HEIGHT || !playfield_is_empty_cell(state, px, py))
        {
            // Collision
            // Restore piece in playfield and display
            playfield_place_piece(state, piece, x, y, rotation);
            display_piece(piece, x, y, rotation);
            return rotation;
        }
    }

    // No collision
    return new_rotation;
}

// Check full lines and return score
uint8_t check_full_lines(game_state_t* state)
{
    uint8_t x, y, z;
    uint8_t full;
    uint8_t nlines = 0;

    for (y = 0; y < PLAYFIELD_HEIGHT; y++)
    {
        full = 1;
        for (x = 0; x < PLAYFIELD_WIDTH; x++)
        {
            if (playfield_is_empty_cell(state, x, y))
            {
                full = 0;
                break;
            }
        }
        if (full)
        {
            // Clear line in playfield
            for (x = 0; x < PLAYFIELD_WIDTH; x++)
            {
                playfield_set_cell(state, x, y, CELL_EMPTY);
            }
            // Move lines down in playfield
            for (z = y; z > 0; z--)
            {
                for (x = 0; x < PLAYFIELD_WIDTH; x++)
                {
                    uint8_t cell_above = playfield_get_cell(state, x, z - 1);
                    playfield_set_cell(state, x, z, cell_above);
                }
            }
            // Clear top line
            for (x = 0; x < PLAYFIELD_WIDTH; x++)
            {
                playfield_set_cell(state, x, 0, CELL_EMPTY);
            }
            
            
            // Increment number of lines
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
/* Playfield Operations API                                 */
/************************************************************/

void playfield_set_cell(game_state_t* state, uint8_t x, uint8_t y, uint8_t color)
{
    if (x < PLAYFIELD_WIDTH && y < PLAYFIELD_HEIGHT)
        state->playfield[y][x] = color;
}

uint8_t playfield_get_cell(game_state_t* state, uint8_t x, uint8_t y)
{
    if (x < PLAYFIELD_WIDTH && y < PLAYFIELD_HEIGHT)
        return state->playfield[y][x];
    return 0;
}

uint8_t playfield_is_empty_cell(game_state_t* state, uint8_t x, uint8_t y)
{
    return playfield_get_cell(state, x, y) == CELL_EMPTY;
}

void playfield_clear(game_state_t* state)
{
    uint8_t x, y;
    for (y = 0; y < PLAYFIELD_HEIGHT; y++) {
        for (x = 0; x < PLAYFIELD_WIDTH; x++) {
            state->playfield[y][x] = CELL_EMPTY;
        }
    }
}

/************************************************************/
/* Game state initialization                                */
/************************************************************/

void init_game_state(game_state_t* state)
{
    // Clear playfield
    playfield_clear(state);
    
    // Initialize game variables
    state->score = 0;
    state->level = 1;
    state->speed = 15;
    state->piece = platform_random() % 7;
    state->x = PIECE_START_X;
    state->y = PIECE_START_Y;
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

    // Initial display sync
    display_sync_ui(&state);

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
            if (collision_bottom(&state, state.piece, state.x, state.y, state.rotation))
            {
                // Place piece in playfield permanently
                playfield_place_piece(&state, state.piece, state.x, state.y, state.rotation);
                // Check for full lines
                line_score = check_full_lines(&state);
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

                    // Sync UI and playfield display after line clearing
                    display_sync_playfield(&state);
                    display_sync_ui(&state);
                }

                // Reset position
                state.x = PIECE_START_X;
                state.y = PIECE_START_Y;
                state.rotation = 0;
                px = state.x;
                py = state.y;
                protation = state.rotation;

                // Select a random piece
                state.piece = platform_random() % 7;

                // Set initial timer
                timeout_ticks = state.speed;

                // Check for game over
                if (collision_bottom(&state, state.piece, state.x, state.y, state.rotation))
                {
                    // Game over
                    color(white, black);
                    prints(PLAYFIELD_START_X+1, 10, "GAME  OVER");
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
            if (collision_left(&state, state.piece, state.x, state.y, state.rotation) == 0)
                state.x--;
            break;
        case INPUT_MOVE_RIGHT:
            if (collision_right(&state, state.piece, state.x, state.y, state.rotation) == 0)
                state.x++;
            break;
        case INPUT_ROTATE_CW:
            state.rotation = check_rotation(&state, state.piece, state.x, state.y, state.rotation, 0);
            break;
        case INPUT_ROTATE_CCW:
            state.rotation = check_rotation(&state, state.piece, state.x, state.y, state.rotation, 1);
            break;
        default:
            break;
        }

        // Sync entire display
        display_sync_playfield(&state);
        display_sync_current_piece(&state);
    }
}

/************************************************************/
/* Main loop                                                */
/************************************************************/

void main()
{
    while (1)
    {
        // Clear screen and draw borders
        display_clear_screen();
        display_draw_borders();

        // Welcome message and wait to start game
        color(white, black);
        prints(PLAYFIELD_START_X+1, 10, "PRESS  KEY");
        wait_key();
        ticks(5);
        prints(PLAYFIELD_START_X+1, 10, "          ");

        // Call game loop
        gameloop();
    }
}
