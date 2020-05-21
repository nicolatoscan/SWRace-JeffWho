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
ifstream in("../input/input2.txt");
ostream &out(cout);
#endif

void printMat();


struct Anello
{
    int r;
    int c;
    int color;
    int near;
    int used;
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
            red[r][c] = 0;
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
        anelli[i] = a;
        mat[r][c] = a.color;
    }

    printMat();
    return 0;
}

void setHotspot()
{
    for (int i = 0; i < A; i++)
    {
        Anello *a = &anelli[i];
        int r = a->r, c = a->c;
        int anelliVicini = 0;
        int ANELLO_BN = ANELLO_WHITE | ANELLO_BLACK;

        int sx = (c > 0) ? mat[r][c - 1] : 0;
        int dx = (c < C - 1) ? mat[r][c + 1] : 0;
        int up = (r > 0) ? mat[r - 1][c] : 0;
        int dw = (r < R - 1) ? mat[r + 1][c] : 0;

        if (sx & ANELLO_BN)
            anelliVicini++;
        if (dx & ANELLO_BN)
            anelliVicini++;
        if (up & ANELLO_BN)
            anelliVicini++;
        if (dw & ANELLO_BN)
            anelliVicini++;

        a->near = anelliVicini;

        if (anelliVicini <= 1)
            red[r][c] = a->color;
    }
}

void findNextHotspot(int r, int c)
{
    for (int r = 0; r < R; r++)
        for (int c = 0; c < C; c++)
            visited[r][c] = 0;

    queue<pair<int, int>> q;
    q.push(make_pair(r, c));

    while (!q.empty())
    {
        auto cell = q.front();
        q.pop();
        r = cell.first;
        c = cell.second;

        int h = red[cell.first][cell.second];
        if (h == ANELLO_WHITE)
        {
        }
        else if (h == ANELLO_BLACK)
        {
        }
        else
        {
            if (r > 0 && visited[r - 1][c] == 0)
            {
                visited[r - 1][c] = UP;
                q.push(make_pair(r - 1, c));
            }

            if (r < R - 1 && visited[r + 1][c] == 0)
            {
                visited[r + 1][c] = DOWN;
                q.push(make_pair(r + 1, c));
            }
            if (c > 0 && visited[r][c - 1] == 0)
            {
                visited[r][c - 1] = LEFT;
                q.push(make_pair(r, c - 1));
            }
            if (c < C - 1 && visited[r][c + 1] == 0)
            {
                visited[r][c + 1] = RIGHT;
                q.push(make_pair(r, c + 1));
            }
        }
        visited[r][c] = 1;
    }
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