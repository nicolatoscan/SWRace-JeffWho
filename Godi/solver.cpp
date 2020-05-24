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

#define PATH_CURVE_1 16  //   |_
#define PATH_CURVE_2 32  // _|
#define PATH_CURVE_3 64  // ¯|
#define PATH_CURVE_4 128 //   |¯

#define MOV_UP 256
#define MOV_LEFT 512
#define MOV_RIGHT 1024
#define MOV_DOWN 2048
#define MOVS 3840
#define MOVV 2304
#define MOVH 1536


#define BORDER 4096

/* -----------------------------------------¯- */

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
int temp, i, j, r, c;

// Data
int R, C, B, W, A;
int R1, C1, R2, C2;

// Matrice base con anelli
int **mat;

/* ------------------------------------------ */

int input = 6
;

string sfile = "input" + to_string(input) + ".txt";
ifstream in("../input/" + sfile);
// ofstream out("output.txt");

/* ------------------------------------------ */

void load(void);
void get_around(void);

/* ------------------------------------------ */

int keep_ring(int n)
{
    return n & (RING_BLACK | RING_WHITE);
}
int keep_path(int n)
{
    return n & 252;
}
int keep_mov(int n)
{
    return n & 3840;
}
int mov_inv(int n)
{
    switch (n)
    {
        case MOV_UP: return MOV_DOWN;
        case MOV_RIGHT: return MOV_LEFT;
        case MOV_DOWN: return MOV_UP;
        case MOV_LEFT: return MOV_RIGHT;
    }
}
pair<int, int> mov_apply(int r, int c, int mov)
{
    if(mov == MOV_UP) return make_pair(r - 1, c);
    else if(mov == MOV_DOWN) return make_pair(r + 1, c);
    else if(mov == MOV_LEFT) return make_pair(r, c - 1);
    else return make_pair(r, c + 1);
}
pair<int, int> mov_apply(pair<int,int> coord, int mov)
{
    return mov_apply(coord.first, coord.second, mov);
}

int prosegui(pair<int, int> coord, int last_move, bool ultimo_dritto, bool deve_dritto, bool deve_curvare, int depth)
{
    int r = coord.first, c = coord.second;

    // Tolgo mossa inversa dalle possibili
    int movs = MOVS & ~ mov_inv(last_move);

    // Tolgo mosse che vanno fuori dalle celle possibili (Bordi e celle dove sono già stato)
    if(mat[r-1][c] & BORDER) movs &= ~MOV_UP;
    if(mat[r+1][c] & BORDER) movs &= ~MOV_DOWN;
    if(mat[r][c-1] & BORDER) movs &= ~MOV_LEFT;
    if(mat[r][c+1] & BORDER) movs &= ~MOV_RIGHT;
    
    // Rispetto regole
    if(last_move & (MOV_UP | MOV_DOWN))
    {
        // Arrivo da mossa verticale e devo andare verticale
        if(deve_dritto || mat[r][c] & RING_WHITE) movs &= ~(MOV_LEFT | MOV_RIGHT);

        // Arrivo da mossa verticale e devo andare orizzontale
        if(deve_curvare || mat[r][c] & RING_BLACK) movs &= ~(MOV_UP | MOV_DOWN);
    }
    else
    {
        // Arrivo da mossa orizzontale e devo andare orizzontale
        if(deve_dritto || mat[r][c] & RING_WHITE) movs &= ~(MOV_UP | MOV_DOWN);

        // Arrivo da mossa orizzontale e devo andare verticale
        if(deve_curvare || mat[r][c] & RING_BLACK) movs &= ~(MOV_LEFT | MOV_RIGHT);
    }
    
    // Impongo regole se sono su un ring
    bool next_deve_dritto = mat[r][c] & RING_BLACK;
    bool next_deve_curvare = mat[r][c] & RING_WHITE;

    // Ramo morto
    int ways = 0;

    // Faccio mosse possibili
    for(i = MOV_UP; i <= MOV_DOWN; i *= 2)
    {
        // Se mossa è consentita
        if(i & movs)
        {
            // Ho trovato un modo valido per proseguire
            ways++;

            int boh = prosegui(mov_apply(r, c, i), i, i &)
        }
    }
}

/* ---------------------------------------------------------------------------- */

int main(void)
{
    cout << "FILE: \n# " + sfile << endl << endl;

    load();
    
    return 0;
}

/* ---------------------------------------------------------------------------- */

void get_around(void)
{
    // Parto da angolo top-left
    mat[1][1] += PATH_CURVE_4;

    // Vado giù
    // down_left();
}

/* ------------------------------------------ */

void load(void)
{
    in >> R >> C >> B >> W;
    A = B + W;

    R1 = R + 1;
    C1 = C + 1;
    R2 = R + 2;
    C2 = C + 2;

    mat = new int *[R2];

    // Inizializza
    for (r = 0; r < R2; r++)
    {
        mat[r] = new int[C2];
        for (c = 0; c < C2; c++)
        {
            mat[r][c] = (r == 0 || c == 0 || r == R1 || c == C1) ? BORDER : EMPTY;
        }
    }

    // Carica anelli da file
    for (int i = 0; i < B; i++) // Black
    {        
        in >> r >> c;

        mat[r+1][c+1] = RING_BLACK;
    }
    for (int i = 0; i < W; i++) // White
    {        
        in >> r >> c;

        mat[r+1][c+1] = RING_WHITE;
    }
}

/* ------------------------------------------ */

void show_path()
{
    cout << "\033[32m   ";
    for (int c = 0; c < C; c++)
        if (c < 10)
            cout << "  " << c;
        else
            cout << " " << c;
    cout << "\033[0m" << endl;

    for (int r = 0; r < R; r++)
    {
        if (r < 10)
            cout << "\033[32m  " << r << " \033[0m";
        else
            cout << "\033[32m " << r << " \033[0m";

        for (int c = 0; c < C; c++)
        {
            int color = 31;
            int n = redSol[r][c];
            string ccc = "0";
            if (n == MOV_UP)
                ccc = "↑";
            else if (n == MOV_DOWN)
                ccc = "↓";
            else if (n == MOV_LEFT)
                ccc = "←";
            else if (n == MOV_RIGHT)
                ccc = "→";
            else
            {
                ccc = "0";
                color = 37;
            }

            if (mat[r][c] & RING_BLACK)
                color = 32;
            if (mat[r][c] & RING_WHITE)
                color = 34;

            cout << "\033[" << color << "m " << ccc << " \033[0m";
        }
        cout << endl;
    }
    cout << endl;
}