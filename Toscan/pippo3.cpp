#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>
#include <ctime>
#include <queue>

using namespace std;

#ifdef EVAL
ifstream in("input.txt");
ofstream out("output.txt");
#else
ifstream in("../input/input7.txt");
ostream &out(cout);
#endif

void printMat();
int distance(int x1, int y1, int x2, int y2);
void printRed();
void printNMat(int **mat);
void redZone();
void unione(int *a, int x, int y);
int find(int *a, int n);

struct Anello
{
    int r;
    int c;
    int color;
    int near;
    int used;
    int redZone;
};

#define ANELLO_WHITE 64
#define ANELLO_BLACK 128

#define UP 1
#define DOWN 2
#define LEFT 4
#define RIGHT 8

int R, C;
int W, B;
int A;
int **mat;
int **red;
int **visited;
//int **maxFromHotsot; //TODO: sdfg

int *redZonePoints;
int *sol;

Anello *anelli;

int main()
{
    in >> R >> C;
    in >> B >> W;
    A = B + W;
    anelli = new Anello[B + W];

    mat = new int *[R];
    red = new int *[R];
    visited = new int *[R];
    sol = new int[R * C];
    for (int r = 0; r < R; r++)
    {
        visited[r] = new int[C];
        red[r] = new int[C];
        mat[r] = new int[C];
        for (int c = 0; c < C; c++)
        {
            visited[r][c] = 0;
            red[r][c] = -1;
            mat[r][c] = 0;
        }
    }

    for (int i = 0; i < A; i++)
    {
        int r, c;
        in >> r >> c;
        Anello a;
        a.r = r;
        a.c = c;
        a.color = (i < B) ? ANELLO_BLACK : ANELLO_WHITE;
        a.used = false;
        a.redZone = -1;
        anelli[i] = a;
        mat[r][c] = a.color;
    }

    redZone();
    //printMat();
    printRed();
    return 0;
}

int distance(int x1, int y1, int x2, int y2)
{
    return (abs(x1 - x2) + abs(y1 - y2));
}

// ----- RED ZONES -----

void redZone()
{

    int *toFix = new int[A];
    for (int i = 0; i < A; i++)
        toFix[i] = -2;

    for (int i = 0; i < A; i++)
    {
        Anello *a = &anelli[i];
        int r = a->r, c = a->c;
        int anelliVicini = 0;
        int ANLEBBO_BN = ANELLO_WHITE | ANELLO_BLACK;

        if (r < R - 1 && mat[r + 1][c] & ANLEBBO_BN)
            anelliVicini++;
        if (r > 0 && mat[r - 1][c] & ANLEBBO_BN)
            anelliVicini++;
        if (c < C - 1 && mat[r][c + 1] & ANLEBBO_BN)
            anelliVicini++;
        if (c > 0 && mat[r][c - 1] & ANLEBBO_BN)
            anelliVicini++;

        a->near = anelliVicini;
    }

    for (int i = 0; i < A; i++)
    {
        Anello *a = &anelli[i];
        int r = a->r;
        int c = a->c;
        red[r][c] = i;

        int len = 3;
        int *rig = new int[3]{r - 1, r, r + 1};
        int *col = new int[3]{c - 1, c, c + 1};
        if (a->color == ANELLO_BLACK && a->near <= 1)
        {
            len = 5;
            rig = new int[5]{r - 2, r - 1, r, r + 1, r + 2};
            col = new int[5]{c - 2, c - 1, c, c + 1, c + 2};
        }

        for (int k = 0; k < len; k++)
        {
            for (int j = 0; j < len; j++)
            {
                int rr = rig[k];
                int cc = col[j];
                if (rr >= 0 && rr < R && cc >= 0 && cc < C)
                {
                    int n = red[rr][cc];
                    if (n != -1)
                        unione(toFix, n, i);
                    red[rr][cc] = find(toFix, i);
                }
            }
        }
    }
    //JOIN
    for (int r = 0; r < R; r++)
        for (int c = 0; c < C; c++)
            if (red[r][c] != -1)
                red[r][c] = find(toFix, red[r][c]);

    //NORMALIZE
    int nr = 0;
    for (int i = 0; i < A; i++)
    {
        if (toFix[i] >= 0)
            toFix[i] = -1;
        else
            toFix[i] = nr++;
    }

    for (int r = 0; r < R; r++)
        for (int c = 0; c < C; c++)
            if (red[r][c] > 0)
                red[r][c] = toFix[red[r][c]];
    
    //fixRedZoneWR();
}

int find(int *a, int n)
{
    if (a[n] < 0)
        return n;
    return find(a, a[n]);
}

void unione(int *a, int x, int y)
{
    int xSet = find(a, x);
    int ySet = find(a, y);
    if (xSet != ySet)
        a[xSet] = ySet;
}

// ----- end RED ZONES -----

void doRed(int r, int c, int maxPoint)
{
    int points = 0;
    int lastCurva = 100;
    int avevoCurvato = false;
    int stavoAndandoDritto = false;

    //BFS
}

void printMat()
{
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            string o = "☐";
            if (mat[r][c] & ANELLO_WHITE)
                o = "◯";
            else if (mat[r][c] & ANELLO_BLACK)
                o = "⬤";

            cout << o << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void printNMat(int **mat)
{
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            int n = mat[r][c];
            if (n >= 0)
                cout << "\033[31m";
            if (n < 10 && n >= 0)
                cout << " " << n << " ";
            else
                cout << n << " ";
            if (n >= 0)
                cout << "\033[0m";
        }
        cout << endl;
    }
    cout << endl;
}

void printRed()
{
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            int color = 31;

            if (mat[r][c] & ANELLO_BLACK)
                color = 32;
            if (mat[r][c] & ANELLO_WHITE)
                color = 33;

            int n = red[r][c];
            if (n >= 0)
                cout << "\033[" << color << "m";
            if (n < 10 && n >= 0)
                cout << " " << n << " ";
            else
                cout << n << " ";
            if (n >= 0)
                cout << "\033[0m";
        }
        cout << endl;
    }
    cout << endl;
}