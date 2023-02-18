#include "pathvizgl.hpp"
#include "debug.hpp"

struct window_info window;

template <typename T, typename U, typename V>
auto operator/(const std::pair<T, U> &l, const V &r)
    -> std::pair<decltype(l.first / r), decltype(l.second / r)>
{
    return {l.first / r, l.second / r};
}

int initialize(std::string title, int width, int height, int x, int y)
{
    window.width = width;
    window.height = height;

    // Workaround for not passing argc and argv
    int foo = 1;
    char fo[] = " ";
    char *bar[1] = {fo};
    glutInit(&foo, bar);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(x, y);
    return glutCreateWindow(title.c_str());
}

void init_background(int bg_color)
{
    if (bg_color < 0 || bg_color > 7)
        return;
    float r, g, b;
    r = bg_color & 1;
    g = bg_color & 2;
    b = bg_color & 4;
    glClearColor(r, g, b, 0);
}

void sleep(float seconds)
{
    int time = 1000 * seconds;
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

void set_block_size(struct window_info &window_, int x_width, int y_width)
{
    window_.cell_width = x_width;
    window_.cell_height = y_width;
}

void setColor(int color)
{
    if (color < 0 || color > 7)
        return;
    float r, g, b;
    r = color & 1;
    g = color & 2;
    b = color & 4;
    glColor3f(r, g, b);
}

int getPixelColor(int x, int y)
{
    float color[3];
    glReadPixels(x, window.height - y, 1.0, 1.0, GL_RGB, GL_FLOAT, color);
    return int(color[0] + (color[1] * 2) + (color[2] * 4));
}

void drawLine(int x1, int y1, int x2, int y2)
{
    glLineWidth(1.0);
    glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glEnd();
}

void drawGrid(int width, int height, int x_size, int y_size)
{

    // Grid Boundary
    setColor(window.default_theme.background);
    for (int x = 0; x <= width - x_size; x += x_size)
    {
        glRecti(x, 0, x + x_size, y_size);
        glRecti(x, height - y_size, x + x_size, height);
    }
    for (int y = 0; y <= height - y_size; y += y_size)
    {
        glRecti(0, y, x_size, y + y_size);
        glRecti(width - x_size, y, width, y + y_size);
    }

    // Vertical lines
    setColor(window.default_theme.border);
    for (int x = x_size; x <= width - x_size; x += x_size)
    {
        drawLine(x, y_size, x, height - y_size);
    }
    // Horizontal lines
    for (int y = y_size; y <= height - y_size; y += y_size)
    {
        drawLine(x_size, y, width - x_size, y);
    }
}

void drawText(int x, int y, int color, std::string text)
{
    setColor(color);
    glRasterPos2f(x, y);
    for (auto &ch : text)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ch);
    }
}

void startConsole(int bg_color)
{
    setColor(bg_color);
    glRecti(0, window.cell_height - 3, window.width, 3);

    glRasterPos2f(0, 0.25 * window.cell_height);
}

void drawInConsole(int fg_color, int ch)
{
    setColor(fg_color);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ch);
}

void drawInConsole(int fg_color, std::string text)
{
    setColor(fg_color);
    glRasterPos2f(0, 0.25 * window.cell_height);
    for (auto ch : text)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ch);
}

std::vector<std::string> parseString(std::string str)
{
    std::istringstream ss(str);
    std::vector<std::string> words;
    std::string token;
    while (ss >> token)
    {
        words.push_back(token);
    }
    return words;
}

void drawLabel(int x, int y, int bg_color, int fg_color, int x_size, int y_size, std::string text)
{
    setColor(bg_color);
    glRecti(x, y - 3, x + text.length() * 11, y - y_size + 3);

    drawText(x, y - 0.75 * y_size, fg_color, text);
}

void drawBlock(int color, int x_pix, int y_pix)
{
    setColor(color);
    glRecti(x_pix, y_pix + 1, x_pix + window.cell_width - 1, y_pix + window.cell_height);
    glFlush();
}

void drawBlocks(int color, const std::vector<std::pair<int, int>> vect, float speed)
{
    for (auto itr : vect)
    {
        drawBlock(color, itr.first, itr.second);
        sleep(speed);
    }
}

void drawGridCell(int color, int x_pos, int y_pos)
{
    drawBlock(color, x_pos * window.cell_width, y_pos * window.cell_height);
}

bool canOverwrite(int action, int old_color)
{
    for (auto itr : window.preferred_action)
    {
        if (itr.first == action)
            return true;

        if (itr.second == old_color)
            return false;
    }
    return true;
}

