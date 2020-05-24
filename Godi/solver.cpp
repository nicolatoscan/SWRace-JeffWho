#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <queue>
#include <set>
using namespace std;

/* ------------------------------------------ */

#define EMPTY 0
#define RING_WHITE 1
#define RING_BLACK 2
#define RINGS 3

#define PATH_VER 4
#define PATH_HOR 8

#define PATH_CURVE_1 16  //
#define PATH_CURVE_2 32  //
#define PATH_CURVE_3 64  //
#define PATH_CURVE_4 128 //

#define MOV_UP 256
#define MOV_LEFT 512
#define MOV_RIGHT 1024
#define MOV_DOWN 2048
#define MOVV 2304
#define MOVH 1536
#define MOVS 3840

#define BORDER 4096


#define MOSSA_INVALIDA -1
#define MAX_DEPTH 10

/* ------------------------------------------- */

typedef int cella;
typedef int mossa;
typedef int mosse;
typedef pair<int, int> coordinate;

/* ------------------------------------------ */

// Data
int R, C, B, W, A;
int R1, C1, R2, C2;

// Matrice base con anelli
int** mat;

// Movimenti
int** mat_movs;

// Dove sono partito e arrivato
pair<int, int> started;
pair<int, int> arrived;

// Celle usate in brute force
set<pair<int, int>> blocked;

/* ------------------------------------------ */

int input = 17
;
string sfile = "input" + to_string(input) + ".txt";
ifstream in("C:/Users/loren/OneDrive/Desktop/dio_oni/Debug/input/" + sfile);
//ifstream in("input7.txt");

// ofstream out("output.txt");

/* ------------------------------------------ */

void load(void);
void mat_movs_clear(void);

mosse filtra_mosse_possibili(coordinate current_pos, mossa last_mov, bool deve_dritto, bool deve_curvare);
mossa trova_prossima_mossa(coordinate current_pos, mossa last_mov, bool precedente_dritto, bool deve_dritto, bool deve_curvare, mossa direction_focus);
int valuta_mossa(coordinate current_pos, mossa last_mov, bool precedente_dritto, bool deve_dritto, bool deve_curvare, mossa direction_focus, int depth);

int keep_ring(int n);
int keep_path(int n);
int keep_mov(int n);

char mov_to_char(mossa m);

mossa mov_inv(mossa mov);
coordinate mov_apply(int r, int c, mossa mov);
coordinate mov_apply(coordinate coord, mossa mov);
bool same_orientation(mossa mov1, mossa mov2);

void show_path(void);

/* ---------------------------------------------------------------------------- */

bool nx_dr = false, nx_cv = false;
int main(void)
{
    cout << "FILE: \n# " + sfile << endl << endl;

    load();

    mat_movs_clear();

    coordinate caz = make_pair(5, 3);
    mossa j = MOV_LEFT, prev = MOV_LEFT;
    bool mem = true;
    for (int i = 0; i < 20; i++)
    {
        prev = j;
        j = trova_prossima_mossa(caz, j, mem, nx_dr, nx_cv, MOV_DOWN);
        mem = prev == j;

        mat_movs[caz.first][caz.second] = j;

        cout << mov_to_char(j) << endl;

        caz = mov_apply(caz, j);
    }


    return 0;
}

/* ---------------------------------------------------------------------------- */

