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
#include <string.h>
// #include "swrace.h"

using namespace std;

void printMat();
int distance(int x1, int y1, int x2, int y2);
void printRed();
void printNMat(int **mat);
void redZone();
void unione(int *a, int x, int y);
int find(int *a, int n);
int findNextInRedRec(int r, int c, int color, int from, bool avevaCurvato, bool deveCurvare, bool deveAndareDritto, int leftDepth);
pair<int, int> nextRedZone(int r, int c, bool findStart, int rStart, int cStart);
pair<int, int> findRedZonePath(int r, int c);
bool findPath(int r, int c);
void printSol();
string printDir(int dir);
void partiDappertutto();
bool printSolCorrector(int r, int c);

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
#define MAX_BF_DEPTH 6

set<pair<int, int>> used;

int R, C;
int W, B;
int A;
int **mat;
int **red;
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
ifstream in("../input/input19.txt");
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
    red = new int *[R];
    visited = new int *[R];
    visitedTemp = new bool *[R];
    sol = new int[R * C];
    redSol = new int *[R];
    visited = new int *[R];

    for (int r = 0; r < R; r++)
    {
        visited[r] = new int[C];
        visitedTemp[r] = new bool[C];
        red[r] = new int[C];
        mat[r] = new int[C];
        redSol[r] = new int[C];
        for (int c = 0; c < C; c++)
        {
            visited[r][c] = 0;
            redSol[r][c] = 0;
            visitedTemp[r][c] = false;
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
    while (true)
        partiDappertutto();
    //printRed();

    //printNMat(redSol);
    return 0;
}
int maxPointEver = 0;
void partiDappertutto()
{
    int *ii = new int[R * C];
    for (int r = 0; r < R; r++)
        for (int c = 0; c < R; c++)
            ii[(r * C) + c] = (r * C) + c;

    random_shuffle(&ii[0], &ii[R*C]);

    for (int i = 0; i < (R*C); i++)
    {
        int index = ii[i];
        int c = index % C;
        int r = index / R;

        if (red[r][c] < 0)
        {
            //RESET
            for (int r = 0; r < R; r++)
            {
                memset(visited[r], 0, C * sizeof(*visited[r]));
                memset(redSol[r], 0, C * sizeof(*redSol[r]));
                memset(visitedTemp[r], false, C * sizeof(*visitedTemp[r]));
                // for (int c = 0; c < C; c++)
                // {
                //     visited[r][c] = 0;
                //     redSol[r][c] = 0;
                //     visitedTemp[r][c] = false;
                // }
            }

            for (int i = 0; i < redZonePointLenght; i++)
                redZonePoints[i] = redZonePointsOr[i];

            currentPointCounter = 0;
            int isClosed = findPath(r, c);
            cout << "R: " << r << " C: " << c << " POINTS: " << currentPointCounter << endl;

            if (isClosed)
                currentPointCounter *= 2;
            if (currentPointCounter > maxPointEver)
            {
                if (printSolCorrector(r, c))
                    maxPointEver = currentPointCounter;
                //printSol();
            }
        }
    }
}

bool findPath(int r, int c)
{
    auto next = make_pair(r, c);
    auto res = next;
    int startR = r, startC = c;

    while (res.first != -1 && res.second != -1)
    {
        res = nextRedZone(next.first, next.second, false, -1, -1);
        // printSol();
        // cout << res.first << " " << res.second << " - from searching red" << endl;

        if (res.first != -1 && res.second != -1)
            next = res;
        else
            continue;

        res = findRedZonePath(next.first, next.second);
        // printSol();
        // cout << res.first << " " << res.second << " - from red zone" << endl;
        if (res.first != -1 && res.second != -1)
            next = res;
    }
    int backUp = redSol[startR][startC];
    redSol[startR][startC] = 0;
    res = nextRedZone(next.first, next.second, true, startR, startC);
    redSol[startR][startC] = backUp;
    //printSol();

    if (res.first != -1 && res.second != -1)
        return true;
    else
        return false;
    // cout << next.first << " " << next.second << " - FROM ENDING" << endl;
}

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

    redZonePointLenght = nr;
    redZonePointsOr = new int[redZonePointLenght];
    redZonePoints = new int[redZonePointLenght];
    for (int i = 0; i < A; i++)
        redZonePointsOr[red[anelli[i].r][anelli[i].c]]++;
}