void find_and_remove(std::vector<std::pair<int, int>> &vect, const std::pair<int, int> &pa)
{
    auto is_matching = [&pa](std::pair<int, int> p)
    {
        if (p.first == pa.first)
            if (p.second == pa.second)
                return true;

        return false;
    };
    auto itr = std::find_if(vect.begin(), vect.end(), is_matching);
    if (itr != vect.end())
    {
        vect.erase(itr);
    }
}

void find_and_remove(const std::pair<int, int> &current_pixel, const int color)
{
    std::vector<std::pair<int, int>> *vect_ptr = NULL;

    // Checking if any of the colors same, if yes, then have check in all vector
    std::vector<int> tmp{window.default_theme.start, window.default_theme.goal, window.default_theme.obstacle};
    std::sort(tmp.begin(), tmp.end());
    if (std::unique(tmp.begin(), tmp.end()) == tmp.end())
    {
        if (color == window.default_theme.start)
            vect_ptr = &window.start;
        else if (color == window.default_theme.goal)
            vect_ptr = &window.goal;
        else if (color == window.default_theme.obstacle)
            vect_ptr = &window.obstacle;

        if (vect_ptr == NULL)
            return;

        find_and_remove(*vect_ptr, current_pixel);
    }
    else
    {
        find_and_remove(window.start, current_pixel);
        find_and_remove(window.goal, current_pixel);
        find_and_remove(window.obstacle, current_pixel);
    }
}

void clickHandler(int x, int y)
{
    // If given point is out of bounds, do nothing
    if (x < window.cell_width || x >= window.width - window.cell_width || y <= window.cell_height || y >= window.height - window.cell_height)
    {
        std::cerr << "Error: Given point is out of bounds" << std::endl;
        return;
    }

    int new_color = 0;
    int old_color = getPixelColor(x, y);

    // Check if the old color can be overwritten
    if (!canOverwrite(window.state, old_color))
    {
        std::cerr << "Error: Cannot overwrite this block" << std::endl;
        return;
    }

    // ################
    // +------+
    // |      |
    // |      |
    // +------+
    // Left bottom pixel
    // ################
    int px = (x / window.cell_width) * window.cell_width;
    int py = ((window.height - y) / window.cell_height) * window.cell_width;
    std::pair<int, int> current_pixel = {px, py};

    // Determine the target color and vector based on the current state
    int TARGET_BLOCK_COLOR;
    std::vector<std::pair<int, int>> *vect_ptr;
    bool multiple;

    switch (window.state)
    {
    case MenuItem::SET_START:
        TARGET_BLOCK_COLOR = window.default_theme.start;
        vect_ptr = &window.start;
        multiple = window.multistart;
        break;
    case MenuItem::SET_GOAL:
        TARGET_BLOCK_COLOR = window.default_theme.goal;
        vect_ptr = &window.goal;
        multiple = window.multigoal;
        break;
    case MenuItem::SET_OBSTACLE:
        TARGET_BLOCK_COLOR = window.default_theme.obstacle;
        vect_ptr = &window.obstacle;
        multiple = true;
        break;
    default:
        return;
    }

    // Determine the new color based on the current state and old color
    new_color = old_color == TARGET_BLOCK_COLOR ? window.default_theme.non_obstacle : TARGET_BLOCK_COLOR;

    if (vect_ptr->empty())
    {
        // If the vector is empty, add the current pixel
        vect_ptr->push_back(current_pixel);
    }
    else
    {
        // If multiple is false, clear the vector and draw the non-obstacle color
        if (multiple == false)
        {
            drawBlock(window.default_theme.non_obstacle, vect_ptr->at(0).first, vect_ptr->at(0).second);
            vect_ptr->clear();
        }

        // If the old color is not non-obstacle, remove the pixel from the vector
        if (old_color != window.default_theme.non_obstacle)
            find_and_remove(current_pixel, old_color);

        // If the old color is not the target color, add the current pixel to the vector
        if (old_color != TARGET_BLOCK_COLOR)
            vect_ptr->push_back(current_pixel);
    }

    drawBlock(new_color, px, py);
}

std::vector<std::vector<int>> getGrid()
{
    int rows = window.height / window.cell_height;
    int cols = window.width / window.cell_width;
    std::vector<std::vector<int>> grid(rows, std::vector<int>(cols, 0));

    // Obstacles
    int row, col;
    for (auto it : window.obstacle)
    {
        row = it.second / window.cell_height;
        col = it.first / window.cell_width;
        grid[row][col] = 1;
    }

    // Grid boundary
    for (int i = 0; i < cols; ++i)
    {
        grid[0][i] = 1;
        grid[rows - 1][i] = 1;
    }
    for (int i = 0; i < rows; ++i)
    {
        grid[i][0] = 1;
        grid[i][cols - 1] = 1;
    }
    return grid;
}

