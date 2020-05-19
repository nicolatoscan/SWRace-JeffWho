#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>
#include <ctime>
#include <queue>
#include <sstream>

using namespace std;

#ifdef EVAL
ifstream in("input.txt");
ofstream out("output.txt");
#else
ifstream in("../input/input11.txt");
ostream &out(cout);
#endif

#define ANELLO_NONE 0
#define ANELLO_WHITE 1
#define ANELLO_BLACK 2

#define UP 1
#define RIGHT 2
#define DOWN 4
#define LEFT 8
#define DIR_ALL 1 & 2 & 4 & 8
#define DIR_NONE 0

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
int **mat;
int **dir;
Anello *anelli;

int main()
{

    for (int file = 0; file < 20; file++)
    {
        stringstream ins;
        stringstream outs;
        ins << "../input/input" << file << ".txt";
        outs << "../mat/mat" << file << ".txt";
        ifstream in(ins.str());
        ofstream out(outs.str());
        in >> R >> C;
        in >> B >> W;
        A = B + W;
        anelli = new Anello[B + W];
        mat = new int *[R];
        dir = new int *[R];
        for (int r = 0; r < R; r++)
        {
            mat[r] = new int[C];
            dir[r] = new int[C];
            for (int c = 0; c < C; c++)
            {
                mat[r][c] = ANELLO_NONE;
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
            mat[r][c] = (i < B) ? ANELLO_BLACK : ANELLO_WHITE;
        }

        for (int r = 0; r < R; r++)
        {
            for (int c = 0; c < C; c++)
            {
                string o;
                switch (mat[r][c])
                {
                case ANELLO_BLACK:
                    o = "⬤";
                    break;
                case ANELLO_WHITE:
                    o = "◯";
                    break;
                default:
                    o = "☐";
                    break;
                }
                out << o << " ";
            }
            out << endl;
        }
    }
    return 0;
}

void createDir()
{

    for (int i = 0; i < A; i++)
    {
        Anello a = anelli[i];
    }
}

void printMat()
{
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            string o;
            switch (mat[r][c])
            {
            case ANELLO_BLACK:
                o = "⬤";
                break;
            case ANELLO_WHITE:
                o = "◯";
                break;
            default:
                o = "☐";
                break;
            }
            cout << o << " ";
        }
        cout << endl;
    }
}