int voidTrysLeft = VOID_TRYES;
int puntiInZonaToDo = 6;
pair<int, int> latestPoint;
pair<int, int> findRedZonePath(int r, int c)
{
    voidTrysLeft = VOID_TRYES;
    int dir = redSol[r][c];
    //SITEMA QUESTO PERCHE ORA VA ANCHE FUORI DA LA
    //PROVARE AD IMPLEMENTARE UN CLEAR RED ZONE
    puntiInZonaToDo = redZonePoints[red[r][c]];

    //TODO: fix direction
    latestPoint = make_pair(-1, -1);

    used.clear();

    int thisDir = redSol[r][c];
    if (thisDir == UP)
    {
        puntiInZonaToDo = redZonePoints[red[r - 1][c]];
        findNextInRedRec(r - 1, c, red[r - 1][c], DOWN, false, false, false, MAX_BF_DEPTH);
        redZonePoints[red[r - 1][c]] = -1;
    }
    else if (thisDir == DOWN)
    {
        puntiInZonaToDo = redZonePoints[red[r + 1][c]];
        findNextInRedRec(r + 1, c, red[r + 1][c], UP, false, false, false, MAX_BF_DEPTH);
        redZonePoints[red[r + 1][c]] = -1;
    }
    else if (thisDir == LEFT)
    {
        puntiInZonaToDo = redZonePoints[red[r][c - 1]];
        findNextInRedRec(r, c - 1, red[r][c - 1], RIGHT, false, false, false, MAX_BF_DEPTH);
        redZonePoints[red[r][c - 1]] = -1;
    }
    else if (thisDir == RIGHT)
    {
        puntiInZonaToDo = redZonePoints[red[r][c + 1]];
        findNextInRedRec(r, c + 1, red[r][c + 1], LEFT, false, false, false, MAX_BF_DEPTH);
        redZonePoints[red[r][c + 1]] = -1;
    }

    return latestPoint;
}

