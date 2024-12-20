// Froggie
//
// This program was written by Kelompok 2:
// - FRANCISCUS FEBY ETDOLO - 01085240013
// - LEONARDUS KEVIN TANJUNG - 01085240011
// - RINA RIDWAN MAMAHIT - 01087240002
// - SERAFINA ALAMSYAIR - 01087240001
// - TRIANTO - 01085240015
//
// on Semester Ganjil 2024

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <stdarg.h>

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  CONSTANTS  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided constants
#define SIZE          9
#define TRUE          1
#define FALSE         0
#define LILLYPAD_ROW  0
#define BANK_ROW      (SIZE - 1)
#define LEFT_COLUMN   0
#define RIGHT_COLUMN  (SIZE - 1)
#define MIN(a,b)      ((a) < (b) ? (a) : (b))
#define MAX(a,b)      ((a) > (b) ? (a) : (b))
#define X             0
#define Y             1
#define DEFAULT_LIVES 3
#define DEAD          0
#define GAME_WON      -1
#define FOOD_LIMIT    2
#define FOOD_DURATION 5

// ANSI color codes
#define RESET_COLOR     "\033[0m"
#define GREEN_COLOR     "\033[32m"          // Green for Lilly Pad
#define BLUE_COLOR      "\033[34m"          // Blue for Water
#define YELLOW_COLOR    "\033[33m"          // Yellow for Turtle
#define RED_COLOR       "\033[31m"          // Red for Bank
#define BROWN_COLOR     "\033[38;5;130m"    // Brown for Log
#define FROG_COLOR      "\033[35m"          // Magenta for Frog

// Provided Enums
enum tile_type {LILLYPAD, BANK, WATER, TURTLE, LOG, FROG, BUG, FOOD};
enum map_skin {RETRO, COLOR, EMOJI, COLOR_EMOJI};

char GAME_MESSAGE[256] = {0};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  STRUCTS  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided structs
struct board_tile {
    enum tile_type type;  // The type of piece it is (water, bank, etc.)
    int occupied;         // TRUE or FALSE based on if Frogger is there.
    struct bug_node* bug; // POINTER or NULL based on if Bug is there.
    int food_timer;       // To ensure the food will only last for 3 turns.
};

