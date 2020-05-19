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
int **mat;
int **pathsDir;
int **paths;
int **dir;

Anello *anelli;

int main()
{

    in >> R >> C;
    in >> B >> W;
    A = B + W;
    anelli = new Anello[B + W];
    mat = new int *[R];
    paths = new int *[R];
    dir = new int *[R];
    pathsDir = new int *[R];
    for (int r = 0; r < R; r++)
    {
        pathsDir[r] = new int[C];
        mat[r] = new int[C];
        paths[r] = new int[C];
        dir[r] = new int[C];
        for (int c = 0; c < C; c++)
        {
            paths[r][c] = -1;
            mat[r][c] = ANELLO_NONE;
            pathsDir[r][c] = DIR_NONE;
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
        pathsDir[r][c] |= a.color;
        mat[r][c] = a.color;
    }
    //printMat();
    patha(7, 5, 1, LEFT, 1, 0, NOT_KNOWN, 0);
    //printPath();
    return 0;
}

void createPaths()
{

    int pathCounter = 1;
    for (int i = 0; i < A; i++)
    {
        Anello a = anelli[i];
        if (paths[a.r][a.c] > 0)
            continue;

        queue<pair<int, int>> q;
        q.push(make_pair(a.r, a.c));
        paths[a.r][a.c] = pathCounter;

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
    }
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

bool goOnPath(int c, int r, int pathNr, int curvato, int needTo, int fromDir, int tryes)
{
    if (paths[r][c] > 0)
    {
        cout << "OH NO!" << endl;
        return false;
    }

    paths[r][c] = pathNr;

    int me = dir[r][c];
    int up = 0, dw = 0, lf = 0, rg = 0;
    if (r > 0)
        up = dir[r - 1][c];
    if (r < R - 1)
        dw = dir[r + 1][c];
    if (c > 0)
        lf = dir[r][c - 1];
    if (c < C - 1)
        rg = dir[r][c + 1];

    if (me & ANELLO_BLACK) //BLACKs
    {
        if (needTo == DRITTO)
            return false;

        int stradeTrovate = 0;
        int stradeRotte = 0;
        if ((fromDir == UP) || (fromDir == DOWN))
        {
            if ((me & LEFT) && (lf && RIGHT) && (lf && LEFT) && !(lf & ANELLO_BLACK)) // GO LEFT
            {
                bool res = goOnPath(r, c - 1, pathNr, CURVATO, DRITTO, RIGHT, 1);
                if (res)
                    stradeTrovate++;
                else
                    stradeRotte++;
            }

            if ((me & RIGHT) && (rg && LEFT) && (rg && RIGHT) && !(rg & ANELLO_BLACK)) // GO RIGHT
            {
                bool res = goOnPath(r, c + 1, pathNr, CURVATO, DRITTO, LEFT, 1);
                if (res)
                    stradeTrovate++;
                else
                    stradeRotte++;
            }
        }
        else if ((fromDir == LEFT) || (fromDir == RIGHT))
        {
            if ((me & UP) && (up && DOWN) && (up && UP) && !(up & ANELLO_BLACK)) // GO UP
            {
                bool res = goOnPath(r - 1, c, pathNr, CURVATO, DRITTO, DOWN, 1);
                if (res)
                    stradeTrovate++;
                else
                    stradeRotte++;
            }

            if ((me & DOWN) && (dw && DOWN) && (dw && UP) && !(dw & ANELLO_BLACK)) // GO DOWN
            {
                bool res = goOnPath(r + 1, c, pathNr, CURVATO, DRITTO, UP, 1);
                if (res)
                    stradeTrovate++;
                else
                    stradeRotte++;
            }
        }

        if (stradeTrovate > 0)
            return true; //CONTINUA
        if (stradeRotte > 0)
        {
            paths[r][c] = 0;
            return false; //TUTTO ROTTO
        }
        return true; //FINITO
    }
    else if (me & ANELLO_WHITE) //BIANCO
    {
        if (fromDir == UP)
        {
        }
    }

    return 666;
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
            }
            if (nFoundDir > 0)
            {
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

void printPath()
{
        for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
            cout << paths[r][c] << " ";
        cout << endl;
    }
}