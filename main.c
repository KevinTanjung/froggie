// Froggie
//
// This program was written by Kelompok 2:
// - FRANCISCUS FEBY ETDOLO - 01085240013
// - LEONARDUS KEVIN TANJUNG - 01085240011
// - RINA RIDWAN MAMAHIT - 01087240002
// - SERAFINA ALMASYAIR - 01087240001
// - TRIANTO - 01085240015
//
// on Semester Ganjil 2024

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
#define MIN(a,b)     ((a) < (b) ? (a) : (b))
#define MAX(a,b)     ((a) > (b) ? (a) : (b))
#define X            0
#define Y            1

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
int is_placeable(int row, int col);
void place_turtles(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]);
void add_log(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]);
void clear_row(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
void remove_log(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
void move_frogger(struct board_tile board[SIZE][SIZE], char command, int last_coordinate[2]);
void occupy(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
void unoccupy(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
void print_board(struct board_tile board[SIZE][SIZE]);
char type_to_char(enum tile_type type);

////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {

    printf("Welcome to Frogger Game!\n");
    struct board_tile game_board[SIZE][SIZE];
    int turtle_row[SIZE] = {FALSE}; // dictionary to help us indicate if certain row has a turtle for O(1) lookup
    int last_coordinate[2]; // (x, y)
    last_coordinate[X] = BANK_ROW;
    last_coordinate[Y] = SIZE / 2;

    init_board(game_board);
    place_turtles(game_board, turtle_row);
    printf("Game Started\n");
    print_board(game_board);

    char command;
    while (1) {
        printf("|--------------------------------------------------------------------|\n");
        printf("| q = quit     |  l = add log    |  c = clear row  |  r = remove log |\n");
        printf("| w = move up  |  a = move left  |  s = move down  |  d = move right |\n");
        printf("|--------------------------------------------------------------------|\n");
        printf("Enter command: ");
        int result = scanf(" %c", &command);
        if (result == EOF || command == 'q') {
            break;
        }

        switch (command) {
            case 'l':
                add_log(game_board, turtle_row);
                break;
            case 'c':
                clear_row(game_board, last_coordinate);
                break;
            case 'r':
                remove_log(game_board, last_coordinate);
                break;
            case 'a':
            case 'w':
            case 's':
            case 'd':
                move_frogger(game_board, command, last_coordinate);
                break;
            default:
                printf("Invalid command!\n");
                continue;
        }
        print_board(game_board);
    }

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
            // last row print bank and frog occupied middle
            else if (i == SIZE - 1) {
                board[i][j].occupied = j == (SIZE / 2);
                board[i][j].type = BANK;
            }
            // print water
            else {
                board[i][j].occupied = 0;
                board[i][j].type = WATER;
            }
        }
    }
}

void place_turtles(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]) {
    // Read user input and place turtles.
    int num_turtles;
    printf("How many turtles? ");
    scanf("%d", &num_turtles);

    // Scan in the turtles, and place them on the map.
    int row, col;
    for (int i = 0; i < num_turtles; i++) {
        scanf("%d %d", &row, &col);

        if (is_placeable(row, col) == TRUE) {
            board[row][col].type = TURTLE;
            turtle_row[row] = TRUE;
        }
    }
}

/**
 * Helper function to allow us easily check if certain item is placeable in the board.
 * Assumption:
 *  1. Lilly Pad Row are prohibited (destination row)
 *  2. Bank Row are prohibited (starting row)
 */
int is_placeable(int row, int col) {
    return row > LILLYPAD_ROW
        && row < BANK_ROW
        && col >= LEFT_COLUMN
        && col <= RIGHT_COLUMN;
}

void add_log(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]) {
    int row, start_col, end_col = -1;
    printf("Adding Log, input [row] [start_col] [end_col]: ");
    scanf("%d %d %d", &row, &start_col, &end_col);

    if (start_col > end_col) {
        printf("Invalid input! [start_col] must be less or equal than [end_col]!\n");
        return;
    }

    // since column range can be out of bounds, but log should still be placed
    // we ensure the input is within bound using MAX and MIN
    start_col = MAX(LEFT_COLUMN, start_col);
    end_col = MIN(RIGHT_COLUMN, end_col);

    // check if placable and is not occupied by turtle
    if (is_placeable(row, start_col) == TRUE
        && is_placeable(row, end_col) == TRUE
        && turtle_row[row] == FALSE
    ) {
        for (int col = start_col; col <= end_col; col++) {
            board[row][col].type = LOG;
        }
    }
}

