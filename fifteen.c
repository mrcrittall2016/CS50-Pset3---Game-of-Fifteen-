/**
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
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
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
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
bool move(int tile);
bool won(void);
bool nexttospace(int array[DIM_MAX][DIM_MAX], int i, int j); 
bool find(int array[DIM_MAX][DIM_MAX], int* a, int* b, int tile);
bool swapped(int array[DIM_MAX][DIM_MAX], int* a, int* b, int* i, int* j);

int main(int argc, string argv[])
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
    while (true)
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
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = GetInt();
        
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
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
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
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(void)
{
    // Initialise the board
    int piece = d*d; 
    int a;
    int b;
    
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            // if get to bottom right corner of board, ensure special character. Note, had to change to 0 for check50 purposes
            if (i == d - 1 && j == d - 1)
            {
                board[i][j] = 0; 
                break; 
            }
            piece--; 
            board[i][j] = piece;
        }
            
    }
    
    // If odd number of tiles then swap 2 and 1
    
    piece = d*d;
    
    if ((piece - 1) % 2 != 0)
    {
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                if (board[i][j] == 1)
                {
                    find(board, &a, &b, 2);
                    swapped(board, &a, &b, &i, &j); 
                }
            }
        }
    }
    
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            if (board[i][j] >= 10)
            {
                printf("%d ", board[i][j]);
            }
            
            if (board[i][j] < 10)
            {
                printf(" %d ", board[i][j]);
            }
            
            if (j == d - 1)
            {
                printf("\n"); 
            }
            
        }
            
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{
    int a;
    int b;
    int i;
    int j; 
    int swap = 0; 
    
    
    // find tile on board and determine whether next to space
    for (i = 0; i < d; i++)
    {
        for (j = 0; j < d; j++)
        {
            if (tile == board[i][j])
            {
               if (!nexttospace(board, i, j))
               {
                    printf("returned false in nexttospace loop\n");
                    return false;  
               }    
               
               // else find space and swap
               else
               {
                    
                    // find space function 
                    if(find(board, &a, &b, 0))
                    {
                        // swap function
                        if(swapped(board, &a, &b, &i, &j))
                        {
                            swap++; 
                        }
                    }
                    
                    else 
                    {
                        printf("returned false in else loop\n"); 
                        return false;
                    }
                    
               }
               
               
            }
            
            if (swap != 0)
            {
                swap = 0; 
                return true;  
            }
        }
    }
    
    printf("returned false at very end\n"); 
    return false; 
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    int count = 0; 
    int n = 0;
    
    // define one-dimensional array 
    int array[d*d];
    
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            *(array + n) = board[i][j]; 
            n++; 
        }       
        
    }
    
    *(array + n) = '\0'; 
    
    for (int i = 0; i < d*d; i++)
    {
        if(array[i + 1] - array[i] == 1)
        {
            count++; 
        }
    }
    
    if (count == d*d - 2)
    {
        printf("count is: %d\n", count); 
        return true; 
    }
    
    printf("count is: %d\n", count); 
    
    return false; 
}

// function to determine whether tile is next to space or not. Returns true if so 
bool nexttospace(int array[DIM_MAX][DIM_MAX], int i, int j)
{
    printf("i and j for tile in nexttospace function are: %d and %d\n", i, j); // as soon as tile as coordinates 1 (ie LHS of board) then becomes 0
    
    // if tile borders left edge of the board
    if (j == 0)
    {
        // top left corner
        if (i == 0)
        {
            if (array[i + 1][j] == 0 || array[i][j + 1] == 0) 
            {
                return true; 
            }
        }
        
        // bottom left corner
        else if (i == d - 1)
        {
            if (array[i - 1][j] == 0 || array[i][j + 1] == 0)
            {
                return true; 
            }
            
        }
        
        // in between 
        else 
        {
            if (array[i + 1][j] == 0 || array[i - 1][j] == 0 || array[i][j + 1] == 0)
            {
                return true;
            }
        }
    }
    
    // if tile borders right edge of the board
    else if (j == d - 1)
    {
        // top right corner
        if (i == 0)
        {
            if (array[i + 1][j] == 0 || array[i][j - 1] == 0) 
            {
                return true; 
            }
        }
        
        // bottom right corner
        else if (i == d - 1)
        {
            if (array[i - 1][j] == 0 || array[i][j - 1] == 0)
            {
                return true; 
            }
            
        }
        
        // in between 
        else 
        {
            if (array[i + 1][j] == 0 || array[i - 1][j] == 0 || array[i][j - 1] == 0)
            {
                return true;
            }
        }
    }
    
    // every other tile 
    else 
    {
        if (array[i + 1][j] == 0 || array[i - 1][j] == 0 || array[i][j + 1] == 0 || array[i][j - 1] == 0) 
        {
            return true; 
        }
    }
 
    
    return false; 
    
}

// function to return coordinates of tile or space. Note, use pointers as way of returning more than one value from a C function 
bool find(int array[DIM_MAX][DIM_MAX], int* a, int* b, int tile)
{
    int x;
    int y;
    
    for (x = 0; x < d; x++)
    {
        for (y = 0; y < d; y++)
        {
            if (board[x][y] == tile)
            {
                *a = x;
                *b = y;
                return true; 
            }
        }
    }
    
    return false; 
    
}

// Swap function. Again, must pass by reference and use dereference notation to ensure values are stored in memory 
bool swapped(int array[DIM_MAX][DIM_MAX], int* a, int* b, int* i, int* j)
{
    
    int temp = board[*a][*b]; 
    board[*a][*b] = board[*i][*j];  
    board[*i][*j] = temp;
    return true; 
}



