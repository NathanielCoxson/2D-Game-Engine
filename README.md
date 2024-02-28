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
