# Path planning with OpenGL
 GUI application written in C++ with OpenGL framework to visualize path planning algorithms

## Dependencies
- OpenGL Utility Toolkit (GLUT) library
- OpenGL Extension Wrangler Library (GLEW)
  
## Installation

### Ubuntu
```
sudo apt-get install freeglut3-dev libglew-dev
git clone https://github.com/ms-jagadeeshan/path-planning-with-OpenGL
cd path-planning-with-OpenGL
make
```

Now, the binary named `pathvizgl` will be there in the current directory

Run `./pathvizgl`


## Usage

I don't use mouse much, so i created vim like command console

1. GUI mode
   Pressing `q` will quit the program  
   Right click the screen to get the menu
    - Plot Path
    - Set Start
    - Set Goal
    - Set Obstacle
    - Clear All
    - Clear Obstacle
    - Clear Path
    - Algorithm
    - Theme
    - Speed
  

2. Keyboard mode
    - ESC - to reset the command console
    - :q or :quit - quit the program
    - :set start|goal|obstacle
    - :clear all|start|goal|obstacle|path
    - :speed fastest|fast|default|slow|slowest
    - :plot
    - wasd or hjkl - movement
    - space


## Contributing
To add new algorithms, add your algo in `src/algorithm/algorithms.cpp` and `src/algorithm/algorithms_info.json`
