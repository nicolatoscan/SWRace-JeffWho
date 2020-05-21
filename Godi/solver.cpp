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

// Temp
int i, j, r, c;

// Data
int R, C, B, W, A;

// Matrice base con anelli
int **mat;

// Quanti anelli per ogni riga/colonna
int *howmany_in_row;
int *howmany_in_col;

/* ------------------------------------------ */

int input = 3
;

string sfile = "input" + to_string(input) + ".txt";
ifstream in("../input/" + sfile);
// ofstream out("output.txt");

/* ------------------------------------------ */

void load(void);
void print_stats(void);

/* ------------------------------------------ */

int main(void)
{
    cout << "FILE: \n# " + sfile << endl << endl;

    load();
    print_stats();

    return 0;
}

/* ------------------------------------------ */



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

void print_stats(void)
{
    int free_rows_count = 0;
    int free_cols_count = 0;

    for(int r = 0; r < R; r++)
    {
        if(howmany_in_row[r] == 0) free_rows_count++;
    }
    for(int c = 0; c < C; c++)
    {
        if(howmany_in_col[c] == 0) free_cols_count++;
    }

    int tot = R * C;
    int freecount = free_rows_count*C + free_cols_count*R - free_rows_count*free_cols_count;
    float rap = tot - freecount;

    cout << "STATS:" << endl;
    cout << "# Tot celle: " << tot << endl;
    cout << "# Free_rows: " << free_rows_count << " su " << R << endl;
    cout << "# Free_cols: " << free_cols_count <<  " su " << C << endl;
    cout << "# Zone Occupate/Totale = " << rap/tot*100 << "%" << endl << endl;
}