struct bug_node {
    int row;
    int col;
    struct bug_node* prev;
    struct bug_node* next;
    int moving_right;
};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  FUNCTION PROTOTYPES  ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void select_skin(enum map_skin* selected_skin);
void init_board(struct board_tile board[SIZE][SIZE]);
int is_placeable(int row, int col);
void place_turtles(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]);
void add_log(struct board_tile board[SIZE][SIZE], int turtle_row[SIZE]);
void add_bug(struct board_tile board[SIZE][SIZE], struct bug_node** bug_linked_list);
void clear_row(struct board_tile board[SIZE][SIZE], int last_coordinate[2], struct bug_node** bug_linked_list, int turtle_row[SIZE]);
void remove_log(struct board_tile board[SIZE][SIZE], int last_coordinate[2], struct bug_node** bug_linked_list);
void move_frogger(struct board_tile board[SIZE][SIZE], char command, int last_coordinate[2]);
void move_bug(struct board_tile board[SIZE][SIZE], struct bug_node* bug_linked_list);
void occupy(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
void unoccupy(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
int check_winning_condition(struct board_tile board[SIZE][SIZE], int last_coordinate[2], int lives, enum map_skin selected_skin);
void reset_frogger(struct board_tile board[SIZE][SIZE], int last_coordinate[2]);
void print_board(struct board_tile board[SIZE][SIZE], enum map_skin selected_skin);
void print_tile(struct board_tile tile, enum map_skin selected_skin);
struct bug_node* create_bug_node(struct bug_node** bug_linked_list, int x, int y);
void remove_bug_node(struct bug_node** bug_linked_list, struct bug_node* to_be_removed);
void add_food(struct board_tile board[SIZE][SIZE]);
void food_timer(struct board_tile board[SIZE][SIZE]);
void food_superpowers(struct board_tile board[SIZE][SIZE], int last_coordinate[2], int *lives);
void show_help();
void clear_screen();
void shout(const char* format, ...);
void print_message();

////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {
    setlocale(LC_ALL, "");
    srand(time(NULL));

    printf("Welcome to Frogger Game!\n");
    struct board_tile game_board[SIZE][SIZE];
    struct bug_node* bug_linked_list = NULL; // linked list to help us add/remove bug for easy traversal and lookup
    int lives = DEFAULT_LIVES;
    int turtle_row[SIZE] = {FALSE}; // dictionary to help us indicate if certain row has a turtle for O(1) lookup
    int last_coordinate[2] = {BANK_ROW, SIZE / 2}; // (x, y)
    enum map_skin selected_skin = RETRO;
    int frogger_moved = FALSE; //Frogger at the start of the game has not moved yet.

    init_board(game_board);
    select_skin(&selected_skin);
    place_turtles(game_board, turtle_row);
    clear_screen();
    print_board(game_board, selected_skin);
    printf("\n\n## GAME STARTED ##\n\n");
    show_help();

    char command;
    while (lives > DEAD) {
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
                clear_row(game_board, last_coordinate, &bug_linked_list, turtle_row);
                break;
            case 'r':
                remove_log(game_board, last_coordinate, &bug_linked_list);
                break;
            case 'b':
                add_bug(game_board, &bug_linked_list);
                break;
            case 'a':
            case 'w':
            case 's':
            case 'd':
                frogger_moved = TRUE;
                move_frogger(game_board, command, last_coordinate);
                move_bug(game_board, bug_linked_list);
                lives = check_winning_condition(game_board, last_coordinate, lives, selected_skin);
                break;
            case 'h':
                while (getchar() != '\n');
                clear_screen();
                show_help();
                printf("\n\nPress enter to exit the help...");
                scanf("%[^\n]", &command);
                printf("\n\n");
                break;
            case 'm':
                select_skin(&selected_skin);
                break;
            default:
                printf("Invalid command!\n");
                continue;
        }
        if (frogger_moved == TRUE) {
            add_food(game_board);
            food_timer(game_board);
            food_superpowers(game_board, last_coordinate, &lives);
        }
        clear_screen();
        print_board(game_board, selected_skin);
        print_message();
    }

    if (lives == DEAD) {
        printf("!! GAME OVER !!\n");
    } else if (lives == GAME_WON) {
        printf("Wahoo!! You Won!\n");
    }

    printf("\nThank you for playing Frogger Game!\n");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// ADDITIONAL FUNCTIONS /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void select_skin(enum map_skin* selected_skin) {
    int skin_idx = 0;
    printf("Which map theme do you want to use?\n");
    printf(" 1. RETRO: Default, plain black & white.\n");
    printf(" 2. COLOR: Similar symbol with RETRO but with color.\n");
    printf(" 3. EMOJI: Change the symbol to emoji!\n");
    printf(" 4. COLOR_EMOJI: Combination of colored character and emoji!\n");
    printf("Enter your choice (1-4): ");
    scanf("%d", &skin_idx);

    switch (skin_idx) {
        case 2:
            *selected_skin = COLOR;
            break;
        case 3:
            *selected_skin = EMOJI;
            break;
        case 4:
            *selected_skin = COLOR_EMOJI;
            break;
        default:
            *selected_skin = RETRO;
            break;
    }
}

void init_board(struct board_tile board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j].occupied = 0;
            board[i][j].bug = NULL;

            // first row
            if (i == 0 && j % 2 == 0) {
                board[i][j].type = LILLYPAD;
            }
            // last row print bank and frog occupied middle
            else if (i == SIZE - 1) {
                board[i][j].occupied = j == (SIZE / 2);
                board[i][j].type = BANK;
            }
            // print water
            else {
                board[i][j].type = WATER;
            }
        }
    }
}

