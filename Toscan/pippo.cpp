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
ifstream in("../input/input5.txt");
ostream &out(cout);
#endif

void printMat();
void setWeight();
void go(int r, int c, int from, bool avevaCurvato, bool deveCurvare, bool deveAndareDritto, int points, int ii, int depth);
void newMax(int points, int n);
int distance(int x1, int y1, int x2, int y2);
void printWeight();
void setDistances();

struct Anello
{
    int r;
    int c;
    int color;
    int near;
    int used;
};

#define ANELLO_NONE 16
#define ANELLO_WHITE 32
#define ANELLO_BLACK 64

#define USED 128

#define UP 1
#define DOWN 2
#define LEFT 4
#define RIGHT 8
#define DIR_ALL 1 & 2 & 4 & 8
#define DIR_NONE 0

int R, C;
int W, B;
int A;
int **mat;
int **solMat;
int **weight;

int *sol;
int *solR;
int *solC;

Anello *anelli;

int maxPoint = 0;

int startR = 0;
int startC = 0;

int main()
{

    in >> R >> C;
    in >> B >> W;
    A = B + W;
    anelli = new Anello[B + W];

    mat = new int *[R];
    solMat = new int *[R];
    weight = new int *[R];

    sol = new int[R * C];
    solR = new int[R * C];
    solC = new int[R * C];
    for (int r = 0; r < R; r++)
    {
        weight[r] = new int[C];
        mat[r] = new int[C];
        solMat[r] = new int[C];
        for (int c = 0; c < C; c++)
        {
            weight[r][c] = INT32_MAX;
            mat[r][c] = 0;
            solMat[r][c] = 0;
        }
    }

    for (int i = 0; i < A; i++)
    {
        int r, c;
        in >> r >> c;
        Anello a;
        a.r = r;
        a.c = c;
        a.used = false;
        a.color = (i < B) ? ANELLO_BLACK : ANELLO_WHITE;
        anelli[i] = a;
        mat[r][c] = a.color;
    }

    printMat();
    setWeight();
    printWeight();
    go(0, 0, LEFT, true, false, false, 0, 0, 0);
    return 0;
}

void setWeight()
{
    //INFLUENCE
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

    setDistances();
}

void setDistances()
{
    for (int r = 0; r < R; r++)
        for (int c = 0; c < C; c++)
            weight[r][c] = INT32_MAX;

    for (int i = 0; i < A; i++)
    {
        Anello a = anelli[i];

        if (a.near <= 1)
            if ((mat[a.r][a.c] & USED) == 0)
                for (int r = 0; r < R; r++)
                    for (int c = 0; c < C; c++)
                        weight[r][c] = min(distance(a.r, a.c, r, c), weight[r][c]);
    }

    for (int i = 0; i < A; i++)
    {
        Anello a = anelli[i];
        if (a.near > 1)
            weight[a.r][a.c] = -1;
    }

    printWeight();
}

int distance(int x1, int y1, int x2, int y2)
{
    return (abs(x1 - x2) + abs(y1 - y2));
}