mossa trova_prossima_mossa(coordinate current_pos, mossa last_mov, bool precedente_dritto, bool deve_dritto, bool deve_curvare, mossa direction_focus)
{
    // r, c
    int r = current_pos.first, c = current_pos.second;

    // mat[r][c]
    cella cel = mat[r][c];

    // Filtro mosse
    blocked.clear(); // PER SICUREZZA, NON DOVREBBE SERVIRE
    mosse movs = filtra_mosse_possibili(current_pos, last_mov, deve_dritto, deve_curvare);
    /*if (movs & (~MOVS))
        cout << "ERRORE ";
    cout << movs << endl;*/
    // Se sono su anello nero le due prossime mosse dovranno avere stessa direzione
    bool next_deve_dritto = cel & RING_BLACK;
    // Se sono su anello bianco e prima non ho curvato, adesso continuerò dritto ma dopo dovrò curvare
    bool next_deve_curvare = cel & RING_WHITE && precedente_dritto;

    // Deve esserci almeno una mossa possibile a questo punto
    if (!(movs & MOVS))
    {
        // Sono bloccato!
        return MOSSA_INVALIDA;
    }

    // Provo le mosse possibili
    int ways = 0;
    pair<int, int>* results = new pair<int, int>[3];
    for (int i = MOV_UP; i <= MOV_DOWN; i *= 2)
    {
        // Se mossa è consentita
        if (i & movs)
        {
            // Ho trovato un modo valido per proseguire
            blocked.clear();
            int points = valuta_mossa(mov_apply(r, c, i), i, i == last_mov, next_deve_dritto, next_deve_curvare, direction_focus, MAX_DEPTH);
            if (points > -1) results[ways++] = make_pair(points, i);
        }
    }

    // Ordino risultati per punteggio
    sort(results, results + ways);

    // Prendo la mossa migliore
    mossa def_mov = results[ways - 1].second;

    // CAOS
    nx_dr = next_deve_dritto;
    nx_cv = next_deve_curvare;

    return def_mov;

    // Salva la mossa definitiva (dice alla cella dove sono dove andare)
    // mat_movs[r][c] = def_mov;
}

int valuta_mossa(coordinate current_pos, mossa last_mov, bool precedente_dritto, bool deve_dritto, bool deve_curvare, mossa direction_focus, int depth)
{
    // Mi trovo in una cella in seguito a una mossa fatta non definitiva
    // Devo ritornare il punteggio di questa mossa valutando anche le mosse successive possibili

    // r, c
    int r = current_pos.first, c = current_pos.second;

    // mat[r][c]
    cella cel = mat[r][c];

    // La mossa che ho fatto per andare nella cella in cui sono ora è valida per la cella precedente
    // deve_dritto (deve_curvare) mi obbliga a curvare se nella cella precedente era presente un anello nero (bianco)

    // Posso arrivare in questa cella se qui c'è un anello nero?
    if (cel & RING_BLACK && !precedente_dritto)
    {
        // Sono su un anello nero e prima non sono andato dritto, non valido!
        return MOSSA_INVALIDA;
    }

    // A questo punto la mossa che mi porta nella cella in cui sono è valida e fattibile
    // Devo vedere se il percorso può continuare oltre e quanto mi conviene

    // Punto se sono su un ring
    int point = cel & RINGS ? 1 : 0;

    // Filtro mosse
    mosse movs = filtra_mosse_possibili(current_pos, last_mov, deve_dritto, deve_curvare);
    /*cout << "# ";
    if (movs & (~MOVS))
        cout << "ERROREEEEEE ";
    cout << movs << endl;*/
    // Se sono su anello nero le due prossime mosse dovranno avere stessa direzione
    bool next_deve_dritto = cel & RING_BLACK;
    // Se sono su anello bianco e prima non ho curvato, adesso continuerò dritto ma dopo dovrò curvare
    bool next_deve_curvare = cel & RING_WHITE && precedente_dritto;

    // Sono bloccato?
    if (!(movs & MOVS))
    {
        return -1;
    }

    // Sono in fondo alla ricorsione?
    if (depth == 0)
    {
        return point;
    }

    // Non sono in fondo alla ricorsione
    // Provo le mosse possibili (1<= ways <= 3)
    int ways = 0;
    pair<int, int>* results = new pair<int, int>[3];

    for (int i = MOV_UP; i <= MOV_DOWN; i *= 2)
    {
        // Se mossa è consentita
        if (i & movs)
        {
            // Rendo la cella in cui sono provvisioriamente occupata
            blocked.insert(current_pos);

            // Ho trovato un modo valido per proseguire
            int points = valuta_mossa(mov_apply(r, c, i), i, i == last_mov, next_deve_dritto, next_deve_curvare, direction_focus, depth - 1);
            if (points > -1) results[ways++] = make_pair(points, i);

            // Rendo la cella in cui sono nuovamente libera
            blocked.erase(current_pos);
        }
    }

    if (ways < 1) return -1;

    // Ordino risultati per punteggio
    sort(results, results + ways);

    // Prendo la mossa migliore
    mossa def_mov = results[ways - 1].second;

    // Ritorno il punteggio massimo possibile trovato
    return point + results[ways - 1].first;
}