int findNextInRedRec(int r, int c, int color, int from, bool avevaCurvato, bool deveCurvare, bool deveAndareDritto, int leftDepth)
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
    if (red[r][c] != color)
        return 0;
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
        int p = findNextInRedRec(r - 1, c, color, DOWN, (from & (LEFT | RIGHT)), nextDeveCurvare, nextDeveAndareDritto, leftDepth - 1);
        used.erase(make_pair(r - 1, c));
        toSort[nrOfPossibility++] = make_pair(p, UP);
    }
    if (nextDir & DOWN)
    {
        int p = findNextInRedRec(r + 1, c, color, UP, (from & (LEFT | RIGHT)), nextDeveCurvare, nextDeveAndareDritto, leftDepth - 1);
        used.erase(make_pair(r + 1, c));
        toSort[nrOfPossibility++] = make_pair(p, DOWN);
    }
    if (nextDir & LEFT)
    {
        int p = findNextInRedRec(r, c - 1, color, RIGHT, (from & (UP | DOWN)), nextDeveCurvare, nextDeveAndareDritto, leftDepth - 1);
        used.erase(make_pair(r, c - 1));
        toSort[nrOfPossibility++] = make_pair(p, LEFT);
    }
    if (nextDir & RIGHT)
    {
        int p = findNextInRedRec(r, c + 1, color, LEFT, (from & (UP | DOWN)), nextDeveCurvare, nextDeveAndareDritto, leftDepth - 1);
        used.erase(make_pair(r, c + 1));
        toSort[nrOfPossibility++] = make_pair(p, RIGHT);
    }
    sort(toSort, toSort + nrOfPossibility);
    if (nrOfPossibility == 0)
        return -1;
    int maxPoint = toSort[nrOfPossibility - 1].first;

    if (leftDepth == MAX_BF_DEPTH && maxPoint > -1)
    {
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

        if (puntiInZonaToDo == 0)
            puntiInZonaToDo--;

        if (puntiInZonaToDo < 0)
        {
            redSol[r][c] = toSort[nrOfPossibility - 1].second;
            if (!deveAndareDritto && !deveCurvare)
                return 0;
        }
        if (voidTrysLeft == 0)
        {
            // cout << "FF" << endl;
            return -3;
        }

        if (me & (ANELLO_BLACK | ANELLO_WHITE))
        {
            voidTrysLeft = VOID_TRYES;
            currentPointCounter++;
            puntiInZonaToDo--;
        }
        else
            voidTrysLeft--;

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
            // cout << "R: " << r - 1 << " C: " << c << "\t-> GO UP" << endl;
            redSol[r][c] = UP;
            visitedTemp[r - 1][c] = true;
            latestPoint = make_pair(r - 1, c);
            ree = findNextInRedRec(r - 1, c, color, DOWN, (from & (LEFT | RIGHT)), nextDeveCurvare, nextDeveAndareDritto, leftDepth);
            used.erase(make_pair(r - 1, c));
            return ree;
            break;
        case DOWN:
            // cout << "R: " << r + 1 << " C: " << c << "\t-> GO DOWN" << endl;
            redSol[r][c] = DOWN;
            visitedTemp[r + 1][c] = true;
            latestPoint = make_pair(r + 1, c);
            ree = findNextInRedRec(r + 1, c, color, UP, (from & (LEFT | RIGHT)), nextDeveCurvare, nextDeveAndareDritto, leftDepth);
            used.erase(make_pair(r + 1, c));
            return ree;
            break;
        case LEFT:
            // cout << "R: " << r << " C: " << c - 1 << "\t-> GO LEFT" << endl;
            redSol[r][c] = LEFT;
            visitedTemp[r][c - 1] = true;
            latestPoint = make_pair(r, c - 1);
            ree = findNextInRedRec(r, c - 1, color, RIGHT, (from & (UP | DOWN)), nextDeveCurvare, nextDeveAndareDritto, leftDepth);
            used.erase(make_pair(r, c - 1));
            return ree;
            break;
        case RIGHT:
            // cout << "R: " << r << " C: " << c + 1 << "\t-> GO RIGHT" << endl;
            redSol[r][c] = RIGHT;
            visitedTemp[r][c + 1] = true;
            latestPoint = make_pair(r, c + 1);
            ree = findNextInRedRec(r, c + 1, color, LEFT, (from & (UP | DOWN)), nextDeveCurvare, nextDeveAndareDritto, leftDepth);
            used.erase(make_pair(r, c + 1));
            return ree;
            break;

        default:
            break;
        }
    }

    return maxPoint + point;
}

