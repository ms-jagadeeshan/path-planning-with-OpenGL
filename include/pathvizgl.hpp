#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include <GL/glut.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>

#include "debug.hpp"
#include "algo_info.hpp"

// Color codes definition
#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7

enum MenuItem
{
    PLOT_PATH = 1,
    SET_START = 2,
    SET_GOAL = 3,
    SET_OBSTACLE = 4,
    CLEAR_ALL = 5,
    CLEAR_OBSTACLE = 6,
    CLEAR_PATH = 7
};

/*! \struct theme_info
    \brief Theme configuration

    Contains colors for:
      - start
      - goal
      - obstacle
      - non_obstacle
      - background
      - border
      - path
 */
struct theme_info
{
    int start;
    int goal;
    int obstacle;
    int non_obstacle;
    int background;
    int border;
    int visited;
    int path;
};

// #####################################################
// Function and struct related to window(All the functions require globally declared window_info struct)
/**
 * @brief Contains all the data and config about window.
 * Should be declared as global scope
 *
 */
struct window_info
{
    // Window ids
    int window_id;
    int sub_window_id;

    // Window size
    int width = 720;
    int height = 720;

    // Menu id
    int menu_id;

    // Block size in pixels
    int cell_width = 30;
    int cell_height = 30;

    int state = 0;
    std::vector<std::pair<int, int>> start;
    std::vector<std::pair<int, int>> goal;
    std::vector<std::pair<int, int>> obstacle;
    std::vector<std::pair<int, int>> visited;
    std::vector<std::pair<int, int>> path;

    bool multigoal = false;
    bool multistart = false;

    // Speed callback
    std::vector<std::pair<std::string, float>> speeds = {
        {"Fastest", 0},
        {"Fast", 0.01},
        {"Default", 0.03},
        {"Slow", 0.1},
        {"Slowest", 0.5}};
    float speed = speeds[2].second;

    std::vector<std::pair<std::string, struct theme_info>> themes{
        {"Light", {BLUE, RED, BLACK, WHITE, BLACK, BLACK, YELLOW, GREEN}},
        {"Theme1", {MAGENTA, RED, BLACK, BLUE, BLACK, CYAN, YELLOW, GREEN}},
        {"Theme2", {MAGENTA, RED, CYAN, WHITE, BLACK, BLUE, YELLOW, GREEN}},
        {"Theme3", {CYAN, RED, BLUE, WHITE, BLACK, BLACK, YELLOW, GREEN}},
        {"Theme4", {BLUE, RED, BLACK, WHITE, BLACK, CYAN, YELLOW, GREEN}}};

    /*! @brief Must be updated, when theme is changed
     */
    struct theme_info default_theme = themes[0].second;

    /*! @brief Priority of actions
        @sa theme_info
    */
    std::vector<std::pair<int, int>> preferred_action{
        {SET_START, default_theme.start},
        {SET_GOAL, default_theme.goal},
        {SET_OBSTACLE, default_theme.goal}};

    /** @brief Algos is declared in algos.cpp
        @file algos.hpp
    */
    std::vector<algo_info> algos = ::algos;
    int algo_index = -1;
};

/**
 * @brief Draw grid and boundary
 *
 * @param width Width of the window
 * @param height Height of the window
 * @param x_size Width of a single cell
 * @param y_size Height of a single cell
 *
 * @sa window_info::default_theme.background window.default_theme.border
 */
void drawGrid(int width, int height, int x_size, int y_size);

/**
 * @brief Draws the given color in the cell(pixels)
 *
 * @param color Color to be drawn
 * @param x_pix x pixel of left bottom of cell
 * @param y_pix y pixel of left bottom of cell
 */
void drawBlock(int color, int x_pix, int y_pix);

/**
 * @brief Draws the given color to the cell(Position)
 *
 * @param color Color of the cell
 * @param x_pos Position of the cell in number grids from y axis
 * @param y_pos Position of the cell in number grids from x axis
 * @todo Have to validate this function
 */
void drawGridCells(int color, int x_pos, int y_pos);

/**
 * @brief Checks if particular particular action can overwrite the cell
 *
 * @param action Action eg. SET_START, SET_GOAL,...
 * @param old_color Color in the target cell
 * @return If it can overwrite the cell
 * @sa window_info::preferred_action
 */
bool canOverwrite(int action, int old_color);

/**
 * @brief Finds the pixel(tries to find using color) from start,goal,obstacle vector and removes it
 *
 * @param current_pixel Current pixel position
 * @param color Color of the pixel
 */
void find_and_remove(const std::pair<int, int> &current_pixel, const int color);

/**
 * @brief Handles the left click event, adds and removes color of the cell
 *
 * @param x x position of the click
 * @param y y position of the click
 */
void clickHandler(int x, int y);

/**
 * @brief Linker function to pass visited position to gui
 *
 * @param cell Position of the cell which is visited
 */
void visited_cell_linker(std::pair<int, int> &cell);

/**
 * @brief Linker function to pass planned path from algorithm to gui
 *
 * @param planned_path Vector of pair having planned path(start -> goal)
 * @sa cellToPixel drawBlocks
 */
void planned_path_linker(const std::vector<std::pair<int, int>> &planned_path);

/**
 * @brief Clears the positions given in the vector, both in gui and vector
 *
 * @param vect
 * @sa drawBlocks
 */
void clear(std::vector<std::pair<int, int>> &vect);