void visited_cell_linker(const std::pair<int, int> &cell)
{
    auto pix = cellToPixel(cell, window.cell_width, window.cell_height);
    window.visited.push_back(pix);

    for (auto it : window.start)
    {
        if (it.first == pix.first && it.second == pix.second)
            return;
    }

    drawBlock(window.default_theme.goal, pix.first, pix.second);
    sleep(window.speed);
    drawBlock(window.default_theme.visited, pix.first, pix.second);
}

void planned_path_linker(const std::vector<std::pair<int, int>> &planned_path)
{
    auto planned_path_pixel = cellToPixel(planned_path, window.cell_width, window.cell_height);

    drawBlocks(window.default_theme.path, planned_path_pixel, window.speed / 2);
    drawBlocks(window.default_theme.start, window.start);
    drawBlocks(window.default_theme.goal, window.goal);
}

void display_callback(void)
{
    drawGrid(window.width, window.height, window.cell_width, window.cell_height);
    glFlush();
}

void mouse_callback(int button, int state, int x, int y)
{

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (window.state != MenuItem::PLOT_PATH)
        {
            clickHandler(x, y);
        }
    }
}

void mouse_drag_callback(int x, int y)
{
    static int prev_x = 0, prev_y = 0;

    if (prev_x == x / window.cell_width && prev_y == (y - 1) / window.cell_height)
        return;
    if (((x + 1) % window.cell_width) == 0)
        return;
    if ((y % window.cell_height) == 0)
        return;

    clickHandler(x, y);
    prev_x = x / window.cell_width;
    prev_y = (y - 1) / window.cell_height;
}

void keyboard_callback(unsigned char key, int x, int y)
{

    static int STATE = 0;
    static std::string str = "";
    static std::pair<int, int> pos = {1, 1};
    switch (STATE)
    {
    case 0:
    {
        if (key == 'q')
        {
            exit(0);
        }
        else if (key == ':')
        {
            STATE = 1;
            str.push_back(key);

            startConsole(CYAN);
            drawInConsole(BLUE, str);
        }
        break;
    }
    // Command starting
    case 1:
    {
        // Enter
        if (key == 13)
        {
            auto commands = parseString(str);
            if (commands.size() < 1)
            {
                STATE = 0;
                break;
            }

            if (commands[0] == ":q" || commands[0] == ":quit")
            {
                exit(0);
            }

            if (commands[0] == ":set")
            {
                if (commands.size() < 2)
                {
                    fprintf(stderr, "Command usage: set <type>\n<type> can be start,goal,obstacle\n");
                    STATE = 0;
                    break;
                }

                if (commands[1] == "start")
                    window.state = MenuItem::SET_START;
                else if (commands[1] == "goal")
                    window.state = MenuItem::SET_GOAL;
                else if (commands[1] == "obstacle")
                    window.state = MenuItem::SET_OBSTACLE;

                pos = {1, 1};
                STATE = 10;
                break;
            }

            if (commands[0] == ":clear")
            {
                if (commands.size() < 2)
                {
                    fprintf(stderr, "Command usage: :clear <type>\n<type> can be start,goal,obstacle,path,all\n");
                    STATE = 0;
                    break;
                }

                if (commands[1] == "start")
                    clear(window.start);
                else if (commands[1] == "goal")
                    clear(window.goal);
                else if (commands[1] == "obstacle")
                    clear(window.obstacle);
                else if (commands[1] == "path")
                    clearPath();
                else if (commands[1] == "all")
                    clearAll();
            }

            if (commands[0] == ":speed")
            {
                if (commands.size() < 2)
                {
                    fprintf(stderr, "Command usage: :speed <type>\n<type> can be fastest,fast,default,slow,slowest\n");
                    STATE = 0;
                    break;
                }

                if (commands[1] == "fastest")
                    window.speed = window.speeds[0].second;
                else if (commands[1] == "fast")
                    window.speed = window.speeds[1].second;
                else if (commands[1] == "default")
                    window.speed = window.speeds[2].second;
                else if (commands[1] == "slow")
                    window.speed = window.speeds[3].second;
                else if (commands[1] == "slowest")
                    window.speed = window.speeds[4].second;
            }

            if (commands[0] == ":plot")
            {
                if (commands.size() < 2)
                {
                    menu_callback(MenuItem::PLOT_PATH);
                    STATE = 0;
                }
            }

            /*! @todo command to change theme */
            STATE = 0;
            startConsole(CYAN);
            str.clear();
        }
        // Escape
        else if (key == 27)
        {
            STATE = 0;
            startConsole(CYAN);
            str.clear();
        }
        // Backspace
        else if (key == 8)
        {
            STATE = 1;
            if (str.size() != 0)
                str.pop_back();

            startConsole(CYAN);
            drawInConsole(BLUE, str);
        }
        else
        {
            STATE = 1;
            str.push_back(key);
            drawInConsole(BLUE, key);
        }
        break;
    }
    // Setting
    case 10:
    {
        // Escape
        if (key == 27)
        {
            STATE = 0;
            str.clear();
            break;
        }
        switch (key)
        {
        case GLUT_KEY_DOWN:
        case 's':
        case 'j':
        {
            pos.first++;
            break;
        }
        case GLUT_KEY_UP:
        case 'w':
        case 'k':
        {
            pos.first--;
            break;
        }
        case GLUT_KEY_RIGHT:
        case 'a':
        case 'h':
        {
            pos.second--;
            break;
        }
        case GLUT_KEY_LEFT:
        case 'l':
        {
            pos.second++;
            break;
        }
        };
        auto pix = cellToPixel(pos, window.cell_width, window.cell_height);
        clickHandler(pix.first, pix.second + 1);
    }
    };
    printf("String: %s\nSTATE = %d, key = %d\n", str.c_str(), STATE, key);
    std::cout << std::endl;
    glFlush();
}

