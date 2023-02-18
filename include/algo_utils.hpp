#ifndef _ALGORITHM_HPP_
#define _ALGORITHM_HPP_

#include <bits/stdc++.h>

extern void visited_cell_linker(const std::pair<int, int> &cell);
extern void planned_path_linker(const std::vector<std::pair<int, int>> &planned_path);

/*! \struct Sructure to hold the info
 */
struct cell
{
    std::pair<int, int> parent;
    int steps;
    cell() : parent(-1, -1), steps(0) {}
};

template <typename T, typename U, typename V, typename W>
auto operator+(const std::pair<T, U> &l, const std::pair<V, W> &r)
    -> std::pair<decltype(l.first + r.first), decltype(l.second + r.second)>
{
    return {l.first + r.first, l.second + r.second};
}

/**
 * @brief Checks if the cell position is valid
 *
 * @param cell_position Cell position
 * @param rows number of rows
 * @param cols number of columns
 */
bool isValid(const std::pair<int, int> &cell_position, const int rows, const int cols)
{
    return (cell_position.first >= 0) && (cell_position.first < rows) && (cell_position.second >= 0) && (cell_position.second < cols);
}

/**
 * @brief Checks if the cell is blocked or not(i.e obstacle, out of grid)
 *
 * @param grid
 * @param cell_position
 * @param rows
 * @param cols
 * @return true
 * @return false
 */
bool isBlocked(const std::vector<std::vector<int>> &grid, const std::pair<int, int> &cell_position, const int rows, const int cols)
{
    if (isValid(cell_position, rows, cols) && grid[cell_position.first][cell_position.second] == 0)
        return false;

    return true;
}

bool isDestination(const std::pair<int, int> &position, const std::pair<int, int> &destination)
{
    return position == destination;
}

bool check(const std::vector<std::vector<int>> &grid, const std::vector<std::pair<int, int>> &src_vect, const std::vector<std::pair<int, int>> &dest_vect)
{
    // Checks and variable initialization
    int rows = grid.size();
    if (rows == 0)
    {
        std::cerr << "Grid is empty" << std::endl;
        return false;
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
        return false;
    }

    if (isBlocked(grid, src, rows, cols) || isBlocked(grid, dest, rows, cols))
    {
        std::cerr << "Source or destination is blocked" << std::endl;
        return false;
    }
    return true;
}

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

#endif