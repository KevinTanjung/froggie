// Program name
//
// This program was written by [your name] (NIM i.e. )
// on [date]
//
// TODO: Description of program

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  CONSTANTS  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided constants
#define SIZE        9
#define TRUE        1
#define FALSE       0

// TODO: you may choose to add additional #defines here.

// Provided Enums
enum tile_type {LILLYPAD, BANK, WATER, TURTLE, LOG};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  STRUCTS  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided structs
struct board_tile {
    enum tile_type type; // The type of piece it is (water, bank, etc.)
    int occupied;        // TRUE or FALSE based on if Frogger is there.
};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  FUNCTION PROTOTYPES  ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void init_board(struct board_tile board[SIZE][SIZE]);
void print_board(struct board_tile board[SIZE][SIZE]);
char type_to_char(enum tile_type type);

////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {

    printf("Welcome to Frogger Game!\n");
    struct board_tile game_board[SIZE][SIZE];

    // Initialise the gameboard.
    init_board(game_board);

    // Read user input and place turtles.
    printf("How many turtles? ");
    // TODO (Stage 1.2): Scan in the turtles, and place them on the map.
    printf("Enter pairs:\n");

    // Start the game and print out the gameboard.
    printf("Game Started\n");
    print_board(game_board);

    printf("Enter command: ");
    // TODO (Stage 1.3): Create a command loop, to read and execute commands!

    printf("\nThank you for playing Frogger Game!\n");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// ADDITIONAL FUNCTIONS /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void init_board(struct board_tile board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            // first row
            if (i == 0 && j % 2 == 0) {
                board[i][j].occupied = 0;
                board[i][j].type = LILLYPAD;
            }
            // last row print bank or frog
            else if (i == SIZE - 1) {
                // middle print frog, else bank
                if (j == (SIZE / 2)) {
                    board[i][j].occupied = 1;
                } else {
                    board[i][j].occupied = 0;
                    board[i][j].type = BANK;
                }
            }
            // print water
            else {
                board[i][j].occupied = 0;
                board[i][j].type = WATER;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// PROVIDED FUNCTIONS //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void print_board(struct board_tile board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            char type_char = '\0';
            if (board[row][col].occupied) {
                type_char = 'F';
            } else {
                type_char = type_to_char(board[row][col].type);
            }
            printf("%c ", type_char);
        }
        printf("\n");
    }
}

char type_to_char(enum tile_type type) {
    char type_char = ' ';
    if (type == LILLYPAD) {
        type_char = 'o';
    } else if (type == BANK) {
        type_char = 'x';
    } else if (type == WATER) {
        type_char = '~';
    } else if (type == TURTLE) {
        type_char = 'T';
    } else if (type == LOG) {
        type_char = 'L';
    }
    return type_char;
}