std::vector<std::pair<int, int>> pixelToCell(const std::vector<std::pair<int, int>> &pixel_vect, int cell_width, int cell_height)
{
    std::vector<std::pair<int, int>> grid_vect;
    int row, col;

    for (auto it : pixel_vect)
    {
        row = it.second / cell_height;
        col = it.first / cell_width;
        grid_vect.emplace_back(row, col);
    }
    return grid_vect;
}

std::pair<int, int> cellToPixel(const std::pair<int, int> &grid_pair, int cell_width, int cell_height)
{
    int row = grid_pair.second * cell_height;
    int col = grid_pair.first * cell_width;
    return {row, col};
}

std::vector<std::pair<int, int>> cellToPixel(const std::vector<std::pair<int, int>> &grid_vect, int cell_width, int cell_height)
{
    std::vector<std::pair<int, int>> pixel_vect;
    int row, col;
    for (auto it : grid_vect)
    {
        row = it.second * cell_height;
        col = it.first * cell_width;
        pixel_vect.emplace_back(row, col);
    }
    return pixel_vect;
}

void clear(std::vector<std::pair<int, int>> &vect)
{
    drawBlocks(window.default_theme.non_obstacle, vect);
    vect.clear();
}

void clearPath()
{
    clear(window.visited);
    clear(window.path);
    drawBlocks(window.default_theme.start, window.start);
    drawBlocks(window.default_theme.goal, window.goal);
}

void clearAll()
{
    clear(window.start);
    clear(window.goal);
    clear(window.obstacle);
    clear(window.visited);
    clear(window.path);
}

void menu_callback(int value)
{
    if (value == MenuItem::CLEAR_ALL)
    {
        // glClear(GL_COLOR_BUFFER_BIT);
        // display_callback();
        // window.start.clear();
        // window.goal.clear();
        // window.obstacle.clear();
        clearAll();
    }
    else if (value == MenuItem::CLEAR_OBSTACLE)
    {
        clear(window.obstacle);
    }
    else if (value == MenuItem::CLEAR_PATH)
    {
        clearPath();
    }
    else if (value == MenuItem::PLOT_PATH)
    {
        if (window.algo_index == -1)
            if (window.algos.size() != 0)
                window.algo_index = 0;

        // If start is not set
        if (window.start.size() == 0)
        {
            std::string msg = "Set the start";
            fprintf(stderr, "%s\n", msg.c_str());
            drawLabel(window.cell_width, window.height, CYAN, BLUE, window.cell_width, window.cell_height, msg);
            return;
        }

        // If goal is not set
        if (window.goal.size() == 0)
        {
            std::string msg = "Set the goal";
            fprintf(stderr, "%s\n", msg.c_str());
            drawLabel(window.cell_width, window.height, CYAN, BLUE, window.cell_width, window.cell_height, msg);
            return;
        }

        // If previously path is plotted, then clearing all
        clear(window.visited);
        clear(window.path);
        drawBlocks(window.default_theme.start, window.start);
        drawBlocks(window.default_theme.goal, window.goal);

        // print(std::cerr, pixelToCell(window.start, window.cell_width, window.cell_height));
        // std::cout << std::endl;
        // print(std::cerr, pixelToCell(window.goal, window.cell_width, window.cell_height));
        // std::cout << std::endl;

        window.algos.at(window.algo_index)
            .algo_function_ptr(getGrid(), pixelToCell(window.start, window.cell_width, window.cell_height), pixelToCell(window.goal, window.cell_width, window.cell_height));
    }
    window.state = value;
}

