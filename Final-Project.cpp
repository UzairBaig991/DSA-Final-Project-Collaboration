#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <windows.h>  // For gotoxy function and Sleep
#include <conio.h>    // For _getch() function

using namespace std;

vector<string> mazeLayout = {
    "************************************************",
    "* S  *                                          *",
    "*    *****  *****************                   *",
    "*    *                *  *                      *",
    "* *****  ****  ************ *                   *",
    "*                *                *             *",
    "*    ********   ******   ******* *              *",
    "*    *                *                 *       *",
    "*    *  ********   *******  *** *               *",
    "*    *  *                *                 *    *",
    "*    *  *  *******   *******   ***** *          *",
    "*       *                                  *    *",
    "* *******   *********** **** *                  *",
    "*       *                  *       *            *",
    "*  *****   *******   ****   **** *              *",
    "*  *                *      *           *   *    *",
    "*  *****   *****   *******   ***  *             *",
    "*                *                        *  D  *",
    "************************************************"
};

struct Position {
    int x, y;
};

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int dx[4] = {1, -1, 0, 0};  // Directions for X-axis (right, left, down, up)
int dy[4] = {0, 0, 1, -1};  // Directions for Y-axis (right, left, down, up)

void displayMaze(const vector<string>& maze) {
    system("cls");  // Use "cls" for Windows
    for (const auto& row : maze) {
        cout << row << endl;
    }
}

bool isValidMove(int x, int y, const vector<string>& maze, vector<vector<bool>>& visited) {
    return x >= 0 && y >= 0 && x < maze[0].size() && y < maze.size() && maze[y][x] != '*' && !visited[y][x];
}

bool dfs(vector<string>& maze, Position cur, Position end, vector<vector<bool>>& visited) {
    if (cur.x < 0 || cur.x >= maze[0].size() || cur.y < 0 || cur.y >= maze.size() || 
        maze[cur.y][cur.x] == '*' || visited[cur.y][cur.x]) {
        return false;
    }

    if (cur.x == end.x && cur.y == end.y) {
        maze[cur.y][cur.x] = 'D';  // Mark destination on the path
        return true;
    }

    visited[cur.y][cur.x] = true;
    maze[cur.y][cur.x] = '.';  // Mark the path for visualization

    displayMaze(maze);
    Sleep(50);  // Sleep for 50 milliseconds (adjust as needed)

    for (int i = 0; i < 4; ++i) {
        int nx = cur.x + dx[i];
        int ny = cur.y + dy[i];

        if (dfs(maze, {nx, ny}, end, visited)) {
            return true;
        }
    }

    if (!(cur.x == end.x && cur.y == end.y)) {
        maze[cur.y][cur.x] = ' ';  // Remove from path for backtracking
    }
    return false;
}

bool bfs(vector<string>& maze, Position start, Position end) {
    queue<Position> q;
    vector<vector<bool>> visited(maze.size(), vector<bool>(maze[0].size(), false));
    vector<vector<Position>> parent(maze.size(), vector<Position>(maze[0].size(), {-1, -1}));

    q.push(start);
    visited[start.y][start.x] = true;

    while (!q.empty()) {
        Position cur = q.front();
        q.pop();

        if (cur.x == end.x && cur.y == end.y) {
            // Trace the path from end to start
            while (cur.x != start.x || cur.y != start.y) {
                maze[cur.y][cur.x] = '.';
                cur = parent[cur.y][cur.x];
            }
            maze[start.y][start.x] = 'S';  // Mark the start point
            maze[end.y][end.x] = 'D';  // Mark the end point
            return true;
        }

        for (int i = 0; i < 4; ++i) {
            int nx = cur.x + dx[i];
            int ny = cur.y + dy[i];

            if (isValidMove(nx, ny, maze, visited)) {
                visited[ny][nx] = true;
                parent[ny][nx] = cur;
                q.push({nx, ny});

                // Visual update
                maze[ny][nx] = '.';  // Mark the path being explored
                displayMaze(maze);
                Sleep(50);  // Pause for visual effect, adjust as needed
            }
        }
    }
    return false;
}

void saveRecord(double timeTaken) {
    ofstream file("records.txt", ios::app);
    if (file.is_open()) {
        file << "Manual play time: " << timeTaken << " seconds" << endl;
        file.close();
    }
}

