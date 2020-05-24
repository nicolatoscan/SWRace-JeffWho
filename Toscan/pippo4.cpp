#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>
#include <ctime>
#include <queue>
#include <set>
#include <sstream>
//#include "swrace.h"

using namespace std;

void findRedZonePath(int r, int c);
int findNextInRedRec(int r, int c, int from, bool avevaCurvato, bool deveCurvare, bool deveAndareDritto, int leftDepth);
void printSol();

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
#define ANELLO_ANY (128 | 64)

#define UP 1
#define DOWN 2
#define LEFT 4
#define RIGHT 8

#define VOID_TRYES 8
#define MAX_BF_DEPTH 10

int R, C;
int W, B;
int A;
int **mat;
int **redSol;
bool **visitedTemp; //FORSE UNIRE
int **visited;      //FORSE UNIRE
int solI = 0;
//int **maxFromHotsot; //TODO: sdfg

int *redZonePoints;
int *redZonePointsOr;
int redZonePointLenght;
int *sol;
Anello *anelli;

#ifdef EVAL
ifstream in("input.txt");
ofstream out("output.txt");
#else
ifstream in("../input/input18.txt");
ostream &out(cout);
#endif

int currentPointCounter = 0;

int main()
{
    srand(time(NULL));
    in >> R >> C;
    in >> B >> W;
    A = B + W;
    anelli = new Anello[B + W];

    mat = new int *[R];
    visited = new int *[R];
    visitedTemp = new bool *[R];
    sol = new int[R * C];
    redSol = new int *[R];
    visited = new int *[R];

    for (int r = 0; r < R; r++)
    {
        visited[r] = new int[C];
        visitedTemp[r] = new bool[C];
        mat[r] = new int[C];
        redSol[r] = new int[C];
        for (int c = 0; c < C; c++)
        {
            visited[r][c] = 0;
            redSol[r][c] = 0;
            visitedTemp[r][c] = false;
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

    printSol();
    redSol[0][0] = RIGHT;
    findRedZonePath(99, 69);
    cout << " ----- " << currentPointCounter << endl;
    printSol();


    //printRed();

    //printNMat(redSol);
    return 0;
}


void findRedZonePath(int r, int c)
{
    int dir = redSol[r][c];
    //SITEMA QUESTO PERCHE ORA VA ANCHE FUORI DA LA
    //PROVARE AD IMPLEMENTARE UN CLEAR RED ZONE

    int thisDir = redSol[r][c];
    if (thisDir == UP)
    {
        findNextInRedRec(r - 1, c, DOWN, false, false, false, MAX_BF_DEPTH);
    }
    else if (thisDir == DOWN)
    {
        findNextInRedRec(r + 1, c, UP, false, false, false, MAX_BF_DEPTH);
    }
    else if (thisDir == LEFT)
    {
        findNextInRedRec(r, c - 1, RIGHT, false, false, false, MAX_BF_DEPTH);
    }
    else if (thisDir == RIGHT)
    {
        findNextInRedRec(r, c + 1, LEFT, false, false, false, MAX_BF_DEPTH);
    }
}

set<pair<int, int>> used;

int findNextInRedRec(int r, int c, int from, bool avevaCurvato, bool deveCurvare, bool deveAndareDritto, int leftDepth)
{
    int point = 0;
    if (r < 0 || r >= R || c < 0 || c >= C)
        return -1;
    if (deveCurvare && deveAndareDritto)
        return -1;
    if (used.count(make_pair(r, c)))
        return -1;
    used.insert(make_pair(r, c));

    if (redSol[r][c] != 0)
        return -1;
    if (leftDepth != MAX_BF_DEPTH && visitedTemp[r][c]) // MAYBE SET
        return -1;

    int me = mat[r][c];
    if (((me & ANELLO_BLACK) && deveAndareDritto) || ((me & ANELLO_WHITE) && deveCurvare))
        return -1;
    if ((me & ANELLO_BLACK) && avevaCurvato)
        return -1;

    if (me & (ANELLO_BLACK | ANELLO_WHITE))
        point++;

    if (leftDepth <= 0)
        return point;
    bool nextDeveCurvare = ((me & ANELLO_WHITE) && !avevaCurvato) ? true : false;
    bool nextDeveAndareDritto = (me & ANELLO_BLACK) ? true : false;

    int nextDir = 0;

    if (deveAndareDritto || (me & ANELLO_WHITE)) //1 Possibilità
    {
        if (from == UP)
            nextDir |= DOWN;
        else if (from == DOWN)
            nextDir |= UP;
        else if (from == LEFT)
            nextDir |= RIGHT;
        else if (from == RIGHT)
            nextDir |= LEFT;
    }
    else // > 1 possibilità
    {
        if (deveCurvare || (me & ANELLO_BLACK))
        {
            if (from == UP || from == DOWN)
                nextDir |= (LEFT | RIGHT);
            else
                nextDir |= (UP | DOWN);
        }
        else
        {
            nextDir = (UP | DOWN | LEFT | RIGHT) & ~(from);
        }

        //CERCA ANELLI
        // int preferedDir = 0;
        // if (r > 0 && (mat[r - 1][c] & ANELLO_ANY))
        //     preferedDir |= UP;
        // if (r < R - 1 && (mat[r + 1][c] & ANELLO_ANY))
        //     preferedDir |= DOWN;
        // if (c > 0 && (mat[r][c - 1] & ANELLO_ANY))
        //     preferedDir |= LEFT;
        // if (c < C - 1 && (mat[r][c + 1] & ANELLO_ANY))
        //     preferedDir |= RIGHT;
    }

    pair<int, int> *toSort = new pair<int, int>[4];
    int nrOfPossibility = 0;
    if (nextDir & UP)
    {
        int p = findNextInRedRec(r - 1, c, DOWN, (from & (LEFT | RIGHT)), nextDeveCurvare, nextDeveAndareDritto, leftDepth - 1);
        used.erase(make_pair(r - 1, c));
        toSort[nrOfPossibility++] = make_pair(p, UP);
    }
    if (nextDir & DOWN)
    {
        int p = findNextInRedRec(r + 1, c, UP, (from & (LEFT | RIGHT)), nextDeveCurvare, nextDeveAndareDritto, leftDepth - 1);
        used.erase(make_pair(r + 1, c));
        toSort[nrOfPossibility++] = make_pair(p, DOWN);
    }
    if (nextDir & LEFT)
    {
        int p = findNextInRedRec(r, c - 1, RIGHT, (from & (UP | DOWN)), nextDeveCurvare, nextDeveAndareDritto, leftDepth - 1);
        used.erase(make_pair(r, c - 1));
        toSort[nrOfPossibility++] = make_pair(p, LEFT);
    }
    if (nextDir & RIGHT)
    {
        int p = findNextInRedRec(r, c + 1, LEFT, (from & (UP | DOWN)), nextDeveCurvare, nextDeveAndareDritto, leftDepth - 1);
        used.erase(make_pair(r, c + 1));
        toSort[nrOfPossibility++] = make_pair(p, RIGHT);
    }
    sort(toSort, toSort + nrOfPossibility);
    if (nrOfPossibility == 0)
        return -1;
    int maxPoint = toSort[nrOfPossibility - 1].first;

    if (leftDepth == MAX_BF_DEPTH)
    {
        if (maxPoint < 0)
            return -1;

        //GO DRITTO SE SI PUO
        bool goingDritto = false;
        int nextDefDir = 0;
        if (from == DOWN)
            nextDefDir = UP;
        else if (from == UP)
            nextDefDir = DOWN;
        else if (from == LEFT)
            nextDefDir = RIGHT;
        else if (from == RIGHT)
            nextDefDir = LEFT;

        for (int i = 0; i < nrOfPossibility; i++)
        {
            if (toSort[i].second == nextDefDir)
            {
                if (toSort[i].first == toSort[nrOfPossibility - 1].first)
                {
                    auto temp = toSort[i];
                    toSort[i] = toSort[nrOfPossibility - 1];
                    toSort[nrOfPossibility - 1] = temp;
                    goingDritto = true;
                }
            }
        }

        if (me & (ANELLO_BLACK | ANELLO_WHITE))
            currentPointCounter++;

        //Fai random;
        if (!goingDritto)
        {
            int nrUguali = 0;
            for (int i = 0; i < nrOfPossibility; i++)
                if (toSort[i].first == toSort[nrOfPossibility - 1].first)
                    nrUguali++;
            if (nrUguali > 1)
            {
                int randomN = (rand() % nrUguali);
                auto temp = toSort[nrOfPossibility - randomN - 1];
                toSort[nrOfPossibility - randomN - 1] = toSort[nrOfPossibility - 1];
                toSort[nrOfPossibility - 1] = temp;
            }
        }
        int ree;
        switch (toSort[nrOfPossibility - 1].second)
        {
        case UP:
            cout << "R: " << r - 1 << " C: " << c << "\t-> GO UP" << endl;
            redSol[r][c] = UP;
            visitedTemp[r - 1][c] = true;
            ree = findNextInRedRec(r - 1, c, DOWN, (from & (LEFT | RIGHT)), nextDeveCurvare, nextDeveAndareDritto, leftDepth);
            used.erase(make_pair(r - 1, c));
            return ree;
            break;
        case DOWN:
            cout << "R: " << r + 1 << " C: " << c << "\t-> GO DOWN" << endl;
            redSol[r][c] = DOWN;
            visitedTemp[r + 1][c] = true;
            ree = findNextInRedRec(r + 1, c, UP, (from & (LEFT | RIGHT)), nextDeveCurvare, nextDeveAndareDritto, leftDepth);
            used.erase(make_pair(r + 1, c));
            return ree;
            break;
        case LEFT:
            cout << "R: " << r << " C: " << c - 1 << "\t-> GO LEFT" << endl;
            redSol[r][c] = LEFT;
            visitedTemp[r][c - 1] = true;
            ree = findNextInRedRec(r, c - 1, RIGHT, (from & (UP | DOWN)), nextDeveCurvare, nextDeveAndareDritto, leftDepth);
            used.erase(make_pair(r, c - 1));
            return ree;
            break;
        case RIGHT:
            cout << "R: " << r << " C: " << c + 1 << "\t-> GO RIGHT" << endl;
            redSol[r][c] = RIGHT;
            visitedTemp[r][c + 1] = true;
            ree = findNextInRedRec(r, c + 1, LEFT, (from & (UP | DOWN)), nextDeveCurvare, nextDeveAndareDritto, leftDepth);
            used.erase(make_pair(r, c + 1));
            return ree;
            break;

        default:
            break;
        }
    }

    return maxPoint + point;
}

void printSol()
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
            if (n == UP)
                ccc = "↑";
            else if (n == DOWN)
                ccc = "↓";
            else if (n == LEFT)
                ccc = "←";
            else if (n == RIGHT)
                ccc = "→";
            else
            {
                ccc = "0";
                color = 37;
            }

            if (mat[r][c] & ANELLO_BLACK)
                color = 32;
            if (mat[r][c] & ANELLO_WHITE)
                color = 34;

            cout << "\033[" << color << "m " << ccc << " \033[0m";
        }
        cout << endl;
    }
    cout << endl;
}
