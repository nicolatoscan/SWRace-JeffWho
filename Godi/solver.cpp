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

typedef struct Angoli
{
    int first_row;
    int first_col;
    int last_row;
    int last_col;

    Angoli()
    {
        first_row = first_col = last_row = last_col = -1;
    }
    Angoli(int fr, int fc, int lr, int lc)
    {
        first_row = fr;
        first_col = fc;
        last_row = lr;
        last_col = lc;
    }
}Angoli;

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

int input = 6
;

string sfile = "input" + to_string(input) + ".txt";
ifstream in("../input/" + sfile);
// ofstream out("output.txt");

/* ------------------------------------------ */

void load(void);
void print_stats(void);
Angoli* get_quadrato(void);

void find_path(void);

void print_path_quadrato(Angoli a);

/* ---------------------------------------------------------------------------- */

int main(void)
{
    cout << "FILE: \n# " + sfile << endl << endl;

    load();
    // print_stats();

    Angoli* ang = get_quadrato();
    if(ang != NULL)
    {
        print_path_quadrato(*ang);
    }


    return 0;
}

/* ---------------------------------------------------------------------------- */

void find_path(void)
{

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

Angoli* get_quadrato(void)
{
    if(B*2 != W) return NULL;    
    Angoli* ang = new Angoli();

    // Righe
    r = -1;
    
    while(howmany_in_row[++r] == 0);

    if(howmany_in_row[r] != 4) return NULL;
    ang->first_row = r;
    if(howmany_in_row[++r] != 2) return NULL;

    while(howmany_in_row[++r] == 0);

    if(howmany_in_row[r] != 2) return NULL;
    if(howmany_in_row[++r] != 4) return NULL;
    ang->last_row = r;

    while(r + 1 < R && howmany_in_row[++r] == 0);
    if(r + 1 != R) return NULL;


    // Colonne
    c = -1;
    
    while(howmany_in_col[++c] == 0);

    if(howmany_in_col[c] != 4) return NULL;
    ang->first_col = c;
    if(howmany_in_col[++c] != 2) return NULL;

    while(howmany_in_col[++c] == 0);

    if(howmany_in_col[c] != 2) return NULL;
    if(howmany_in_col[++c] != 4) return NULL;
    ang->last_col = c;

    while(c + 1 < C && howmany_in_col[++c] == 0);
    if(c + 1 != C) return NULL;

    return ang;
}

/* ------------------------------------------ */

void print_path_quadrato(Angoli a)
{
    cout << "SOLUTION:\n# Type: Quadrato\n# Starting Point: (R,C) (" << a.first_row << "," << a.first_col << ")\n# Path: ";
    
    for(c = a.first_col; c < a.last_col; c++)
    {
        cout << "R";
    }
    for(r = a.first_row; r < a.last_row; r++)
    {
        cout << "D";
    }
    for(c = a.first_col; c < a.last_col; c++)
    {
        cout << "L";
    }
    for(r = a.first_row; r < a.last_row; r++)
    {
        cout << "U";
    }
    cout << endl << endl;
}