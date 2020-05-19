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

#define NOT_KNOWN 0
#define DRITTO 1
#define CURVATO 2

struct Anello
{
    int r;
    int c;
    int color;
};

// -- FUNCTION
void printMat();
void printPath();
int patha(int r, int c, int pathN, int from, int tryesLeft, int lastTurn, int needToGo, int depth);

int R, C;
int W, B;
int A;
int **pathsDir;
int **paths;
int **imbocchi;

Anello *anelli;

int main()
{

    in >> R >> C;
    in >> B >> W;
    A = B + W;
    anelli = new Anello[B + W];
    paths = new int *[R];
    pathsDir = new int *[R];
    imbocchi = new int *[R];
    for (int r = 0; r < R; r++)
    {
        pathsDir[r] = new int[C];
        paths[r] = new int[C];
        imbocchi[r] = new int[C];
        for (int c = 0; c < C; c++)
        {
            paths[r][c] = -1;
            pathsDir[r][c] = DIR_NONE;
            imbocchi[r][c] = 0;
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
        pathsDir[r][c] |= a.color;
    }
    //printMat();
    patha(7, 5, 1, LEFT, 1, 0, NOT_KNOWN, 0);
    //printPath();
    return 0;
}

int patha(int r, int c, int pathN, int from, int tryesLeft, int lastTurn, int needToGo, int depth)
{

    if (c < 0 || r < 0 || c >= C || r >= R)
        return -1;

    if (tryesLeft < 0) {
        return 0;
    }

    if (paths[r][c] > 0)
        return -1;

    for (int i = 0; i < depth; i++) cout << "\t";
    cout << "R: " << r << "C: " << c << endl;

    int me = pathsDir[r][c];

    int maxPoint = -1;
    int newDir = DIR_NONE;

    if (me & ANELLO_WHITE)
    {
        if (needToGo == CURVATO)
            return -1;

        int res;
        int testNewDir;
        switch (from)
        {
        case UP:
            res = patha(r + 1, c, pathN, UP, 1, lastTurn + 1, lastTurn == 0 ? NOT_KNOWN : CURVATO, depth + 1);
            testNewDir = UP & DOWN;
            break;
        case DOWN:
            res = patha(r - 1, c, pathN, DOWN, 1, lastTurn + 1, lastTurn == 0 ? NOT_KNOWN : CURVATO, depth + 1);
            testNewDir = UP & DOWN;
            break;
        case LEFT:
            res = patha(r, c + 1, pathN, LEFT, 1, lastTurn + 1, lastTurn == 0 ? NOT_KNOWN : CURVATO, depth + 1);
            testNewDir = LEFT & RIGHT;
            break;
        case RIGHT:
            res = patha(r, c - 1, pathN, RIGHT, 1, lastTurn + 1, lastTurn == 0 ? NOT_KNOWN : CURVATO, depth + 1);
            testNewDir = LEFT & RIGHT;
            break;

        default:
            break;
        }
        if (res >= 0)
        {
            maxPoint = res + 1;
            newDir = testNewDir;
        }
    }
    else if (me & ANELLO_BLACK)
    {
        if (needToGo == DRITTO)
            return false;

        int res;

        if (from == UP || from == DOWN)
        {
            res = patha(r, c - 1, pathN, RIGHT, 1, 0, DRITTO, depth + 1);
            if (res > maxPoint && res >= 0)
            {
                maxPoint = res + 1;
                newDir = (from & LEFT);
            }
            res = patha(r, c + 1, pathN, LEFT, 1, 0, DRITTO, depth + 1);
            if (res > maxPoint && res >= 0)
            {
                maxPoint = res + 1;
                newDir = (from & RIGHT);
            }
        }
        else if (from == LEFT || from == RIGHT)
        {
            res = patha(r - 1, c, pathN, DOWN, 1, 0, DRITTO, depth + 1);
            if (res > maxPoint && res >= 0)
            {
                maxPoint = res + 1;
                newDir = (from & UP);
            }
            res = patha(r + 1, c, pathN, UP, 1, 0, DRITTO, depth + 1);
            if (res > maxPoint && res >= 0)
            {
                maxPoint = res + 1;
                newDir = (from & DOWN);
            }
        }
    }
    else
    {
        if (tryesLeft <= 0)
            return false;

        int res;

        switch (from)
        {
        case UP:
            if (needToGo != DRITTO)
            {
                res = patha(r + 1, c, pathN, UP, tryesLeft - 1, lastTurn + 1, NOT_KNOWN, depth + 1); //DOWN
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & DOWN);
                }
            }
            if (needToGo != CURVATO)
            {
                res = patha(r, c - 1, pathN, RIGHT, tryesLeft - 1, 0, NOT_KNOWN, depth + 1); // LEFT
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & LEFT);
                }
                res = patha(r, c + 1, pathN, LEFT, tryesLeft - 1, 0, NOT_KNOWN, depth + 1); //RIGHT
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & RIGHT);
                }
            }
            break;

        case DOWN:
            if (needToGo != DRITTO)
            {
                res = patha(r - 1, c, pathN, DOWN, tryesLeft - 1, lastTurn + 1, NOT_KNOWN, depth + 1); //UP
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & UP);
                }
            }
            if (needToGo != CURVATO)
            {
                res = patha(r, c - 1, pathN, RIGHT, tryesLeft - 1, 0, NOT_KNOWN, depth + 1); //LEFT
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & LEFT);
                }
                res = patha(r, c + 1, pathN, LEFT, tryesLeft - 1, 0, NOT_KNOWN, depth + 1); //RIGHT
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & RIGHT);
                }
            }
            break;

        case LEFT:
            if (needToGo != DRITTO)
            {
                res = patha(r, c + 1, pathN, LEFT, tryesLeft - 1, lastTurn + 1, NOT_KNOWN, depth + 1); //RIGHT
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & RIGHT);
                }
            }
            if (needToGo != CURVATO)
            {
                res = patha(r + 1, c, pathN, UP, tryesLeft - 1, 0, NOT_KNOWN, depth + 1); //DOWN
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & DOWN);
                }
                res = patha(r - 1, c, pathN, DOWN, tryesLeft - 1, 0, NOT_KNOWN, depth + 1); //UP
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & UP);
                }
            }
            break;

        case RIGHT:
            if (needToGo != DRITTO)
            {

                res = patha(r, c - 1, pathN, RIGHT, tryesLeft - 1, lastTurn + 1, NOT_KNOWN, depth + 1); //LEFT
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & LEFT);
                }
            }
            if (needToGo != CURVATO)
            {
                res = patha(r + 1, c, pathN, UP, tryesLeft - 1, 0, NOT_KNOWN, depth + 1); //DOWN
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & DOWN);
                }
                res = patha(r - 1, c, pathN, DOWN, tryesLeft - 1, 0, NOT_KNOWN, depth + 1); //UP
                if (res > maxPoint && res >= 0)
                {
                    maxPoint = res;
                    newDir = (from & UP);
                }
            }
            break;

        default:
            break;
        }
    }

    cout << "MAX POINT: " << maxPoint << endl;
    if (maxPoint < 0)
        return -1;
    if (maxPoint == 0)
        newDir = from;

    paths[r][c] = pathN;
    pathsDir[r][c] = newDir;
    return maxPoint;
}

void printMat()
{
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            string o = "☐";
            if (pathsDir[r][c] & ANELLO_WHITE)
                o = "◯";
            else if (pathsDir[r][c] & ANELLO_BLACK)
                o = "⬤";

            cout << o << " ";
        }
        cout << endl;
    }
}

void printPath()
{
        for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
            cout << paths[r][c] << " ";
        cout << endl;
    }
}