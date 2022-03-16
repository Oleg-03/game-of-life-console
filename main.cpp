#include <iostream>
#include <thread>
#include <string.h>
#include "rlutil.h"

using namespace std;
using namespace rlutil;

// Displays a message about incorrect parameters
void errorArgOutput(int argc, char** argv);

// Check the first parameter entered with the specified
bool validArg(const char* arg, int argc, char** argv);

// A class that is two matrices (cell status and number of neighbors)
class Space final
{
public:
    explicit Space();
    ~Space();

    void update() const;
    void countNeighbors();
    void genesis();
    void changeState(int x, int y);
    bool getState(int x, int y);
    void clear();

private:
    bool** space;
    int** value;
    const int ROWS, COLS;
};

struct Coord
{
    int x, y;
};

int main(int argc, char** argv)
{
    if (argc >= 2)
    {
        // Displays a control hint if the "--help" or "-h" parameter is passed
        if (validArg( "--help", argc, argv) || validArg("-h", argc, argv))
        {
            cout << "[W A S D] - moving" << endl;
            cout << "[P] - clear" << endl;
            cout << "[Space] - revire or kill cell" << endl;
            cout << "[Enter] - start or stop simulation" << endl;
        }
        else
        {
            errorArgOutput(argc, argv);
        }
    }
    else
    {
        // Variable - the simulation is ongoing or paused
        bool evolutionStatus = false;
        Space space;

        // User cursor
        Coord cursor { tcols()/2, trows()/2 };

        // Stream where calculations and simulation output will be performed
        thread floatOfLife
        (
            [&evolutionStatus, &space]
            {
                while (true)
                {
                    while (evolutionStatus)
                    {
                        space.countNeighbors();
                        space.genesis();
                        space.update();

                        msleep(100);
                    }

                    msleep(10);
                }
            }
        );

        while (true)
        {
            // Implementation of cursor control and key functionality
            locate(cursor.x, cursor.y);
            char p = getch();
            switch(p)
            {
                case 'w':
                case 'W':
                    if (cursor.y > 1)
                    {
                        --cursor.y;
                    }
                    break;

                case 's':
                case 'S':
                    if (cursor.y < trows())
                    {
                        ++cursor.y;
                    }
                    break;

                case 'a':
                case 'A':
                    if (cursor.x > 1)
                    {
                        --cursor.x;
                    }
                    break;

                case 'd':
                case 'D':
                    if (cursor.x < tcols())
                    {
                        ++cursor.x;
                    }
                    break;

                case ' ':
                    if (!evolutionStatus)
                    {
                        space.changeState(cursor.x, cursor.y);
                        cout << (space.getState(cursor.x, cursor.y) ? '0' : ' ');
                    }
                    break;

                case '\n':
                    evolutionStatus = !evolutionStatus;
                    if (evolutionStatus)
                    {
                        hidecursor();
                    }
                    else
                    {
                        showcursor();
                    }
                    break;

                case 'p':
                case 'P':
                    if (!evolutionStatus)
                    {
                        space.clear();
                        space.update();
                    }
                    break;
            }

            msleep(10);
        }

        floatOfLife.join();
    }

    return 1;
}

// Is the specified cell alive
bool Space::getState(int x, int y)
{
    return space[--y][--x];
}

// Changing the status of the cell
void Space::changeState(int x, int y)
{
    --x;
    --y;

    space[y][x] = !space[y][x];
}

// Resuscitation and killing of cells according to the number of neighbors
// The name of the method is a reference to "Neon Genesis Evangelion"
void Space::genesis()
{
    for (short r = 0; r < ROWS; ++r)
    {
        for (short c = 0; c < COLS; ++c)
        {
            if (value[r][c] == 3 && !space[r][c])
            {
                space[r][c] = true;
            }
            else if ((value[r][c] > 3 || value[r][c] < 2) && space[r][c])
            {
                space[r][c] = false;
            }
        }
    }
}

// Determining the number of neighbors and writing values to the matrix
void Space::countNeighbors()
{
    for (int r = 0; r < ROWS; ++r)
    {
        for (int c = 0; c < COLS; ++c)
        {
            if ((r-1 > 0 && c-1 > 0) && (r+1 < ROWS && c+1 < COLS))
            {
                int count = 0;

                if (space[r+1][c-1]) ++count;
                if (space[r+1][c  ]) ++count;
                if (space[r+1][c+1]) ++count;
                if (space[r  ][c-1]) ++count;
                if (space[r  ][c+1]) ++count;
                if (space[r-1][c-1]) ++count;
                if (space[r-1][c  ]) ++count;
                if (space[r-1][c+1]) ++count;

                value[r][c] = count;
            }
        }
    }
}

// Removal of living cells
void Space::update() const
{
    for (int r = 0; r < ROWS; ++r)
    {
        for (int c = 0; c < COLS; ++c)
        {
            locate(c+1, r+1);
            cout << ((space[r][c]) ? '0' : ' ');
        }
    }
}

// Zeroing matrices
void Space::clear()
{
    for (short r = 0; r < ROWS; ++r)
    {
        for (short c = 0; c < COLS; ++c)
        {
            space[r][c] = false;
            value[r][c] = 0;
        }
    }
}

Space::Space()
    : ROWS(trows())
    , COLS(tcols())
{
    space = new bool*[ROWS];
    value = new int*[ROWS];

    for (short r = 0; r < ROWS; ++r)
    {
        space[r] = new bool[COLS];
        value[r] = new int[COLS];
    }

    this->clear();
}

Space::~Space()
{
    for (short r = 0; r < ROWS; ++r)
    {
        delete[] space[r];
    }
    delete[] space;

    for (short r = 0; r < ROWS; ++r)
    {
        delete[] value[r];
    }
    delete[] value;
}

bool validArg(const char* arg, int argc, char** argv)
{
    return argc == 2 && !strcmp(argv[1], arg);
}

void errorArgOutput(int argc, char** argv)
{
    cout << '\"';
    for (short i = 1; i < argc; ++i)
    {
        cout << argv[i] << ' ';
    }
    cout << "\b\" is not a correct argument" << endl;
}