mosse filtra_mosse_possibili(coordinate current_pos, mossa last_mov, bool deve_dritto, bool deve_curvare)
{
    // r, c
    int r = current_pos.first, c = current_pos.second;

    // mat[r][c]
    cella cel = mat[r][c];

    // Tolgo mossa inversa dalle possibili
    mosse movs = MOVS & ~mov_inv(last_mov);

    // Tolgo mosse che vanno fuori dalle celle possibili (Bordi e celle dove sono già stato)
    if (mat[r - 1][c] & BORDER || mat_movs[r - 1][c] & MOVS || blocked.count(make_pair(r - 1, c)) > 0) movs &= ~MOV_UP;
    if (mat[r + 1][c] & BORDER || mat_movs[r + 1][c] & MOVS || blocked.count(make_pair(r + 1, c)) > 0) movs &= ~MOV_DOWN;
    if (mat[r][c - 1] & BORDER || mat_movs[r][c - 1] & MOVS || blocked.count(make_pair(r, c - 1)) > 0) movs &= ~MOV_LEFT;
    if (mat[r][c + 1] & BORDER || mat_movs[r][c + 1] & MOVS || blocked.count(make_pair(r, c + 1)) > 0) movs &= ~MOV_RIGHT;

    // Rispetto regole
    if (last_mov & MOVV)
    {
        // Arrivo da mossa verticale e devo andare verticale
        if (deve_dritto || cel & RING_WHITE) movs &= ~MOVH;

        // Arrivo da mossa verticale e devo andare orizzontale
        if (deve_curvare || cel & RING_BLACK) movs &= ~MOVV;
    }
    else
    {
        // Arrivo da mossa orizzontale e devo andare orizzontale
        if (deve_dritto || cel & RING_WHITE) movs &= ~MOVV;

        // Arrivo da mossa orizzontale e devo andare verticale
        if (deve_curvare || cel & RING_BLACK) movs &= ~MOVH;
    }

    return movs;
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

    mat = new int* [R2];
    mat_movs = new int* [R2];

    int r, c;

    // Inizializza
    for (r = 0; r < R2; r++)
    {
        mat[r] = new int[C2];
        mat_movs[r] = new int[C2];
        for (c = 0; c < C2; c++)
        {
            mat[r][c] = (r == 0 || c == 0 || r == R1 || c == C1) ? BORDER : EMPTY;
        }
    }

    // Carica anelli da file
    for (int i = 0; i < B; i++) // Black
    {
        in >> r >> c;

        mat[r + 1][c + 1] = RING_BLACK;
    }
    for (int i = 0; i < W; i++) // White
    {
        in >> r >> c;

        mat[r + 1][c + 1] = RING_WHITE;
    }
}

void mat_movs_clear(void)
{
    for (int r = 0; r < R2; r++)
    {
        for (int c = 0; c < C2; c++)
        {
            mat_movs[r][c] = 0;
        }
    }
}

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

char mov_to_char(mossa m)
{
    char c = '?';
    switch (m)
    {
    case MOV_UP:
        c = 'U';
        break;
    case MOV_LEFT:
        c = 'L';
        break;
    case MOV_RIGHT:
        c = 'R';
        break;
    case MOV_DOWN:
        c = 'D';
        break;
    }
    return c;
}

mossa mov_inv(mossa mov)
{
    switch (mov)
    {
    case MOV_UP: return MOV_DOWN;
    case MOV_RIGHT: return MOV_LEFT;
    case MOV_DOWN: return MOV_UP;
    case MOV_LEFT: return MOV_RIGHT;
    }
}
coordinate mov_apply(int r, int c, mossa mov)
{
    if (mov == MOV_UP) return make_pair(r - 1, c);
    else if (mov == MOV_DOWN) return make_pair(r + 1, c);
    else if (mov == MOV_LEFT) return make_pair(r, c - 1);
    else return make_pair(r, c + 1);
}
coordinate mov_apply(coordinate coord, mossa mov)
{
    return mov_apply(coord.first, coord.second, mov);
}

bool same_orientation(mossa mov1, mossa mov2)
{
    return ((mov1 & MOVH && mov2 & MOVH) || (mov1 & MOVV && mov2 & MOVV));
}

/* ------------------------------------------ */

void show_path(void)
{
    /*
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
    */
}