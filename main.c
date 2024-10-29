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
#define SIZE         9
#define TRUE         1
#define FALSE        0
#define LILLYPAD_ROW 0
#define BANK_ROW     (SIZE - 1)
#define LEFT_COLUMN  0
#define RIGHT_COLUMN (SIZE - 1)

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

void place_turtles(struct board_tile board[SIZE][SIZE]);

// Prints out the current state of the board.
void print_board(struct board_tile board[SIZE][SIZE]);
char type_to_char(enum tile_type type);

////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {

    printf("Welcome to Frogger Game!\n");
    struct board_tile game_board[SIZE][SIZE];

    // TODO (Stage 1.1) Initialise the gameboard.

    // Read user input and place turtles.
    place_turtles(game_board);

    // Start the game and print out the gameboard.
    printf("Game Started\n");
    print_board(game_board);

    printf("Enter command: ");
    // TODO (Stage 1.3): Create a command loop, to read and execute commands!

    printf("Thank you for playing Frogger Game!\n");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// ADDITIONAL FUNCTIONS /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void place_turtles(struct board_tile board[SIZE][SIZE]) {
    int turtleCount = 0;
    printf("How many turtles? ");
    scanf("%d", &turtleCount);
    printf("Enter pairs:\n");
    while (turtleCount > 0) {
        int row = -1, column = -1;
        scanf("%d %d", &row, &column);
        if (row > LILLYPAD_ROW && row < BANK_ROW
            && column >= LEFT_COLUMN && column <= RIGHT_COLUMN
        ) {
            board[row][column].occupied = 0;
            board[row][column].type = TURTLE;
        }
        turtleCount--;
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
