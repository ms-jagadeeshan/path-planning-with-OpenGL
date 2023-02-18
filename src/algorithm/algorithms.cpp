#include "algo_utils.hpp"

// Name: Breadth First Search
// Multiple source : false
// Multiple destination : false
// Completeness : true
void breadthFirstSearch(const std::vector<std::vector<int>> &grid, const std::vector<std::pair<int, int>> &src_vect, const std::vector<std::pair<int, int>> &dest_vect)
{
    if (!check(grid, src_vect, dest_vect))
        return;

    int rows = grid.size();
    int cols = grid.at(0).size();
    auto src = src_vect[0];
    auto dest = dest_vect[0];

    // Algorithm starts
    // Creating bool 2D array for storing if visited
    bool visited[rows][cols];
    memset(visited, false, sizeof(visited));

    // Offset which nearby cell differs
    int offset[][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    std::vector<std::vector<cell>> cells_detail(rows, std::vector<cell>(cols));
    std::queue<std::pair<int, int>> Queue;

    cells_detail[src.first][src.second].parent = src;
    Queue.push(src);

    std::pair<int, int> cur; /*! current cell*/
    while (!Queue.empty())
    {
        cur = Queue.front();
        visited[cur.first][cur.second] = true;
        Queue.pop();

        // ##########################
        visited_cell_linker(cur);
        // ##########################
        for (int i = 0; i < 4; i++)
        {
            std::pair<int, int> neighbour = cur + std::make_pair(offset[i][0], offset[i][1]);

            if (!(visited[neighbour.first][neighbour.second] || isBlocked(grid, neighbour, rows, cols)))
            {
                if (isDestination(neighbour, dest))
                {
                    cells_detail[neighbour.first][neighbour.second].parent = cur;
                    tracePath(cells_detail, dest);
                    return;
                }
                visited[neighbour.first][neighbour.second] = true;
                cells_detail[neighbour.first][neighbour.second].steps = cells_detail[cur.first][cur.second].steps + 1;
                cells_detail[neighbour.first][neighbour.second].parent = cur;
                Queue.push(neighbour);
            }
        }
    }
}

// Name: Depth First Search
// Multiple source : false
// Multiple destination : false
// Completeness : false
void depthFirstSearch(const std::vector<std::vector<int>> &grid, const std::vector<std::pair<int, int>> &src_vect, const std::vector<std::pair<int, int>> &dest_vect)
{
    if (!check(grid, src_vect, dest_vect))
        return;

    int rows = grid.size();
    int cols = grid.at(0).size();
    auto src = src_vect[0];
    auto dest = dest_vect[0];

    // Algorithm starts
    // Creating bool 2D array for storing if visited
    bool visited[rows][cols];
    memset(visited, false, sizeof(visited));

    // Offset which nearby cell differs
    int offset[][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    std::vector<std::vector<cell>> cells_detail(rows, std::vector<cell>(cols));
    std::stack<std::pair<int, int>> Stack;
    cells_detail[src.first][src.second].parent = src;
    Stack.push(src);

    std::pair<int, int> cur; /*! current cell*/
    while (!Stack.empty())
    {
        cur = Stack.top();
        visited[cur.first][cur.second] = true;
        Stack.pop();

        // ##########################
        visited_cell_linker(cur);
        // ##########################
        for (int i = 0; i < 4; i++)
        {
            std::pair<int, int> neighbour = cur + std::make_pair(offset[i][0], offset[i][1]);

            if (visited[neighbour.first][neighbour.second] || isBlocked(grid, neighbour, rows, cols))
                continue;

            if (isDestination(neighbour, dest))
            {
                cells_detail[neighbour.first][neighbour.second].parent = cur;
                tracePath(cells_detail, dest);
                return;
            }
            visited[neighbour.first][neighbour.second] = true;
            cells_detail[neighbour.first][neighbour.second].steps = cells_detail[cur.first][cur.second].steps + 1;
            cells_detail[neighbour.first][neighbour.second].parent = cur;
            Stack.push(neighbour);
        }
    }
}

// int main()
// {
//     std::vector<std::vector<int>> arr{
//         {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
//         {1, 0, 1, 1, 1, 1, 0, 0, 0, 1},
//         {1, 0, 1, 0, 1, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
//         {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
//         {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
//     };

//     std::pair<int, int> start = {6, 3};
//     std::pair<int, int> goal = {6, 7};
//     breadthFirstSearch(arr, start, goal);
// }