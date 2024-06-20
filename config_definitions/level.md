### Level File Definition

Each level will be specified by a level file, which will contain 
a list of entity specifications, one per line. It will also contain a single line which 
specifies properties of the player in that level. In this way, you can define an entire level 
in the data file, rather than in programming code. The syntax of the lines are as follows.  

Note:  
    All (GX, GY) positions given in the level specification file are given in 'grid' coordinates. The 'grid' 
    cells are of size 64 by 64 pixels, and the entity should be positioned such that the bottom left corner of its texture is 
    aligned with the bottom left corner of the given grid coordinate. The grid starts at (0, 0) in the bottom 
    left of the screen, and can be seen by pressing the 'G' key while the game is running.  

Tile Entity:
Tile N GX GY SX SY
 - Animation Name    N     std::string (Animation asset name for this tile)
 - GX Grid X Pos     GX    float
 - GY Grid Y Pos     GY    float
 - X Scale           SX    float
 - Y Scale           SY    float

Decoration Entity:
Dec N X Y SX SY
 - Animation Name    N     std::string (Animation asset name for this tile)
 - X Position        X     float
 - Y Position        Y     float
 - X Scale           SX    float
 - Y Scale           SY    float

Player:
Player GX GY CW CH SX SY SM GY B SCX SCY
 - Grid Pos X, Y       GX, GY    float, float (starting position of player)
 - BoundingBox W/H     CW, CH    float, float
 - Left/Right Speed    SX        float
 - Jump Speed          SY        float
 - Max Speed           SM        float
 - Gravity             GY        float
 - Bullet Animation    B         std::string (Animation asset to use for bullets)
 - Bullet Speed        BS        float
 - Bullet Lifespan     BL        float
 - X Scale             SCX       float
 - Y Scale             SCY       float
