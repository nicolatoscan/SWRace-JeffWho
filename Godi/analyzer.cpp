#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <stack>
using namespace std;

/* ------------------------------------------ */

#define EMPTY 0
#define RING_WHITE 1
#define RING_BLACK 2

#define RIGHE 0
#define COLONNE 1

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

// Robe strane
int whites_alone;
int good_corners;
stack <pair<int,int>> blacks;

/* ------------------------------------------ */

void load(void);
void print_stats(int fl);

bool quadrato(void);
bool black_nearby(int r, int c);
int find_corners(void);

/* ---------------------------------------------------------------------------- */

int main(void)
{
    std::cout << std::setprecision(4) << std::fixed;

    for(int fl = 1; fl <= 19; fl++)
    {
        print_stats(fl);
    }

    return 0;
}

/* ---------------------------------------------------------------------------- */

void load(int fl)
{
    ifstream in("../input/input" + to_string(fl) + ".txt");

    in >> R >> C >> B >> W;
    A = B + W;

    mat = new int *[R];
    howmany_in_row = new int[R];
    howmany_in_col = new int[C];

    whites_alone = 0;

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

        blacks.push(make_pair(r, c));
    }
    for (int i = 0; i < W; i++) // White
    {        
        in >> r >> c;

        mat[r][c] = RING_WHITE;
        howmany_in_row[r]++;
        howmany_in_col[c]++;

        if(!black_nearby(r, c)) whites_alone++;
    }
}

void print_stats(int fl)
{
    load(fl);
    find_corners();

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

    cout << "STATS input" + to_string(fl) + ".txt:" << endl;
    cout << "# Tipo: " << (quadrato() ? "Quadrato" : "Altro") << endl;
    cout << "# Tot anelli: " << A << "(" << B << "B + " << W << "W)" << endl;
    cout << "# Tot celle: " << tot << endl;
    cout << "# Anelli/Celle: " << ((float)A)/tot*100 << "%" << endl;
    cout << "# Free_rows: " << free_rows_count << " su " << R << endl;
    cout << "# Free_cols: " << free_cols_count <<  " su " << C << endl;
    cout << "# Zone Occupate/Totale = " << ((float)tot - freecount)/tot*100 << "%" << endl;
    cout << "# B con due W ad angolo: [" << good_corners << " su " << B << "] e restano " << W-good_corners*2 << " W" << endl;
    cout << "# W totalmente isolati: " << whites_alone << endl;
    cout << endl;
}

bool quadrato(void)
{
    if(B*2 != W) return false;

    // Righe
    r = -1;
    
    while(howmany_in_row[++r] == 0);

    if(howmany_in_row[r] != 4) return false;
    if(howmany_in_row[++r] != 2) return false;

    while(howmany_in_row[++r] == 0);

    if(howmany_in_row[r] != 2) return false;
    if(howmany_in_row[++r] != 4) return false;

    while(r + 1 < R && howmany_in_row[++r] == 0);
    if(r + 1 != R) return false;


    // Colonne
    c = -1;
    
    while(howmany_in_col[++c] == 0);

    if(howmany_in_col[c] != 4) return false;
    if(howmany_in_col[++c] != 2) return false;

    while(howmany_in_col[++c] == 0);

    if(howmany_in_col[c] != 2) return false;
    if(howmany_in_col[++c] != 4) return false;

    while(c + 1 < C && howmany_in_col[++c] == 0);
    if(c + 1 != C) return false;

    return true;
}

bool black_nearby(int r, int c)
{
    if(r+1 < R) if(mat[r+1][c] == RING_BLACK) return true;
    if(r-1 >= 0) if(mat[r-1][c] == RING_BLACK) return true;
    if(c+1 < C) if(mat[r][c+1] == RING_BLACK) return true;
    if(c-1 >= 0) if(mat[r][c-1] == RING_BLACK) return true;
    return false;
}

int find_corners(void)
{
    int angoli = 0;
    int result;

    while(!blacks.empty())
    {
        result = 1;

        r = blacks.top().first;
        c = blacks.top().second;
        blacks.pop();

        // (14) 2 (6)
        //    7 # 3
        // (35) 5 (15)

        if(r+1 < R && mat[r+1][c] == RING_WHITE) result *= 5;
        if(r-1 >= 0 && mat[r-1][c] == RING_WHITE) result *= 2;
        if(c+1 < C && mat[r][c+1] == RING_WHITE) result *= 3;
        if(c-1 >= 0 && mat[r][c-1] == RING_WHITE) result *= 7;

        if(result == 6 || result == 15 || result == 35 || result == 14) angoli++;
    }

    good_corners = angoli;
    return angoli;
}