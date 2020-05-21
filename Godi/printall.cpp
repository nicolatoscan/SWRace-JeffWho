#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

#define RING_WHITE 1
#define RING_BLACK 2

// Temp
int r, c;

// Data
int R, C;
int B, W, A;

// Matrice base con anelli
int **mat;

int main(int argc,  char** argv)
{
    for(int i = 1; i <= 19; i++)
    {
        ifstream f("../input/input" + to_string(i) + ".txt");
        
        f >> R >> C;
        f >> B >> W;
        A = B + W;

        mat = new int *[R];
        for (r = 0; r < R; r++)
        {
            mat[r] = new int[C];
            for (c = 0; c < C; c++)
            {
                mat[r][c] = 0;
            }
        }
        for (int i = 0; i < B; i++) // Black
        {        
            f >> r >> c;

            mat[r][c] = RING_BLACK;
        }
        for (int i = 0; i < W; i++) // White
        {        
            f >> r >> c;

            mat[r][c] = RING_WHITE;
        }
        f.close();

        ofstream out("view" + to_string(i) + ".txt");
        for(r = 0; r < R; r++)
        {
            for(c = 0; c < C; c++)
            {
                if(mat[r][c] == 0)
                {
                        out << "- ";         
                }
                else
                {
                    if(mat[r][c] == RING_WHITE)
                    {
                        out << "O ";
                    }
                    else
                    {
                        out << "+ ";
                    }

                }
            }
            out << endl;
        }
        out.close();
        out << endl;
    }
    cout << "Done!" << endl;

    return 0;
}