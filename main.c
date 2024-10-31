// Frogger Game
//
// This program was written by Franciscus Feby Etdolo 01085240013
// on 31/10/2024
//
// This program initializes a Frogger-style game board and allows the player to add turtles
// and logs to help Frogger reach the lillypads at the top of the board.

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  CONSTANTS  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided constants
#define SIZE        9
#define TRUE        1
#define FALSE       0

// Provided Enums
enum tile_type { LILLYPAD, BANK, WATER, TURTLE, LOG };

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

// Function prototypes
void initialize_board(struct board_tile board[SIZE][SIZE]);
void place_turtles(struct board_tile board[SIZE][SIZE], int num_turtles);
void print_board(struct board_tile board[SIZE][SIZE]);
char type_to_char(enum tile_type type);

////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {

    printf("Welcome to Frogger Game!\n");

    struct board_tile game_board[SIZE][SIZE];

    // Initialize the game board
    initialize_board(game_board);

    // Read user input and place turtles
    int num_turtles;
    printf("How many turtles? ");
    scanf("%d", &num_turtles);
    place_turtles(game_board, num_turtles);

    // Start the game and print out the gameboard
    printf("Game Started\n");
    print_board(game_board);

    // Enter command loop
    char command;
    int row, start_col, end_col;

    while (1) {
        printf("Enter command: ");
        int result = scanf(" %c", &command);
        
        // Exit if CTRL+C or input is invalid
        if (result == EOF || command == 'q') break;
        
        // Handle log command
        if (command == 'l') {
            scanf("%d %d %d", &row, &start_col, &end_col);

            // Ensure the row and columns are within bounds and check if row has no turtles
            if (row > 0 && row < SIZE - 1 && start_col < SIZE && end_col >= 0) {
                int has_turtle = FALSE;
                for (int col = 0; col < SIZE; col++) {
                    if (game_board[row][col].type == TURTLE) {
                        has_turtle = TRUE;
                        break;
                    }
                }

                // Add logs only if row has no turtles
                if (!has_turtle) {
                    for (int col = (start_col < 0 ? 0 : start_col); col <= (end_col >= SIZE ? SIZE - 1 : end_col); col++) {
                        if (game_board[row][col].type == WATER) {
                            game_board[row][col].type = LOG;
                        }
                    }
                }
            }

            // Print updated game board after command
            print_board(game_board);
        }
    }

    printf("Thank you for playing Frogger Game!\n");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// ADDITIONAL FUNCTIONS /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void initialize_board(struct board_tile board[SIZE][SIZE]) {
    // Set up the top row with alternating LILLYPAD and WATER tiles
    for (int col = 0; col < SIZE; col++) {
        board[0][col].type = (col % 2 == 0) ? LILLYPAD : WATER;
        board[0][col].occupied = FALSE;
    }

    // Set up the middle rows as WATER tiles
    for (int row = 1; row < SIZE - 1; row++) {
        for (int col = 0; col < SIZE; col++) {
            board[row][col].type = WATER;
            board[row][col].occupied = FALSE;
        }
    }

    // Set up the bottom row as BANK tiles with Frogger in the center
    for (int col = 0; col < SIZE; col++) {
        board[SIZE - 1][col].type = BANK;
        board[SIZE - 1][col].occupied = (col == SIZE / 2) ? TRUE : FALSE;
    }
}

void place_turtles(struct board_tile board[SIZE][SIZE], int num_turtles) {
    int row, col;
    for (int i = 0; i < num_turtles; i++) {
        printf("Enter turtle position (row col): ");
        scanf("%d %d", &row, &col);

        // Check if the position is within bounds for turtles
        if (row >= 1 && row <= 7 && col >= 0 && col < SIZE && board[row][col].type == WATER) {
            board[row][col].type = TURTLE;
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
        type_char = 'O';
    } else if (type == BANK) {
        type_char = 'X';
    } else if (type == WATER) {
        type_char = '~';
    } else if (type == TURTLE) {
        type_char = 'T';
    } else if (type == LOG) {
        type_char = 'L';
    }
    return type_char;
}
