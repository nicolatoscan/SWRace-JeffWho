#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

/* ------------------------------------------ */

#define EMPTY 0
#define RING_WHITE 1
#define RING_BLACK 2

#define PATH_VER 4
#define PATH_HOR 8

#define CURVE_1 16
#define CURVE_2 32
#define CURVE_3 64
#define CURVE_4 128

#define UP 256
#define DOWN 512
#define LEFT 1024
#define RIGHT 2048

/* ------------------------------------------ */

ifstream in("../input/input1.txt");
// ofstream out("output.txt");

// Temp
int i, j, r, c;

// Data
int R, C, B, W, A;

// Matrice base con anelli
int **mat;

// Quanti anelli per ogni riga/colonna
int *howmany_in_row;
int *howmany_in_col;

// Quante righe e colonne sono libere
int free_rows_count;
int free_cols_count;

/* ------------------------------------------ */

void load(void);

/* ------------------------------------------ */

int main(void)
{
    load();

    return 0;
}

/* ------------------------------------------ */

void load(void)
{
    in >> R >> C >> B >> W;
    A = B + W;

    mat = new int *[R];
    howmany_in_row = new int[R];
    howmany_in_col = new int[C];

    // Inizializza
    for (r = 0; r < R; r++)
    {
        mat[r] = new int[C];
        for (c = 0; c < C; c++)
        {
            mat[r][c] = EMPTY;
        }
        howmany_in_row[r] = 0;
    }
    for (c = 0; c < C; c++)
    {
        howmany_in_col[c] = 0;
    }

    // Carica anelli da file
    for (int i = 0; i < B; i++) // Black
    {        
        in >> r >> c;

        mat[r][c] = RING_BLACK;

        howmany_in_row[r]++;
        howmany_in_col[c]++;
    }
    for (int i = 0; i < W; i++) // White
    {        
        in >> r >> c;

        mat[r][c] = RING_WHITE;
        howmany_in_row[r]++;
        howmany_in_col[c]++;
    }
}