void viewRecords() {
    ifstream file("records.txt");
    string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    } else {
        cout << "No records found." << endl;
    }
}

void manualPlay(vector<string> maze, Position start, Position end) {
    Position cur = start;
    displayMaze(maze);

    vector<vector<bool>> visited(maze.size(), vector<bool>(maze[0].size(), false));
    auto startTime = chrono::high_resolution_clock::now();

    while (true) {
        auto currentTime = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = currentTime - startTime;

        if (elapsed.count() > 120) {  // Check if the elapsed time is greater than 2 minutes (120 seconds)
            cout << "\nYou did not solve the maze within 2 minutes. You lose!\n";
            return;
        }

        gotoxy(cur.x, cur.y);
        
        int key = _getch();
        if (key == 0 || key == 224) {
            key = _getch(); // Get the arrow key code
        }

        Position next = cur;
        if (key == 72) next.y--;        // Up arrow key
        else if (key == 80) next.y++;   // Down arrow key
        else if (key == 75) next.x--;   // Left arrow key
        else if (key == 77) next.x++;   // Right arrow key

        if (isValidMove(next.x, next.y, maze, visited)) {
            if (maze[next.y][next.x] == 'D') {  // Check if the player reaches the destination 'D'
                auto endTime = chrono::high_resolution_clock::now();
                chrono::duration<double> totalElapsed = endTime - startTime;
                cout << "\nYou reached the destination in " << totalElapsed.count() << " seconds! You win!\n";
                saveRecord(totalElapsed.count());
                return;
            }
            
            maze[cur.y][cur.x] = ' ';   // Clear the previous position
            cur = next;
            gotoxy(cur.x, cur.y);
            cout << "S"; // Move to new position
            maze[cur.y][cur.x] = 'S';   // Update the maze with the new position
            displayMaze(maze);          // Show updated maze
        } else {
            cout << "Invalid move!" << endl;
        }
    }
}


int main()
{
    cout << "**************************************************************\n";
    cout << "*                                                             *\n";
    cout << "*                WELCOME TO THE MAZE GAME                     *\n";
    cout << "*                                                             *\n";
    cout << "**************************************************************\n";
    cout << "\n\n";

    while (true)
    {
        string choice;
        cout << "\n\t ______________________________________________\n";
        cout << "\t|       |" << setw(40) << "|\n";
        cout << "\t| [1]   |     Manual Playing" << setw(21) << "|\n";
        cout << "\t| [2]   |     DFS PathFinding" << setw(20) << "|\n";
        cout << "\t| [3]   |     BFS PathFinding" << setw(20) << "|\n";
        cout << "\t| [4]   |     View Previous Record" << setw(15) << "|\n";
        cout << "\t| [0]   |     Exit" << setw(31) << "|\n";
        cout << "\t|_______|______________________________________|\n";
        while (true)
        {
            cout << "\n\tSelect an option: ";
            getline(cin, choice);
            if (choice == "1" || choice == "2" || choice == "3" || choice == "4" || choice == "0")
            {
                break;
            }
            cout << "\n\t\tInvalid input.\n";
        }
        if (choice == "1")
        {
            manualPlay(mazeLayout, {2, 1}, {48, 17});
        }
        else if (choice == "2")
        {
            vector<string> mazeCopy = mazeLayout;
            vector<vector<bool>> visited(mazeLayout.size(), vector<bool>(mazeLayout[0].size(), false));
            if (dfs(mazeCopy, {2, 1}, {48, 17}, visited)) {
                cout << "Path found with DFS!" << endl;
                displayMaze(mazeCopy);
            } else {
                cout << "No solution found with DFS." << endl;
            }
        }
        else if (choice == "3")
        {
            vector<string> mazeCopy = mazeLayout;
            if (bfs(mazeCopy, {2, 1}, {48, 17}))
            {
                displayMaze(mazeCopy);
            }
            else
            {
                cout << "No solution found" << endl;
            }
        }
        else if (choice == "4")
        {
            viewRecords();
        }
        else if (choice == "0")
        {
            cout << "Thanks for playing this game!" << endl;
            break;
        }
    }

    return 0;
}