void algo_callback(int value)
{
    window.algo_index = value;
}

void speed_callback(int value)
{
    window.speed = window.speeds[value].second;
}

void createMenu(struct window_info &window_)
{
    // Create menu entries for each category
    std::vector<std::pair<std::string, int>> menu_entries = {
        {"Plot Path", MenuItem::PLOT_PATH},
        {"Set Start", MenuItem::SET_START},
        {"Set Goal", MenuItem::SET_GOAL},
        {"Set Obstacle", MenuItem::SET_OBSTACLE},
        {"Clear All", MenuItem::CLEAR_ALL},
        {"Clear Obstacle", MenuItem::CLEAR_OBSTACLE},
        {"Clear Path", MenuItem::CLEAR_PATH}};

    std::vector<std::pair<std::string, int>> themes;
    for (size_t i = 0; i < window_.themes.size(); i++)
        themes.emplace_back(window.themes[i].first, i);
    themes.emplace_back("Custom", window.themes.size());

    std::vector<std::pair<std::string, int>> speeds;
    for (size_t i = 0; i < window_.speeds.size(); i++)
        speeds.emplace_back(window.speeds[i].first, i);

    std::vector<std::pair<std::string, int>> algorithms;
    for (size_t i = 0; i < window_.algos.size(); i++)
        algorithms.emplace_back(window.algos[i].name, i);

    // Create sub-menus for each category
    int algoMenu = glutCreateMenu(algo_callback);
    addMenuEntry(algorithms);

    int themeMenu = glutCreateMenu(theme_callback);
    addMenuEntry(themes);

    int speedMenu = glutCreateMenu(speed_callback);
    addMenuEntry(speeds);

    // Create main menu
    window_.menu_id = glutCreateMenu(menu_callback);
    addMenuEntry(menu_entries);

    // Attach sub-menus to main menu
    glutAddSubMenu("Change Algo", algoMenu);
    glutAddSubMenu("Change Theme", themeMenu);
    glutAddSubMenu("Speed", speedMenu);

    // Attach main menu to right mouse button click
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void theme_callback(int value)
{
    if (value >= window.themes.size())
    {
        fprintf(stderr, "Invalid theme index %d\n", value);
        return;
    }

    const theme_info &selected_theme = window.themes[value].second;
    window.default_theme = selected_theme;

    // Initialize background with the non-obstacle color
    init_background(window.default_theme.non_obstacle);

    // Clear the screen and redraw everything
    glClear(GL_COLOR_BUFFER_BIT);
    display_callback();

    // Set preferred action blocks
    window.preferred_action = {{MenuItem::SET_START, window.default_theme.start},
                               {MenuItem::SET_GOAL, window.default_theme.goal},
                               {MenuItem::SET_OBSTACLE, window.default_theme.goal}};

    // Draw start, goal, and obstacle blocks with the new theme
    drawBlocks(window.default_theme.start, window.start);
    drawBlocks(window.default_theme.goal, window.goal);
    drawBlocks(window.default_theme.obstacle, window.obstacle);
}

void addMenuEntry(const std::vector<std::pair<std::string, int>> &entries)
{
    for (const auto &entry : entries)
    {
        glutAddMenuEntry(entry.first.c_str(), entry.second);
    }
}

// Debugging
void printPixelInfo(int x, int y)
{
    std::cout << "Color: " << getPixelColor(x, y) << " ; x: " << x << " ; y: " << y << std::endl;
}

int main(int argc, char **argv)
{
    // parse_args(argc,argv);

    window.window_id = initialize("Path Planning with Open-GL", 1500, 900);

    // Setting callbacks
    glutDisplayFunc(display_callback);
    glutMouseFunc(mouse_callback);
    glutKeyboardFunc(keyboard_callback);
    glutMotionFunc(mouse_drag_callback);

    createMenu(window);

    init_background(window.default_theme.non_obstacle);
    glOrtho(0, window.width, 0, window.height, -1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glutMainLoop();
}