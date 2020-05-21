#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

#define RING_WHITE 1
#define RING_BLACK 2

ifstream in("../input/input11.txt");
// ofstream out("output.txt");

// Temp
int r, c;

// Data
int R, C;
int B, W, A;

// Matrice base con anelli
int **mat;

// Righe/Colonne libere
int free_rows_count;
int free_cols_count;
int *free_rows;
int *free_cols;

void load();
void print(void);
void printfree(void);

int main(int argc,  char** argv)
{
    std::cout << std::setprecision(4) << std::fixed;

    load();
    print();
    printfree();


    return 0;
}

void load()
{
    in >> R >> C;
    in >> B >> W;
    A = B + W;

    mat = new int *[R];
    free_cols = new int[C];
    free_rows = new int[R];

    // Inizializza
    for (r = 0; r < R; r++)
    {
        mat[r] = new int[C];
        for (c = 0; c < C; c++)
        {
            mat[r][c] = 0;
        }
        free_rows[r] = 0;
    }
    for (c = 0; c < C; c++)
    {
        free_cols[c] = 0;
    }

    // Carica anelli da file
    for (int i = 0; i < B; i++) // Black
    {        
        in >> r >> c;

        mat[r][c] = 1;

        free_cols[c] = 1;
        free_rows[r] = -1;
    }
    for (int i = 0; i < W; i++) // White
    {        
        in >> r >> c;

        mat[r][c] = 2;
        free_cols[c] = 1;
        free_rows[r] = -1;
    }
}

void print(void)
{
    for(r = 0; r < R; r++)
    {
        for(c = 0; c < C; c++)
        {
            if(mat[r][c] == 0)
            {
                if(free_cols[c] == 0 || 0 == free_rows[r])
                {
                    cout << "\033[31m0 \033[0m";
                }
                else
                {
                    cout << "0 ";
                }                
            }
            else
            {
                if(mat[r][c] == 1)
                {
                    cout << "\033[32m1 \033[0m";
                }
                else
                {
                    cout << "\033[33m2 \033[0m";
                }
                
            }
        }
        cout << endl;
    }
    cout << endl;
}

void printfree(void)
{
    int tot = R * C;
    int rcount = 0;
    int ccount = 0;

    for(c = 0; c < C; c++)
    {
        if(free_cols[c] == 0) ccount++;
    }
    for(r = 0; r < R; r++)
    {
        if(free_rows[r] == 0) rcount++;
    }

    int freecount = rcount*C + ccount*R - rcount*ccount;

    float rap = tot - freecount;

    cout << "# Tot: " << tot << endl;
    cout << "# Free_rows: " << rcount << "  -  Free_cols: " << ccount << endl;
    cout << "# Occupate/Totale = " << rap/tot*100 << " %" << endl;
}
