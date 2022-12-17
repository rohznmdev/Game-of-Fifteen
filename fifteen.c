/**************************************************************************
 * This work originally copyright David J. Malan of Harvard University, or-
 * iginally released under:
 * CC BY-NC-SA 3.0  http://creativecommons.org/licenses/by-nc-sa/3.0/
 * licensing.
 *
 * It has been adapted for use in csci 1730.  Per the share-alike
 * clause of this license, this document is also released under the
 * same license.
 **************************************************************************/
/**
 * fifteen.c
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
int move(int tile);
int won(void);

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE* file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (1)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("win!\n");
            break;
        }

        // prompt for move
        printf("Tile to move (0 to exit): ");
        int tile;
		scanf("%d", &tile);
		getchar();

        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(50000);
        }

        // sleep thread for animation's sake
        usleep(50000);
    }

    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(200000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).
 */
void init(void)
{
    // Calculates the number of needed spots.
    int maxNum = d * d - 1;
    // Loops through the grid

    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            //sets the board at a certain slot to the total value
            board[i][j] = maxNum;
            maxNum--;
        } // for
    } // for

    // if the board is of even dimensions, swap the position of 1 and 2.
    if ((d * d) % 2 == 0) {
        board[d - 1][d - 3] = 1;
        board[d - 1][d - 2] = 2;
    } // if
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    // loop through the board.
    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            // if the init function sets a spot to 0, it will print _ instead of a number.
            if (board[i][j] == 0) {
                printf("  _");
            } else {
                // else just print the value of the board set by init.
                printf("%3i", board[i][j]);
            } // if-else
        } // for
        //prevents it from clumping in one line.
        printf("\n");
    } // for

}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false.
 */
int move(int tile)
{
    // checks if the tile is a valid input
    int grid = d*d;
    if (tile > grid - 1 || tile < 1) {
        // returns 0 for false
        return 0;
    } // if
    // this finds the tile the empty slot is on.
    int r = 0;
    int c = 0;
    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            if (board[i][j] == tile) {
                // r = row
                // c = col
                r = i;
                c = j;
            } // if
        } // for
    } // for
    // checking if the slot is eligible to be moved.
    // if it is swap it and return 1.
    if (board[r][c + 1] == 0 && c + 1 < d) {
        board[r][c + 1] = board[r][c];
        board[r][c] = 0;
        return 1;
    } else if (board[r][c - 1] == 0 && c - 1 >= 0) {
        board[r][c - 1] = board[r][c];
        board[r][c] = 0;
        return 1;
    } else if (board[r + 1][c] == 0 && r + 1 < d) {
        board[r + 1][c] = board[r][c];
        board[r][c] = 0;
        return 1;
    } else if (board[r - 1][c] == 0 && r - 1 >= 0) {
        board[r - 1][c] = board[r][c];
        board[r][c] = 0;
        return 1;
    }
    return 0;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration),
 * else false.
 */
int won(void)
{
    int checkCnt = 1;
    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            //loops through to see if all the values are in the right order.
            if (board[i][j] != 0 && board[i][j] != checkCnt) {
                return 0;
            }
            checkCnt++;
        } // for
    } // for
    return 1;
}