void place_turtles(
    struct board_tile board[SIZE][SIZE],
    int turtle_row[SIZE]
) {
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

void add_log(
    struct board_tile board[SIZE][SIZE],
    int turtle_row[SIZE]
) {
    int row, start_col, end_col = -1;
    printf("Adding Log, input [row] [start_col] [end_col]: ");
    scanf("%d %d %d", &row, &start_col, &end_col);

    if (start_col > end_col) {
        shout("Invalid input! [start_col] must be less or equal than [end_col]!\n");
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

void add_bug(
    struct board_tile board[SIZE][SIZE],
    struct bug_node** bug_linked_list
) {
    int row, col = -1;
    printf("Adding Bug, input [row] [col]: ");
    scanf("%d %d", &row, &col);

    if ((board[row][col].type != TURTLE && board[row][col].type != LOG)
        || board[row][col].bug != NULL
    ) {
        return;
    }
    board[row][col].bug = create_bug_node(bug_linked_list, row, col);
}

void clear_row(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2],
    struct bug_node** bug_linked_list,
    int turtle_row[SIZE]
) {
    int row;
    printf("Enter row to clear: ");
    scanf("%d", &row);

    // Check if row is out of bounds OR prohibited (LILLYPAD or BANK row)
    if (is_placeable(row, LEFT_COLUMN) == FALSE) {
        shout("Error: Row must be between %d and %d.\n", LILLYPAD_ROW + 1, BANK_ROW - 1);
        return;
    }

    // Check if Frogger is on the row
    if (last_coordinate[X] == row) {
        shout("Error: Cannot clear row %d because Frogger is present.\n", row);
        return;
    }

    // Clear the row by setting all tiles to WATER
    for (int col = 0; col < SIZE; col++) {
        if (board[row][col].type == TURTLE || board[row][col].type == LOG) {
            board[row][col].type = WATER;
        }
        if (board[row][col].bug != NULL) {
            remove_bug_node(bug_linked_list, board[row][col].bug);
            board[row][col].bug = NULL;
        }
    }
    turtle_row[row] = FALSE;
    shout("Row %d cleared.\n", row);
}

void remove_log(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2],
    struct bug_node** bug_linked_list
) {
    int row, col;
    printf("To remove log(s), please input [row] [column]: ");
    scanf("%d %d", &row, &col);

    //Tile that is not log tile will not be changed.
    if (board[row][col].type != LOG)  {
        shout("There is no log to remove at row %d and column %d.\n", row, col);
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
        shout("Cannot remove logs as a Frogger is present on an adjacent log.\n");
    } else {
        int removal = z;
        while (removal <= y) {
            board[row][removal].type = WATER;
            if (board[row][removal].bug != NULL) {
                remove_bug_node(bug_linked_list, board[row][removal].bug);
                board[row][removal].bug = NULL;
            }
            removal++;
        }
        shout("Log(s) removed on row %d and column %d, and any logs adjacent to it.\n", row, col);
    }
}

void move_frogger(
    struct board_tile board[SIZE][SIZE],
    char command,
    int last_coordinate[2]
) {
    unoccupy(board, last_coordinate);
    switch (command) {
        case 'w':
            last_coordinate[X] = MAX(last_coordinate[X] - 1, LILLYPAD_ROW);
            break;
        case 's':
            last_coordinate[X] = MIN(last_coordinate[X] + 1, BANK_ROW);
            break;
        case 'a':
            last_coordinate[Y] = MAX(last_coordinate[Y] - 1, LEFT_COLUMN);
            break;
        case 'd':
            last_coordinate[Y] = MIN(last_coordinate[Y] + 1, RIGHT_COLUMN);
            break;
    }
    occupy(board, last_coordinate);
}


void add_food(struct board_tile board[SIZE][SIZE]) {
    int food_count = 0;
    int row = 0; //To check if the board has a food tile. If there are any, add 1 to the count.
    while (row < SIZE) {
        int col = 0;
        while (col < SIZE) {
            if (board[row][col].type == FOOD) {
                food_count++;
            }
            col++;
        }
        row++;
    }

    //If from the checking above the board has 2 food tiles, do nothing.
    if (food_count >= FOOD_LIMIT) {
        return;
    }
    shout("Food has appeared! Hurry and eat them for superpowers!\n");
    while (food_count < FOOD_LIMIT) {
        int min_row = LILLYPAD_ROW + 1;
        int max_row = BANK_ROW - 1;
        int row = min_row + rand() % (max_row - min_row + 1);
        int col = LEFT_COLUMN + rand() % (RIGHT_COLUMN - LEFT_COLUMN + 1);

        if (board[row][col].type == WATER) //if the selected random tile is a water tile, change it to a food tile.
        {
            board[row][col].type = FOOD;
            // Set the food timer to FOOD_DURATION + 1.
            // The food will expire after FOOD_DURATION (default to 5) turns
            // (beginning of 5th turn, the food is gone).
            board[row][col].food_timer = FOOD_DURATION + 1;
            food_count++;
        }
    }
}

void food_timer(struct board_tile board[SIZE][SIZE]) {
    int row = 0;
    while (row < SIZE) {
        int col = 0;
        while (col < SIZE) { //To check for any food tile on the board.
            if (board[row][col].type == FOOD) { //Should a food tile be found, the food timer will be reduced each turn.
                board[row][col].food_timer--;
                if (board[row][col].food_timer <= 0) { //Should the food timer be equal to 0, the food tile will be changed to water.
                    board[row][col].type = WATER;
                }
            }
            col++;
        }
        row++;
    }
}

void food_superpowers(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2],
    int *lives
) {
    if(board[last_coordinate[X]][last_coordinate[Y]].type == FOOD) {
        int current_Live = *lives;
        *lives = current_Live + 1;
        shout("Congratulations! Frogger ate some food and felt rejuvenated. Total lives: %d -> %d\n", current_Live, *lives);
        board[last_coordinate[X]][last_coordinate[Y]].type = LOG;
    }
}

