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
ifstream in("../input/input0.txt");
ostream &out(cout);
#endif

#define UP 1
#define RIGHT 2
#define DOWN 4
#define LEFT 8
#define DIR_ALL 1 & 2 & 4 & 8
#define DIR_NONE 0

#define ANELLO_NONE 16
#define ANELLO_WHITE 32
#define ANELLO_BLACK 64

struct Anello
{
    int r;
    int c;
    int color;
};

// -- FUNCTION
void printMat();

int R, C;
int W, B;
int A;
//int **mat;
int **dir;
Anello *anelli;

int main()
{

    in >> R >> C;
    in >> B >> W;
    A = B + W;
    anelli = new Anello[B + W];
    //mat = new int *[R];
    dir = new int *[R];
    for (int r = 0; r < R; r++)
    {
        //mat[r] = new int[C];
        dir[r] = new int[C];
        for (int c = 0; c < C; c++)
        {
            dir[r][c] = DIR_ALL;
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
        anelli[i] = a;
        dir[r][c] |= a.color;
    }
    printMat();
    return 0;
}

void createDir()
{
    for (int r = 0; r < R; r++)
    {
        dir[r][0] |= ~(LEFT);
        dir[r][C - 1] |= ~(RIGHT);
    }
    for (int c = 0; c < C; c++)
    {
        dir[0][c] |= ~(UP);
        dir[R - 1][c] |= ~(DOWN);
    }

    for (int i = 0; i < A; i++)
    {
        Anello a = anelli[i];

        int me = dir[a.r][a.c];
        int up = 0, dw = 0, lf = 0, rg = 0;

        if (a.r > 0)
            up = dir[a.r - 1][a.c];
        if (a.r < R - 1)
            dw = dir[a.r + 1][a.c];
        if (a.c > 0)
            lf = dir[a.r][a.c - 1];
        if (a.c < C - 1)
            rg = dir[a.r][a.c + 1];

        if (a.color == ANELLO_BLACK) // --- BLACK ---
        {

            // CERCA 2 BIANCHI AI LATI
            int newDir = DIR_NONE;
            int nFoundDir = 0;
            if ((me & UP) && (up & UP) && (up & DOWN) && (up & ANELLO_WHITE))
            {
                if ((me & LEFT) && (lf & LEFT) && (lf & RIGHT) && (lf & ANELLO_WHITE))
                {
                    newDir = (UP & LEFT);
                    nFoundDir++;
                }

                if ((me & RIGHT) && (rg & RIGHT) && (rg & LEFT) && (rg & ANELLO_WHITE))
                {
                    newDir = (UP & RIGHT);
                    nFoundDir++;
                }
            }

            if ((me & DOWN) && (dw & DOWN) && (dw & UP) && (dw & ANELLO_WHITE))
            {
                if ((me & LEFT) && (lf & LEFT) && (lf & RIGHT) && (lf & ANELLO_WHITE))
                {
                    newDir = (DOWN & LEFT);
                    nFoundDir++;
                }

                if ((me & RIGHT) && (rg & RIGHT) && (rg & LEFT) && (rg & ANELLO_WHITE))
                {
                    newDir = (DOWN & RIGHT);
                    nFoundDir++;
                }
            }

            if (nFoundDir > 0) {
                me = ANELLO_BLACK | newDir;
            }


        }
        else // --- WHITE ---
        {
        }

        dir[a.r][a.c] = me;
    }
}

void printMat()
{
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            string o = "☐";
            if (dir[r][c] & ANELLO_WHITE)
                o = "◯";
            else if (dir[r][c] & ANELLO_BLACK)
                o = "⬤";

            cout << o << " ";
        }
        cout << endl;
    }
}