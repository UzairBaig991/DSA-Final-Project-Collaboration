#include <iostream>
#include <vector>
#include <queue>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <limits>

using namespace std;

const int Width = 70;
const int Height = 20;

struct Position {
    int x, y;
};

vector<string> mazeLayout = {
    "",
    "* S  *                                                              *",
    "*    **************  *********************************************** *",
    "*                *  *                                               *",
    "* ***************  ***********  *********************************** *",
    "*                          *                   *                   *",
    "*    ***********************   ***************   ***************** *",
    "*    *                        *                 *                 *",
    "*    *  ***********************   ********************  ********* *",
    "*    *  *                       *                 *                *",
    "*    *  *  ********************   ******************   *********** *",
    "*       *                                                *         *",
    "* ********************   ******************************* ********* *",
    "*       *                  *                             *         *",
    "*  *************   *********************   **********   ********** *",
    "*  *                            *          *                    *  *",
    "*  **************   **************   *********************   *****  *",
    "*                *                                            *  D  *",
    ""
};

// Utility function to position the cursor
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Utility function to clear the screen
void clearScreen() {
    system("cls");
}

// Utility function to display running dots
void displayDots(int count, int delayMs) {
    for (int i = 0; i < count; ++i) {
        cout << ".";
        Sleep(delayMs); // Pause for the specified milliseconds
    }
}

class Maze {
public:
    vector<string> layout;

    Maze() {
        layout = mazeLayout;
    }

    void drawScreen() const {
        for (int i = 0; i < layout.size(); ++i) {
            gotoxy(0, i);
            cout << layout[i];
        }
    }

    bool isCollision(const Position& pos) const {
        return pos.x < 0 || pos.x >= Width || pos.y < 0 || pos.y >= Height || layout[pos.y][pos.x] == '*';
    }
};

class Player {
public:
    Position pos;
    Position destination;
    string result;

    Player(Position start, Position end) : pos(start), destination(end), result("Not Played") {}

    void drawPlayer() const {
        gotoxy(pos.x, pos.y);
        cout << "@";
        gotoxy(destination.x, destination.y);
        cout << "()";
    }

    bool hasReachedDestination() const {
        return pos.x == destination.x && pos.y == destination.y;
    }

    void move(const Maze& maze, vector<string>& records) {
        while (true) {
            Position newPos = pos;
            if (_kbhit()) {
                char key = _getch();
                if (key == 80) newPos.y++; // Down
                else if (key == 72) newPos.y--; // Up
                else if (key == 77) newPos.x++; // Right
                else if (key == 75) newPos.x--; // Left
            }

            if (!maze.isCollision(newPos)) {
                gotoxy(pos.x, pos.y);
                cout << " ";
                pos = newPos;
                drawPlayer();
            }

            if (hasReachedDestination()) {
                gotoxy(2, Height + 1);
                cout << "Congratulations! You reached the destination!";
                displayDots(3, 500); // Show dots for effect
                result = "Won";
                records.push_back("Manual Play - " + result);
                Sleep(1500); // Delay for feedback
                break;
            }
        }
    }
};

