# 2D-Game-Engine
2D video game engine written in C++

## Clangd LSP instructions
For now, setting up clangd is done by generating the compile_commands.json
file whenever new files are added to src. 
This is done using CMake with: 
```
cmake -S . -G "Unix Makefiles" -B cmake
```
The compile_commands.json file is already symlinked to the root directory,
so all that is requried is running cmake to regenerate it when necessary.

## Example Games
Note: Windows will require additional setup for SFML  

Installation:
  - Checkout the branch for the game you want to play to your local machine.
  - If you are on Linux or MacOS, install the SFML library.
    - If on Linux, no additional build steps should be required.
    - MacOS may require you to edit the build configuration to point to your install location for SFML.
  - Use the command: `make` in the root directory to build and run the game.
    - Optionally you can use `make build` to first build the game and then `make run` to run.
### Geometry Shooter
  - A simple shooter game where the player fires projectiles to destory enemy shapes and score points.
  - Controls:
    - `LMB` will fire a bullet in the direction of the cursor
    - `Space` will fire a special weapon
    - `p` will pause the game
