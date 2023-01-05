// Author: Alex Hartford
// Program: Emblem
// File: Tests
// Date: July 2022

#ifndef STRUCTS_H
#define STRUCTS_H


struct InputState
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool a;
    bool b;
    bool r;

	int joystickCooldown = 0;
};

// ================================= Rendering =================================
struct Texture
{
    SDL_Texture *sdlTexture;
    string filename;
    string dir;
    int width;
    int height;

    Texture(SDL_Texture *sdlTexture_in, string dir_in, string filename_in, int width_in, int height_in)
    {
        this->sdlTexture = sdlTexture_in;
        this->width = width_in;
        this->height = height_in;
        this->dir = dir_in;
        this->filename = filename_in;
    }

    Texture()
    {
        printf("WARN: Default texture constructor called.\n");
    }
};

struct SpriteSheet
{
    Texture texture;
    int size    = SPRITE_SIZE;
    int tracks  = 0;
    int frames  = 0;
    int track   = 0;
    int frame   = 0;
    int speed   = 1; // inverse. 1 is faster than 10.
    int counter = 0;

    SpriteSheet(Texture texture_in, int size_in, int speed_in)
    : texture(texture_in),
      size(size_in),
      speed(speed_in)
    {
        this->tracks = texture_in.height / size_in;
        this->frames = texture_in.width / size_in;
    }

    // called each frame
    void Update()
    {
        counter++;
        if(counter % speed == 0)
        {
            int newFrame = frame + 1;
            if(newFrame >= frames)
            {
                this->frame = 0;
            }
            else
            {
                this->frame = newFrame;
            }
        }
    }

    // switches the sprite to the next animation track
    void ChangeTrack(int track_in)
    {
        assert(track_in < tracks && track_in >= 0);
        this->track = track_in;
        this->frame = 0;
    }
};

// =================================== Gameplay ================================
struct Unit
{
    string name;
    int id;
    bool isAlly;
    int col = 0;
    int row = 0;
    bool isExhausted = false;
    bool shouldDie = false;
    int mov;
    int hp;
    int maxHp;
    int attack;
    int magic;
    int defense;
    int resistance;
    int healing;
    int minRange;
    int maxRange;
    int accuracy;
    int avoid;
    int crit;
    SpriteSheet sheet;
    Texture portrait;

    void Update()
    {
        sheet.Update();
		if(hp <= 0)
		{
			shouldDie = true;
		}
    }

    Unit(string name_in, SpriteSheet sheet_in,
         Texture portrait_in,
         int id_in, bool isAlly_in, int mov_in,
         int hp_in, int maxHp_in,
         int attack_in, int magic_in,
         int defense_in, int resistance_in,
         int accuracy_in, int avoid_in, int crit_in,
         int minRange_in, int maxRange_in)
    : name(name_in),
      sheet(sheet_in),
      portrait(portrait_in),
      id(id_in),
      isAlly(isAlly_in),
      mov(mov_in),
      hp(hp_in),
      maxHp(maxHp_in),
      attack(attack_in),
      magic(magic_in),
      defense(defense_in),
      resistance(resistance_in),
      accuracy(accuracy_in),
      avoid(avoid_in),
      crit(crit_in),
      minRange(minRange_in),
      maxRange(maxRange_in)
    {} // haha c++
    // This little thing is like a vestigial organ
    // disgusting

    // Damages a unit and resolves things involved with that process.
    // Includes a clamp function for less code reuse
    void
    Damage(int dmg)
    {
        hp = clamp(hp - dmg, 0, maxHp);
    }
};


// ========================== map stuff =======================================
enum TileTypes
{
    FLOOR,
    WALL,
    FOREST,
    DESERT,
    OBJECTIVE,
};

struct Tile
{
    int type = 0;
    int penalty = 1;
    int avoid = 0;
    Unit *occupant = nullptr;
    point atlas_index = {0, 16};
};
struct Tilemap
{
    int width;
    int height;
    vector<vector<Tile>> tiles;
    vector<point> accessible;
    vector<point> attackable;
    vector<point> healable;
    //CONSIDER: For trading, talking, playing rock-paper scissors, haha whatever
    //vector<point> adjacent;
    Texture atlas;
    int atlas_tile_size = ATLAS_TILE_SIZE;
};

struct Level
{
    Tilemap map;
    vector<unique_ptr<Unit>> combatants;

    void RemoveDeadUnits()
    {
        vector<pair<int, int>> tiles;

        for(auto const &u : combatants)
        {
            if(u->shouldDie)
            {
                tiles.push_back(pair<int, int>(u->col, u->row)); 
            }
        }

        combatants.erase(remove_if(combatants.begin(), combatants.end(), [](auto const &u) { return u->shouldDie; }), combatants.end());

        for(pair<int, int> tile : tiles)
        {
            map.tiles[tile.first][tile.second].occupant = nullptr;
        }
    }
};

struct Cursor
{
    int col = 1;
    int row = 1;
    Unit *selected = nullptr;
    Unit *targeted = nullptr;
    int selectedCol = -1; // Where the cursor was before placing a unit
    int selectedRow = -1;
    int sourceCol = -1; // Where the cursor was before choosing a target
    int sourceRow = -1;

    SpriteSheet sheet;
    path path_draw = {};

    Cursor(SpriteSheet sheet_in)
    : sheet(sheet_in)
    {}

    void Update()
    {
        sheet.Update();
    }

    // returns the current quadrant of where the cursor is on the screen.
    enum quadrant
    Quadrant() const
    {
        int x = col - viewportCol;
        int y = row - viewportRow;

        if(x > VIEWPORT_WIDTH / 2 && y >= VIEWPORT_HEIGHT / 2)
        {
            return BOTTOM_RIGHT;
        }
        else if(x > VIEWPORT_WIDTH / 2)
        {
            return TOP_RIGHT;
        }
        else if(y >= VIEWPORT_HEIGHT / 2)
        {
            return BOTTOM_LEFT;
        }
        return TOP_LEFT;
    }
};

#endif
