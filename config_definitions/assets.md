### Three types of assets can be defined in the assets config file:

- Texture Assets
  - Texture Name        N    std::string (no spaces)
  - Texture FilePath    P    std::string (no spaces)

- Animation Assets
  - Animation Name      N    std::string (no spaces)
  - Texture Name        T    std::string (no spaces, refers to existing texture)
  - Frame Count         F    int         (# of frames in the animation)
  - Animation Speed     S    int         (# of game frames between animation frames)

- Font Assets
  - Font Name           N    std::string (no spaces)
  - Font File Path      P    std::string (no spaces)

- Sound Assets
  - Sound Name          N    std::string (no spaces)
  - Sound File Path     P    std::string (no spaces)