int is_bug_moveable(
    struct board_tile board[SIZE][SIZE],
    int row,
    int col
) {
    return row > LILLYPAD_ROW && row < BANK_ROW
        && col <= RIGHT_COLUMN && col >= LEFT_COLUMN
        && (board[row][col].type == TURTLE || board[row][col].type == LOG)
        && (board[row][col].bug == NULL);
}

void move_bug_direction(
    struct board_tile board[SIZE][SIZE],
    struct bug_node* bug_linked_list,
    int direction
) {
    struct bug_node* current_bug = bug_linked_list;
    int row = current_bug->row;
    int col = current_bug->col;
    int next_col = col + direction;
    if (is_bug_moveable(board, row, next_col) == FALSE) {
        if (is_bug_moveable(board, row, col - direction) == TRUE) {
            current_bug->moving_right = !current_bug->moving_right;
            move_bug_direction(board, current_bug, -direction);
        }
    } else {
        board[row][col].bug = NULL;
        board[row][next_col].bug = current_bug;
        current_bug->col = next_col;
    }
}

void move_bug(
    struct board_tile board[SIZE][SIZE],
    struct bug_node* bug_linked_list
) {
    struct bug_node* current_bug = bug_linked_list;
    while (current_bug != NULL) {
        move_bug_direction(board, current_bug, current_bug->moving_right ? 1 : -1);
        current_bug = current_bug->next;
    }
}

void occupy(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2]
) {
    board[last_coordinate[X]][last_coordinate[Y]].occupied = TRUE;
}

void unoccupy(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2]
) {
    board[last_coordinate[X]][last_coordinate[Y]].occupied = FALSE;
}

int check_winning_condition(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2],
    int lives,
    enum map_skin selected_skin
) {
    //To simplify current Frogger tile.
    struct board_tile tile = board[last_coordinate[X]][last_coordinate[Y]];

    //If the Frogger lands on water, decrease lives.
    if (tile.type == WATER) {
        lives--;

        if (lives == 0) //Should lives reach 0, player has lost the game (losing condition).
        {
            return 0;
        }
        else //If lives have not reached 0, reset Frogger position and print the game board to start again.
        {
            reset_frogger(board, last_coordinate);
            shout("Frogger has fallen into the water! Remaining lives: %d\n", lives);
            return lives;
        }
    }

    //If Frogger hits a bug, decrease lives.
    if (tile.bug != NULL) { //Conditional check for bugs.
        lives--;

        if (lives == 0) //Should lives reach 0, player has lost the game (losing condition).
        {
            return 0;
        }
        else //If lives have not reached 0, reset Frogger position and print the game board to start again.
        {
            reset_frogger(board, last_coordinate);
            shout("\nOh no! Frogger hit a bug! Remaining lives: %d\n\n", lives);
            return lives;
        }
    }

    //If Frogger lands on a lillypad, player has won the game (winning condition).
    if (tile.type == LILLYPAD) {
        return GAME_WON;
    }

    return lives;
}

void reset_frogger(
    struct board_tile board[SIZE][SIZE],
    int last_coordinate[2]
) {
    board[last_coordinate[X]][last_coordinate[Y]].occupied = FALSE;
    last_coordinate[X] = BANK_ROW;
    last_coordinate[Y] = SIZE / 2;
    board[last_coordinate[X]][last_coordinate[Y]].occupied = TRUE;
}

/**
 * This function acts as a helper to hide the complexity of how we can add a
 * bug into the first item of the linked list.
 *
 * Linked List is a list data structure that allows easier addition/removal of item
 * into the list, since array needs to have a predetermined length, and removal might
 * introduce gap in the list.
 */
