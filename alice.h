/************************************************************/
/* Alice 32/90                                              */
/************************************************************/

// EF9345 registers
#define R0 0xBF20     // Command
#define R1 0xBF21     // Argument
#define R2 0xBF22     // Argument
#define R3 0xBF23     // Argument
#define R4 0xBF24     // Auxiliary Pointer (AP) high
#define R5 0xBF25     // Auxiliary Pointer (AP) low
#define R6 0xBF26     // Main Pointer (MP) high
#define R7 0xBF27     // Main Pointer (MP) low
#define R0EXEC 0xBF28 // Execute

// Color codes
#define black 0
#define red 1
#define green 2
#define orange 3
#define blue 4
#define magenta 5
#define cyan 6
#define pink 7
#define lgreen 10
#define yellow 11
#define lmagenta 13
#define white 15

// This is the key matrix, based on the table in "Les Astuces d'Alice" p. 18.
// The table below is organized by columns, and each column is a list of keys.
// The first 7 elements of each row are the keys, the last one is a 0 sentinel.
// NB: the position for the 'X' is wrong in the book, it's fixed here.
uint8_t keys_per_column[8][8] = {
    {'@', 'H', 'P', 'X', '0', '8', 0 /*(control)*/, 0},
    {'Q', 'I', 'A', 'Y', '1', '9', '_', 0},
    {'B', 'J', 'R', 'W', '2', ':', 0 /*(break)*/, 0},
    {'C', 'K', 'S', '_', '3', 'M', 0 /*(left)*/, 0},
    {'D', 'L', 'T', '_', '4', ',', 0 /*(right)*/, 0},
    {'E', '/', 'U', '_', '5', '-', 0 /*(down)*/, 0},
    {'F', 'N', 'V', 0 /*(enter)*/, '6', '.', 0 /*(up)*/, 0},
    {'G', 'O', 'Z', ' ', '7', '+', 0 /*(shift)*/, 0},
};

// This is the list of columns to scan, in order.
uint8_t all_key_columns[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