void clear_row(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2]
) {
    int row;
    printf("Enter row to clear: ");
    scanf("%d", &row);

    // Check if row is out of bounds OR prohibited (LILLYPAD or BANK row)
    if (is_placeable(row, LEFT_COLUMN) == FALSE) {
        printf("Error: Row must be between %d and %d.\n", LILLYPAD_ROW + 1, BANK_ROW - 1);
        return;
    }

    // Check if Frogger is on the row
    if (last_coordinate[X] == row) {
        printf("Error: Cannot clear row %d because Frogger is present.\n", row);
        return;
    }

    // Clear the row by setting all tiles to WATER
    for (int col = 0; col < SIZE; col++) {
        if (board[row][col].type == TURTLE || board[row][col].type == LOG) {
            board[row][col].type = WATER;
        }
    }
    printf("Row %d cleared.\n", row);
}

void remove_log(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2]
) {
    int row, col;
    printf("To remove log(s), please input [row] [column]: ");
    scanf("%d %d", &row, &col);

    //Tile that is not log tile will not be changed.
    if (board[row][col].type != LOG)  {
        printf("There is no log to remove at row %d and column %d.\n", row, col);
        return;
    }

    //To check for Frogger.
    int frogger_present = FALSE;

    int z = col; //Looping to check if Frogger is present on the left.
    while ((z >= LEFT_COLUMN) && board[row][z].type == LOG) {
        if(board[row][z].occupied == TRUE) {
            frogger_present = TRUE; //if present, set frogger_present to TRUE.
        }
        z--;
    }

    int y = col; //Looping to check if Frogger is present on the right.
    while ((y <= RIGHT_COLUMN) && board[row][y].type == LOG) {
        if(board[row][y].occupied == TRUE) {
            frogger_present = TRUE; //if present, set frogger_present to TRUE.
        }
        y++;
    }

    //To remove logs if Frogger is not present on any adjacent logs.
    if (frogger_present == TRUE) {
        printf("Cannot remove logs as a Frogger is present on an adjacent log.\n");
    } else {
        int removal = z;
        while (removal <= y) {
            board[row][removal].type = WATER;
            removal++;
        }
        printf("Log(s) removed on row %d and column %d, and any logs adjacent to it.\n", row, col);
    }
}

void move_frogger(
    struct board_tile board[SIZE][SIZE],
    char command,
    int last_coordinate[2]
) {
    // note: for better readability, we will separate win/lose condition from this function
    // TODO: handle obstacle
    switch (command) {
        case 'w':
            // moving up -> smaller X -> cannot be less than LILLYPAD_ROW
            if (last_coordinate[X] - 1 >= LILLYPAD_ROW) {
                unoccupy(board, last_coordinate);
                last_coordinate[X]--;
                occupy(board, last_coordinate);
            }
            break;
        case 'a':
            // moving left -> smaller Y -> cannot be less than LEFT_COLUMN
            if (last_coordinate[Y] - 1 >= LEFT_COLUMN) {
                unoccupy(board, last_coordinate);
                last_coordinate[Y]--;
                occupy(board, last_coordinate);
            }
            break;
        case 's':
            // moving down -> greater X -> cannot be greater than BANK_ROW
            if (last_coordinate[X] + 1 <= BANK_ROW) {
                unoccupy(board, last_coordinate);
                last_coordinate[X]++;
                occupy(board, last_coordinate);
            }
            break;
        case 'd':
            // moving right -> greater Y -> cannot be greater than RIGHT_COLUMN
            if (last_coordinate[Y] + 1 <= RIGHT_COLUMN) {
                unoccupy(board, last_coordinate);
                last_coordinate[Y]++;
                occupy(board, last_coordinate);
            }
            break;

    default:
        printf("Invalid move command: [%c]\n", command);
        break;
    }
}

void occupy(struct board_tile board[SIZE][SIZE], int last_coordinate[2]) {
    board[last_coordinate[X]][last_coordinate[Y]].occupied = TRUE;
}

void unoccupy(struct board_tile board[SIZE][SIZE], int last_coordinate[2]) {
    board[last_coordinate[X]][last_coordinate[Y]].occupied = FALSE;
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