struct bug_node* create_bug_node(
    struct bug_node** bug_linked_list,
    int row,
    int col
) {
    struct bug_node* bug = (struct bug_node*) malloc(sizeof(struct bug_node));
    if (bug == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    bug->row = row;
    bug->col = col;
    bug->prev = NULL;
    bug->next = NULL;
    bug->moving_right = TRUE;

    // list is empty, therefore set as HEAD
    if (*bug_linked_list == NULL) {
        *bug_linked_list = bug;
        return bug;
    }

    // insert in the list based on the row/column order
    struct bug_node* current = *bug_linked_list;
    struct bug_node* previous = NULL;
    while (current != NULL && (
        current->row < bug->row                                     // exit if current item has row smaller than inserted bug
        || (current->row == bug->row && current->col < bug->col)    // exit if current item has column smaller than inserted bug
    )) {
        // keep reference to the previous node, since we have doubly linked list
        previous = current;
        // iterate to the next node
        current = current->next;
    }

    // if no previous, then we need to insert at the beginning
    if (previous == NULL) {
        bug->next = *bug_linked_list;
        (*bug_linked_list)->prev = bug;
        *bug_linked_list = bug;
    }
    // else it's inserting in the middle or end
    else {
        bug->next = current;
        bug->prev = previous;
        previous->next = bug;
        // if it's not the end, then we need to re-point the next node to current node
        if (current != NULL) {
            current->prev = bug;
        }
    }

    return bug;
}

/**
 * This function acts as a helper to hide the complexity of how we can remove
 * a certain bug from the linked list.
 */
void remove_bug_node(
    struct bug_node** bug_linked_list,
    struct bug_node* removed
) {
    if (removed == *bug_linked_list) {
        *bug_linked_list = removed->next;
    }
    if (removed->prev != NULL) {
        removed->prev->next = removed->next;
    }
    if (removed->next != NULL) {
        removed->next->prev = removed->prev;
    }
    free(removed);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// PROVIDED FUNCTIONS //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// order by enum {LILLYPAD, BANK, WATER, TURTLE, LOG, FROG, BUG, FOOD};
char tile_chars[] = { 'o', 'x', '~', 'T', 'L', 'F', 'B', 'Y' };
char* tile_colors[] = { GREEN_COLOR, RED_COLOR, BLUE_COLOR, YELLOW_COLOR, BROWN_COLOR, FROG_COLOR, RED_COLOR, YELLOW_COLOR };
wchar_t* tile_emojis[] = { L"🌸", L"🍀", L"💧", L"🐢", L"🪵 ", L"🐸", L"🐞", L"🍔" };

void print_board(struct board_tile board[SIZE][SIZE], enum map_skin selected_skin) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            print_tile(board[row][col], selected_skin);
        }
        printf("\n");
    }
}

void print_tile(struct board_tile tile, enum map_skin selected_skin) {
    enum tile_type type = tile.type;
    if (tile.occupied) {
        type = FROG;
    } else if (tile.bug != NULL) {
        type = BUG;
    }
    switch (selected_skin) {
        case EMOJI:
        case COLOR_EMOJI:
            if (type == WATER && selected_skin == COLOR_EMOJI) {
                printf("%s%c%c%s ", tile_colors[type], tile_chars[type], tile_chars[type], RESET_COLOR);
            } else {
                wprintf(L"%ls ", tile_emojis[type]);
            }
            break;
        case COLOR:
            printf("%s%c%s ", tile_colors[type], tile_chars[type], RESET_COLOR);
            break;
        default:
            printf("%c ", tile_chars[type]);
            break;
    }
}

void show_help() {
    printf("|--------------------------------------------------------------------|\n");
    printf("|--------------- Enter 'h' command to show this help ----------------|\n");
    printf("|--------------------------------------------------------------------|\n");
    printf("| q = quit     |  l = add log    |  c = clear row  |  r = remove log |\n");
    printf("| b = add bug  |  m = map theme  |                 |                 |\n");
    printf("| w = move up  |  a = move left  |  s = move down  |  d = move right |\n");
    printf("|--------------------------------------------------------------------|\n");
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void shout(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(GAME_MESSAGE, sizeof(GAME_MESSAGE), format, args);
    va_end(args);
}

void print_message() {
    if (GAME_MESSAGE[0] != '\0') {
        printf("\n%s\n", GAME_MESSAGE);
        GAME_MESSAGE[0] = '\0'; // Flush the buffer by setting it to an empty string
    } else {
        printf("\n\n\n");
    }
}