void go(int r, int c, int from, bool avevaCurvato, bool deveCurvare, bool deveAndareDritto, int points, int ii, int depth)
{

    if (r < 0 || c < 0 || r >= R || c >= C)
        return;

    if (deveAndareDritto && deveCurvare)
        return;

    //for (int i = 0; i < depth; i++) cout << " ";
    //cout << "R: " << r << " C: " << c; // << endl;

    //PUNTO DI PARTENZA
    if (startC == c && startR == r && ii != 0)
    {
        sol[ii] = from;
        solR[ii] = r;
        solC[ii] = c;
        if (points * 2 > maxPoint)
            newMax(points * 2, ii + 1);
        return;
    }

    int me = mat[r][c];
    if (me & USED)
        return;

    sol[ii] = from;
    solR[ii] = r;
    solC[ii] = c;

    mat[r][c] = me | USED;
    if ((me & ANELLO_BLACK) || (me & ANELLO_WHITE))
    {
        points++;
        setDistances();
    }

    if (points > maxPoint)
        newMax(points, ii);

    int upW = ((r == 0) ? -1 : weight[r - 1][c]);
    int downW = ((r == R - 1) ? -1 : weight[r + 1][c]);
    int leftW = ((c == 0) ? -1 : weight[r][c - 1]);
    int rightW = ((c == C - 1) ? -1 : weight[r][c + 1]);

    bool nextDeveCurvare = ((me & ANELLO_WHITE) && !avevaCurvato) ? true : false;
    bool nextDeveAndareDritto = (me & ANELLO_BLACK) ? true : false;

    if (from == UP)
        upW = -1;
    if (from == DOWN)
        downW = -1;
    if (from == RIGHT)
        rightW = -1;
    if (from == LEFT)
        leftW = -1;

    if (deveAndareDritto || (me & ANELLO_WHITE))
    {
        if (from != UP)
            downW = -1;
        if (from != DOWN)
            upW = -1;
        if (from != RIGHT)
            leftW = -1;
        if (from != LEFT)
            rightW = -1;
    }

    if (deveCurvare || (me & ANELLO_BLACK))
    {
        if (from == UP)
            downW = -1;
        if (from == DOWN)
            upW = -1;
        if (from == LEFT)
            rightW = -1;
        if (from == RIGHT)
            leftW = -1;
    }

    //cout << "  WEIGHTS:  UP: " << upW << " DOWN: " << downW << " LEFT: " << leftW << " RIGHT: " << rightW << endl;

    pair<int, int> toSort[] = {make_pair(upW, UP), make_pair(downW, DOWN), make_pair(leftW, LEFT), make_pair(rightW, RIGHT)};
    sort(toSort, toSort + 4);

    int maxWeightAround = toSort[3].first;
    int ch = 4;
    if (maxWeightAround > 6)
        ch = 1;
    for (int i = 0; i < 4; i++)
    {
        if (toSort[i].first >= 0)
        {
            switch (toSort[i].second)
            {
            case UP:
                //cout << " goint up" << endl;
                go(r - 1, c, DOWN, (from & (LEFT | RIGHT)) > 0, nextDeveCurvare, nextDeveAndareDritto, points, ii + 1, depth + 1); //UP
                break;
            case DOWN:
                //cout << " goint down" << endl;
                go(r + 1, c, UP, (from & (LEFT | RIGHT)) > 0, nextDeveCurvare, nextDeveAndareDritto, points, ii + 1, depth + 1); //DOWN
                break;
            case LEFT:
                //cout << " goint left" << endl;
                go(r, c - 1, RIGHT, (from & (UP | DOWN)) > 0, nextDeveCurvare, nextDeveAndareDritto, points, ii + 1, depth + 1); //LEFT
                break;
            case RIGHT:
                //cout << " goint right" << endl;
                go(r, c + 1, LEFT, (from & (UP | DOWN)) > 0, nextDeveCurvare, nextDeveAndareDritto, points, ii + 1, depth + 1); //RIGHT
                break;

            default:
                break;
            }
        }
        ch--;
        if (ch <= 0)
            break;
    }

    mat[r][c] = me & ~(USED);
}

void newMax(int points, int n)
{
    maxPoint = points;
    cout << "POINTS: " << maxPoint << endl;
    return;
    for (int i = 0; i < n; i++)
    {
        solMat[solR[i]][solC[i]] = i;
        //cout << solR[i] << ":" << solC[i] << " ";
        continue;

        switch (sol[i])
        {
        case UP:
            cout << "U ";
            break;
        case DOWN:
            cout << "D ";
            break;
        case LEFT:
            cout << "L ";
            break;
        case RIGHT:
            cout << "R ";
            break;

        default:
            break;
        }
    }
    cout << "#" << endl;

    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            int n = solMat[r][c];
            if (n < 10 && n >= 0)
                cout << " " << n << " ";
            else
                cout << n << " ";
            solMat[r][c] = 0;
        }
        cout << endl;
    }

    cout << endl;
}

void printWeight()
{
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            int n = weight[r][c];
            if (n < 10 && n >= 0)
                cout << " " << n << " ";
            else
                cout << n << " ";
        }
        cout << endl;
    }
    cout << endl;
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