class Solver {
public:
    bool DFS(Position playerPos, Position destPos, const Maze& maze, vector<vector<bool>>& visited, vector<string>& records) {
        if (playerPos.x == destPos.x && playerPos.y == destPos.y) {
            gotoxy(playerPos.x, playerPos.y);
            cout << "@";
            records.push_back("DFS Pathfinding - Won");
            return true;
        }

        visited[playerPos.y][playerPos.x] = true;
        gotoxy(playerPos.x, playerPos.y);
        cout << "."; // Mark path for visualization
        Sleep(50);

        int dx[] = {1, 0, -1, 0};
        int dy[] = {0, 1, 0, -1};

        for (int i = 0; i < 4; i++) {
            Position newPos = {playerPos.x + dx[i], playerPos.y + dy[i]};
            if (!maze.isCollision(newPos) && !visited[newPos.y][newPos.x]) {
                if (DFS(newPos, destPos, maze, visited, records)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool BFS(Position startPos, Position destPos, const Maze& maze, vector<string>& records) {
        queue<pair<Position, vector<Position>>> q;
        vector<vector<bool>> visited(Height, vector<bool>(Width, false));
        q.push({startPos, {startPos}});
        visited[startPos.y][startPos.x] = true;

        int dx[] = {1, 0, -1, 0};
        int dy[] = {0, 1, 0, -1};

        while (!q.empty()) {
            Position curr = q.front().first;
            vector<Position> path = q.front().second;
            q.pop();

            gotoxy(curr.x, curr.y);
            cout << "."; // Mark path
            Sleep(50);

            if (curr.x == destPos.x && curr.y == destPos.y) {
                for (const Position& step : path) {
                    gotoxy(step.x, step.y);
                    cout << "o"; // Highlight the path
                    Sleep(50);
                }
                records.push_back("BFS Pathfinding - Won");
                return true;
            }

            for (int i = 0; i < 4; i++) {
                Position newPos = {curr.x + dx[i], curr.y + dy[i]};
                if (!maze.isCollision(newPos) && !visited[newPos.y][newPos.x]) {
                    visited[newPos.y][newPos.x] = true;
                    vector<Position> newPath = path;
                    newPath.push_back(newPos);
                    q.push({newPos, newPath});
                }
            }
        }

        records.push_back("BFS Pathfinding - Lost");
        return false;
    }
};

class Validation {
public:
    static int getValidChoice(int min, int max) {
        int choice;
        while (true) {
            cin >> choice;
            if (cin.fail() || choice < min || choice > max) {
                cin.clear(); // Clear error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                cout << "Invalid choice. Please enter a number between " << min << " and " << max << ": ";
            } else {
                return choice;
            }
        }
    }
};

void displayWelcome() {
    clearScreen();
    cout << R"(
    **********************************************************************
    *                                                                    *
    *                       WELCOME TO THE MAZE GAME                     *
    *                                                                    *
    **********************************************************************
    )" << endl;

    cout << "\nGet ready to explore the maze!";
    displayDots(3, 500); // Show dots for effect
    cout << "\nPress any key to continue...";
    _getch();
    clearScreen();
}

void displayMenu(vector<string>& records) {
    clearScreen();
    gotoxy(0, 0); // Ensure the menu starts at the top
    cout << "=================================================\n";
    cout << "                MAZE SOLVER GAME                 \n";
    cout << "=================================================\n\n";
    cout << "1. Manual Play                                   \n";
    cout << "2. DFS Pathfinding                               \n";
    cout << "3. BFS Pathfinding                               \n";
    cout << "4. View Previous Records                         \n";
    cout << "5. Exit                                          \n";
    cout << "-------------------------------------------------\n";
    cout << "Loading menu";
    displayDots(3, 500); // Show dots for effect
    cout << "\nSelect an option: ";
}

void displayRecords(const vector<string>& records) {
    clearScreen();
    gotoxy(10, 5);
    cout << "Previous Game Records:";
    int y = 7;
    for (const string& record : records) {
        gotoxy(10, y++);
        cout << record;
    }
    gotoxy(10, y);
    cout << "Press any key to return to the menu.";
    _getch();
}

void setupConsole() {
    system("mode con: lines=30 cols=70");
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

int main() {
    setupConsole();
    vector<string> records;

    Position startPos = {2, 2};
    Position destPos = {66, 19};
    Maze maze;
    Player player(startPos, destPos);
    Solver solver;

    displayWelcome();

    while (true) {
        displayMenu(records);
        int choice = Validation::getValidChoice(1, 5);  // Ensure valid menu choice
        clearScreen();
        maze.drawScreen();
        player.drawPlayer();

        if (choice == 1) {
            player.move(maze, records);
        } else if (choice == 2) {
            vector<vector<bool>> visited(Height, vector<bool>(Width, false));
            if (solver.DFS(player.pos, player.destination, maze, visited, records)) {
                gotoxy(2, Height + 1);
                cout << "DFS Path found to the destination!" << endl;
            } else {
                gotoxy(2, Height + 1);
                cout << "No path found with DFS." << endl;
            }
        } else if (choice == 3) {
            if (solver.BFS(player.pos, player.destination, maze, records)) {
                gotoxy(2, Height + 1);
                cout << "BFS Path found to the destination!" << endl;
            } else {
                gotoxy(2, Height + 1);
                cout << "No path found with BFS." << endl;
            }
        } else if (choice == 4) {
            displayRecords(records);
        } else if (choice == 5) {
            gotoxy(2, Height + 1);
            cout << "Exiting the game... Goodbye!";
            break;
        }

        // Ask if user wants to play again
        gotoxy(2, Height + 2);
        cout << "Do you want to play again? (1 = Yes, 2 = No): ";
        int replayChoice = Validation::getValidChoice(1, 2);
        if (replayChoice == 2) {
            gotoxy(2, Height + 3);
            cout << "Exiting the game... Goodbye!";
            break;
        } else {
            // Reset player position for replay
            player.pos = startPos;
            player.result = "Not Played";
            Sleep(1500); // Delay for feedback before showing the menu again
        }
    }

    return 0;
} 