/**
 * @brief Clears visited and path both in gui and vector
 * @sa clear drawBlocks
 */
void clearPath();

/**
 * @brief Clears all the marked position including start, goal, obstacle, visited, path
 * @sa clear
 */
void clearAll();

/**
 * @brief Display callback for the window
 *
 */
void display_callback(void);

/**
 * @brief Mouse callback for the window
 *
 * @param button Mouse state
 * @param state Whether mouse is pressed or released
 * @param x x position of the click
 * @param y y position of the click
 */
void mouse_callback(int button, int state, int x, int y);

/**
 * @brief Mouse drag callback for the window
 *
 * @param x x position of the mouse pointer while dragging
 * @param y y position of the mouse pointer while dragging
 */
void mouse_drag_callback(int x, int y);

/**
 * @brief Keyboard callback for the window
 *
 * @param key Key pressed
 * @param x x position of the mouse pointer
 * @param y y position of the mouse pointer
 * @todo Standalone keyboard controls has to be implemented
 */
void keyboard_callback(unsigned char key, int x, int y);

/**
 * @brief Menu callback for the main menu
 *
 * @param value
 */
void menu_callback(int value);

/**
 * @brief Algorithm menu callback
 *
 * @param value
 */
void algo_callback(int value);

/**
 * @brief Theme menu callback
 *
 * @param value
 */
void theme_callback(int value);

// ##########################################

/**
 * @brief Initializes glut and creates the window with given info
 *
 * @param title Title of the window
 * @param width Width of the window
 * @param height Height of the window
 * @param x X position of the window
 * @param y Y position of the window
 * @return  Window id of created window.
 */
int initialize(std::string title, int width = 720, int height = 720, int x = 100, int y = 100);

/**
 * @brief Initializes the pixels with the given color
 *
 * @param bg_color Color to be initialized
 */
void init_background(int bg_color);

/**
 * @brief Set the block size of the grid
 *
 * @param window_ Window
 * @param x Width oth the grid
 * @param y
 */
void set_block_size(struct window_info &window_, int x_width, int y_width);

/**
 * @brief Set the Color using glColor3f
 *
 *  |0|1|2|3|4|5|6|7|
 *  |:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
 *  |Black|Red|Green|Yellow|Blue|Magenta|Cyan|White|
 *
 * To get RGB values
 * R = color & 1
 * G = color & 2
 * B = color & 3
 * @param color
 */
void setColor(int color);

/**
 * @brief Draws text in color
 *
 * @param x x position of text
 * @param y y position of text
 * @param bg_color Color of the text
 * @param text Text to be drawn
 */
void drawText(int x, int y, int color, std::string text);

/**
 * @brief Draws a character in color
 *
 * @param color Color of the character
 * @param ch Character to be drawn
 */
void drawText(int color, char ch);

/**
 * @brief Draws text in color with background
 *
 * @param x x position of text
 * @param y y position of text
 * @param bg_color Background color
 * @param fg_color Foreground color(text color)
 * @param x_size Width of a single cell
 * @param y_size Height of a single cell
 * @param text Text to be drawn
 * @sa drawText
 */
void drawLabel(int x, int y, int bg_color, int fg_color, int x_size, int y_size, std::string text);

/**
 * @brief Draws the given color to the given vector of cells
 *
 * @param color Color to be drawn
 * @param vect Vector of pairs which contains x,y pixel of left bottom of cell
 * @param speed Time to sleep in seconds
 * @sa drawBlock
 */
void drawBlocks(int color, const std::vector<std::pair<int, int>> vect, float speed = 0);

/**
 * @brief Finds if a pair exist in vector of pairs and removes it.
 *
 * @param vect Vector for searching
 * @param pa Pair to be searched
 */
void find_and_remove(std::vector<std::pair<int, int>> &vect, const std::pair<int, int> &pa);

/**
 * @brief Add entries to the current menu
 *
 * @param entries Vector pair of entries
 */
void addMenuEntry(const std::vector<std::pair<std::string, int>> &entries);

/**
 * @brief Prints the info of the pixel
 *
 * @param x x pixel
 * @param y y pixel
 */
void printPixelInfo(int x, int y);

/**
 * @brief Sleeps for given seconds
 *
 * @param seconds
 */
void sleep(float seconds);

/**
 * @brief Converts pixel values to grid index
 *
 * @param pixel_vect Vector of pixel pairs
 * @param cell_width Width of a cell
 * @param cell_height Height of a cell
 * @return std::vector<std::pair<int, int>>
 */
std::vector<std::pair<int, int>> pixelToCell(const std::vector<std::pair<int, int>> &pixel_vect, int cell_width, int cell_height);

/**
 * @brief Converts grid index to pixels
 *
 * @param grid_pair Grid pair
 * @param cell_width Width of a cell
 * @param cell_height Height of a cell
 * @return std::pair<int, int>
 */
std::pair<int, int> cellToPixel(const std::pair<int, int> &grid_pair, int cell_width, int cell_height);

/**
 * @brief Converts grid index to pixels
 *
 * @param grid_vect Vector of grid index pairs
 * @param cell_width Width of a cell
 * @param cell_height Height of a cell
 * @return std::vector<std::pair<int, int>>
 */
std::vector<std::pair<int, int>> cellToPixel(const std::vector<std::pair<int, int>> &grid_vect, int cell_width, int cell_height);

#endif