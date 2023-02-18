#include "algorithm.hpp"

void tracePath(const std::vector<std::vector<cell>> &cells_detail, const std::pair<int, int> &dest)
{
    std::vector<std::pair<int, int>> path;

    int row, col;
    row = dest.first, col = dest.second;
    auto parent_cell = dest;

    do
    {
        path.emplace(path.begin(), parent_cell);
        parent_cell = cells_detail[row][col].parent;
        row = parent_cell.first;
        col = parent_cell.second;
    } while (cells_detail[row][col].parent != parent_cell);
    path.emplace(path.begin(), parent_cell);

    std::cout << "Path: ";
    for (auto it : path)
    {
        std::cout << "(" << it.first << "," << it.second << ") -> ";
    }
    std::cout << std::endl;
    planned_path_linker(path);
}

void searchAlgo(const std::vector<std::vector<int>> &grid, const std::vector<std::pair<int, int>> &src_vect, const std::vector<std::pair<int, int>> &dest_vect)
{
    // Checks and variable initialization
    int rows = grid.size();
    if (rows == 0)
    {
        std::cerr << "Grid is empty" << std::endl;
        return;
    }
    int cols = grid.at(0).size();
    auto src = src_vect[0];
    auto dest = dest_vect[0];

    std::cout << "(" << src.first << " " << src.second << ")"
              << "; row=" << rows << "; cols=" << cols << std::endl;
    std::cout << "(" << dest.first << " " << dest.second << ")"
              << ";row=" << rows << "; cols=" << cols << std::endl;

    if (!(isValid(src, rows, cols) && isValid(dest, rows, cols)))
    {
        std::cerr << "Source or destination is invalid" << std::endl;
        return;
    }

    if (isBlocked(grid, src, rows, cols) || isBlocked(grid, dest, rows, cols))
    {
        std::cerr << "Source or destination is blocked" << std::endl;
        return;
    }

    // Algorithm starts

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
//     searchAlgo(arr, start, goal);
// }