pair<int, int> nextRedZone(int r, int c, bool findStart, int rStart, int cStart)
{
    if (red[r][c] && redZonePoints[red[r][c]] > 0)
        return make_pair(r, c);

    int orR = r, orC = c;

    for (int rr = 0; rr < R; rr++)
    {
        for (int cc = 0; cc < C; cc++)
        {
            int color = red[rr][cc];
            bool blocked = false;
            if (mat[rr][cc] != 0 || redSol[rr][cc] != 0)
                blocked = true;

            visited[rr][cc] = blocked ? -1 : 0;
        }
    }

    queue<pair<int, int>> q;
    q.push(make_pair(r, c));
    int colorFound;
    visited[r][c] = UP; //TODO: fix
    pair<int, int> res;
    bool found = false;

    while (!q.empty())
    {
        auto n = q.front();
        r = n.first;
        c = n.second;
        q.pop();

        colorFound = red[r][c];
        if (
            (colorFound >= 0 && redZonePoints[colorFound] > 0) ||
            (findStart && ((r == rStart && c == cStart))))
        {
            res = make_pair(r, c);
            found = true;
            break;
        }

        if (r > 0 && (visited[r - 1][c] == 0))
        {
            visited[r - 1][c] = DOWN;
            q.push(make_pair(r - 1, c));
        }
        if (r < R - 1 && (visited[r + 1][c]) == 0)
        {
            visited[r + 1][c] = UP;
            q.push(make_pair(r + 1, c));
        }
        if (c > 0 && (visited[r][c - 1]) == 0)
        {
            visited[r][c - 1] = RIGHT;
            q.push(make_pair(r, c - 1));
        }
        if (c < C - 1 && (visited[r][c + 1]) == 0)
        {
            visited[r][c + 1] = LEFT;
            q.push(make_pair(r, c + 1));
        }
    }
    if (!found) //NON TROVATO
        return make_pair(-1, -1);

    //DELETE LAST PERCHE FORSE FUNZIONA MEGLIO BOH NON SO
    int last = visited[res.first][res.second];
    if (last == UP)
        res.first--;
    else if (last == DOWN)
        res.first++;
    else if (last == LEFT)
        res.second--;
    else if (last == RIGHT)
        res.second++;

    int nextDir;
    do
    {
        nextDir = visited[r][c];
        switch (nextDir)
        {
        case UP:
            // cout << "UP" << endl;
            r--;
            redSol[r][c] = DOWN;
            break;
        case DOWN:
            // cout << "DOWN" << endl;
            r++;
            redSol[r][c] = UP;
            break;
        case LEFT:
            // cout << "LEFT" << endl;
            c--;
            redSol[r][c] = RIGHT;
            break;
        case RIGHT:
            // cout << "RIGHT" << endl;
            c++;
            redSol[r][c] = LEFT;
            break;

        default:
            break;
        }
    } while (r != orR || c != orC);

    return res;
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

int distance(int x1, int y1, int x2, int y2)
{
    return (abs(x1 - x2) + abs(y1 - y2));
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
            if (n > 0)
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

string printDir(int dir)
{
    if (dir == UP)
        return "up";
    if (dir == DOWN)
        return "down";
    if (dir == LEFT)
        return "left";
    if (dir == RIGHT)
        return "right";
    return "dir not found";
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

int maxPuntiVeri = 0;
bool printSolCorrector(int r, int c)
{
    // CHECKER
    int prevDir = 0;
    int prevPrevDir = 0;
    bool nextDeveAndareDritto = false;
    bool nextDeveCurvare = false;
    bool errore = false;
    // end CHECKER

    //printSol();
    int stR = r, stC = c;
    stringstream ss;
    int i = 0;
    int d = redSol[r][c];
    int punti = 0;
    bool closed = false;

    do
    {

        int me = mat[r][c];
        if (nextDeveAndareDritto && nextDeveCurvare)
        {
            errore = true;
            break;
        }
        if (nextDeveAndareDritto)
        {
            if (d != prevDir)
            {
                errore = true;
                break;
            }
        }
        if (nextDeveCurvare)
        {
            if (d == prevDir)
            {
                errore = true;
                break;
            }
        }
        nextDeveAndareDritto = false;
        nextDeveCurvare = false;

        if ((me & (ANELLO_BLACK | ANELLO_WHITE)) > 0)
        {
            punti++;
            if (me & ANELLO_BLACK)
            {
                if (!(d != prevDir && prevDir == prevPrevDir))
                {
                    errore = true;
                    break;
                }
                nextDeveAndareDritto = true;
            }
            else if (me & ANELLO_WHITE)
            {
                if (d != prevDir)
                {
                    errore = true;
                    break;
                }
                if (prevDir == prevPrevDir)
                    nextDeveCurvare = true;
            }
        }

        switch (d)
        {
        case UP:
            ss << "U";
            r--;
            break;
        case DOWN:
            ss << "D";
            r++;
            break;
        case LEFT:
            ss << "L";
            c--;
            break;
        case RIGHT:
            ss << "R";
            c++;
            break;

        default:
            break;
        }
        i++;

        prevPrevDir = prevDir;
        prevDir = d;
        d = redSol[r][c];
        if (r == stR && c == stC)
        {
            closed = true;
            d = -1;
        }

    } while (d > 0);

    if (errore || nextDeveAndareDritto || nextDeveCurvare)
        return false;

    ss << "#";

    int anelli = punti;

    if (closed)
    {
        punti *= 2;
        cout << "PUNTI: " << punti << endl;
    }

    if (punti > maxPuntiVeri)
    {
        out << anelli << " "
            << i << " "
            << stR << " "
            << stC << " "
            << ss.str() << endl;
    }

    return true;
}