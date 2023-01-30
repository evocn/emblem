// Author: Alex Hartford
// Program: Emblem
// File: Structs

#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>

// =============================== small-time ===================================
struct InputState
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool a;
    bool b;
    bool l;
    bool r;

	int joystickCooldown = 0;
};

// ================================= Rendering =================================
struct Texture
{
    SDL_Texture *sdl_texture;
    string filename;
    string dir;
    int width;
    int height;

    Texture(SDL_Texture *sdl_texture_in, string dir_in, string filename_in, int width_in, int height_in)
    {
        this->sdl_texture = sdl_texture_in;
        this->width = width_in;
        this->height = height_in;
        this->dir = dir_in;
        this->filename = filename_in;
    }

    Texture()
    {
        //printf("WARN: Default texture constructor called.\n");
    }

    ~Texture()
    {
        //TODO: This causes some funny stuff to happen!
        //      But I think it has to be called. Look into this.
        //SDL_DestroyTexture(sdl_texture);
    }
};

enum SheetTrack
{
    TRACK_IDLE,
    TRACK_ACTIVE,
    TRACK_LEFT,
    TRACK_RIGHT,
    TRACK_DOWN,
    TRACK_UP,
};

struct Spritesheet
{
    Texture texture;
    int size    = SPRITE_SIZE;
    int tracks  = 0;
    int frames  = 0;
    SheetTrack track = TRACK_IDLE;
    int frame   = 0;
    int speed   = 1; // inverse. 1 is faster than 10.
    int counter = 0;

    Spritesheet(Texture texture_in, int size_in, int speed_in)
    : texture(texture_in),
      size(size_in),
      speed(speed_in)
    {
        this->tracks = texture_in.height / size_in;
        this->frames = texture_in.width / size_in;
    }

    // called each frame
    void
    Update()
    {
        counter++;
        if(!(counter % speed))
        {
            int new_frame = frame + 1;
            if(new_frame >= frames)
                this->frame = 0;
            else
                this->frame = new_frame;
        }
    }

    // switches the sprite to the next animation track
    void
    ChangeTrack(SheetTrack track_in)
    {
        SDL_assert(track_in < tracks && track_in >= 0);
        this->track = track_in;
        this->frame = 0;
    }

    void
    ChangeTrackMovement(const direction &dir)
    {
        if(dir == position(1, 0))
            ChangeTrack(TRACK_RIGHT);
        else if(dir == position(-1, 0))
            ChangeTrack(TRACK_LEFT);
        else if(dir == position(0, -1))
            ChangeTrack(TRACK_DOWN);
        else if(dir == position(0, 1))
            ChangeTrack(TRACK_UP);
        else
            assert(!"ChangeTrackMovement: Invalid direction.");
    }
};

// =================================== Gameplay ================================
enum Ability
{
    ABILITY_NONE,
    ABILITY_HEAL,
    ABILITY_BUFF,
    ABILITY_SHIELD,
    ABILITY_DANCE,
};

enum Stat
{
    STAT_NONE,
    STAT_ATTACK,
    STAT_DEFENSE,
    STAT_APTITUDE,
    STAT_SPEED,
};

struct Buff
{
    Stat stat = STAT_NONE;
    int amount = 5;
    int turns_remaining = 1;

    Buff(Stat stat_in, int amount_in, int turns_remaining_in)
    : stat(stat_in),
      amount(amount_in),
      turns_remaining(turns_remaining_in)
    {}
};


enum Expression
{
    EXPR_NEUTRAL,
    EXPR_HAPPY,
    EXPR_ANGRY,
    EXPR_WINCE,
};

struct Unit
{
    string name;
    Spritesheet sheet;
    Texture neutral;
    Texture happy;
    Texture angry;
    Texture wince;
    bool is_ally;
    int movement;
    int health;
    int max_health;
    int attack;
    int aptitude;
    int defense;
    int speed;
    int accuracy;
    int avoid;
    int crit;
    int min_range;
    int max_range;
    Ability ability;

    AIBehavior ai_behavior = NO_BEHAVIOR;
    position pos = {0, 0};
    bool is_exhausted = false;
    bool should_die = false;
    position animation_offset = {0, 0};

    Buff *buff = nullptr;

    ~Unit()
    {
        delete buff;
    }

    size_t
    ID()
    {
        return hash<string>{}(name);
    }

    void
    Update()
    {
        sheet.Update();
    }

    Unit(string name_in, Spritesheet sheet_in,
         Texture neutral_in,
         Texture happy_in,
         Texture angry_in,
         Texture wince_in,
         bool is_ally_in, int movement_in,
         int health_in, int max_health_in,
         int attack_in, int aptitude_in, int defense_in,
         int speed_in,
         int accuracy_in, int avoid_in, int crit_in,
         int min_range_in, int max_range_in,
         Ability ability_in,
         AIBehavior ai_behavior_in)
    : name(name_in),
      sheet(sheet_in),
      neutral(neutral_in),
      happy(happy_in),
      angry(angry_in),
      wince(wince_in),
      is_ally(is_ally_in),
      movement(movement_in),
      health(health_in),
      max_health(max_health_in),
      attack(attack_in),
      aptitude(aptitude_in),
      defense(defense_in),
      speed(speed_in),
      accuracy(accuracy_in),
      avoid(avoid_in),
      crit(crit_in),
      min_range(min_range_in),
      max_range(max_range_in),
      ability(ability_in),
      ai_behavior(ai_behavior_in)
    {} // haha c++
    // This little thing is like a vestigial organ
    // disgusting


    // Damages a unit and resolves things involved with that process.
    void
    Damage(int damage)
    {
        health = clamp(health - damage, 0, max_health);
    }

    void
    Deactivate()
    {
        is_exhausted = true;
        sheet.ChangeTrack(TRACK_IDLE);
    }
    void
    Activate()
    {
        is_exhausted = false;
    }
    void
    ApplyBuff(Buff *buff_in)
    {
        buff = buff_in;
    }

    // Called every turn. If buff is over, deletes the buff.
    void
    TickBuff()
    {
        --(buff->turns_remaining);
        if(buff->turns_remaining <= 0)
        {
            delete buff;
            buff = nullptr;
        }
    }
};

Unit *
GetUnitByName(const vector<shared_ptr<Unit>> &units, const string &name)
{
    for(shared_ptr<Unit> unit : units)
    {
        if(unit->ID() == hash<string>{}(name))
        {
            return unit.get();
        }
    }
    cout << "WARN GetUnitByName: No unit of that name.\n";
    return nullptr;
}

// ===================================== Converation ===========================
enum Speaker
{
    SPEAKER_ONE,
    SPEAKER_TWO,
};

enum ConversationEvent
{
    CONV_NONE,
    CONV_ONE_EXITS,
    CONV_TWO_EXITS,
    CONV_ONE_ENTERS,
    CONV_TWO_ENTERS,
};

Expression
GetExpressionFromString(const string &in)
{
    if(in == "Neutral") return EXPR_NEUTRAL;
    else if(in == "Happy") return EXPR_HAPPY;
    else if(in == "Angry") return EXPR_ANGRY;
    else if(in == "Wince") return EXPR_WINCE;
    cout << "Warning: Unsupported Expression in GetExpressionFromString: " << in << "\n";
    return EXPR_NEUTRAL;
}

ConversationEvent
GetConversationEventFromString(const string &in)
{
    if(in == "ONE Exits") return CONV_ONE_EXITS;
    else if(in == "TWO Exits") return CONV_TWO_EXITS;
    else if(in == "ONE Enters") return CONV_ONE_ENTERS;
    else if(in == "TWO Enters") return CONV_TWO_ENTERS;
    SDL_assert(!"Warning: Unsupported Expression in GetConversationEventFromString.");
    return CONV_ONE_EXITS;
}

struct Sentence
{
    Speaker speaker;
    string text;
    Expression expression;
    ConversationEvent event;
};

// CIRCULAR
Texture LoadTextureText(string, SDL_Color, int);

struct Conversation
{
    string filename = "";
    Unit *one = nullptr;
    Unit *two = nullptr;
    pair<bool, bool> active = {true, false};
    pair<Expression, Expression> expressions = {EXPR_NEUTRAL, EXPR_NEUTRAL};
    vector<Sentence> prose;
    int current = 0;
    Texture words_texture;
    Texture speaker_texture;
    bool done = false;
    Sound *song = nullptr;

    Conversation() = default;

    Conversation(Unit *one_in, Unit *two_in)
    : one(one_in),
      two(two_in),
      prose({})
    {
        cout << "CONVERSATION\n";
    }

    string
    Words() const
    {
        return prose[current].text;
    }

    Speaker
    Speaker() const
    {
        return prose[current].speaker;
    }

    Expression
    Expression() const
    {
        return prose[current].expression;
    }

    void
    ReloadTextures()
    {
        words_texture = LoadTextureText(Words(), black, CONVERSATION_WRAP);
        if(Speaker() == SPEAKER_ONE)
            speaker_texture = LoadTextureText(one->name, black, 0);
        if(Speaker() == SPEAKER_TWO)
            speaker_texture = LoadTextureText(two->name, black, 0);
    }

    void
    Next()
    {
        ++current;
        if(current >= prose.size())
        {
            done = true;
            current = 0;
            return;
        }

        if(Speaker() == SPEAKER_ONE)
        {
            expressions.first = Expression();
        }
        else if(Speaker() == SPEAKER_TWO)
        {
            expressions.second = Expression();
        }

        switch(prose[current].event)
        {
            case CONV_NONE:
            {
            } break;
            case CONV_ONE_EXITS: 
            {
                active.first = false;
            } break;
            case CONV_TWO_EXITS: 
            {
                active.second = false;
            } break;;
            case CONV_ONE_ENTERS: 
            {
                active.first = true;
            } break;;
            case CONV_TWO_ENTERS:
            {
                active.second = true;
            } break;;
            default: SDL_assert(!"ERROR Unhandled enum in Conversation.Next()");
        }
    }
};

struct ConversationList
{
    vector<Conversation> list = {};
    int index = 0;
    vector<Conversation> mid_battle = {};
    Conversation *current = nullptr;
    Conversation prelude;
};



// ========================== map stuff =======================================
struct Tile
{
    TileType type = FLOOR;
    int penalty = 1;
    int avoid = 0;
    int defense = 0;
    Unit *occupant = nullptr;
    position atlas_index = {0, 16};
};
struct Tilemap
{
    int width;
    int height;
    vector<vector<Tile>> tiles = {};
    vector<position> accessible = {};
    vector<position> attackable = {};
    vector<position> ability = {};
    vector<position> range = {};
    vector<position> adjacent = {};
    vector<position> vis_range = {};
    Texture atlas;
    int atlas_tile_size = ATLAS_TILE_SIZE;

    position
    GetNextSpawnLocation()
    {
        for(int col = 0; col < width; ++col)
        {
            for(int row = 0; row < height; ++row)
            {
                if(tiles[col][row].type == SPAWN &&
                   !tiles[col][row].occupant)
                {
                    return position(col, row);
                }
            }
        }
        SDL_assert(!"ERROR GetNextSpawnLocation: No spawn locations available.");
        return position(0, 0);
    }
};

struct Level
{
    Tilemap map;
    vector<shared_ptr<Unit>> combatants;
    Sound *song = nullptr;
    ConversationList conversations;
    string name = "";

    // Puts a piece on the board
    void
    AddCombatant(shared_ptr<Unit> newcomer, const position &pos)
    {
        newcomer->pos = pos;
        combatants.push_back(newcomer);
        SDL_assert(!map.tiles[pos.col][pos.row].occupant);
        map.tiles[pos.col][pos.row].occupant = newcomer.get();
    }

    // Scans the whole board to find the next available spawn point
    void
    GetNextSpawnLocation()
    {
    }

    // Returns the position of the leader.
    position
    Leader()
    {
        for(const shared_ptr<Unit> &unit : combatants)
        {
            if(unit->ID() == LEADER_ID)
                return unit->pos;
        }
        SDL_assert(!"ERROR Level.Leader(): No leader!\n");
        return position(0, 0);
    }

    void
    RemoveDeadUnits()
    {
        if(GlobalInterfaceState != GAME_OVER)
        {
            position leader_pos = Leader();
            // Quit if Leader is dead
            if(map.tiles[leader_pos.col][leader_pos.row].occupant->should_die)
            {
                GlobalInterfaceState = GAME_OVER;
                return;
            }
        }

        // REFACTOR: This could be so much simpler.
        vector<position> tiles;
        for(const shared_ptr<Unit> &unit : combatants)
        {
            if(unit->should_die)
                tiles.push_back(unit->pos);
        }

        combatants.erase(remove_if(combatants.begin(), combatants.end(),
                    [](auto const &u) { return u->should_die; }),
                    combatants.end());

        for(position tile : tiles)
            map.tiles[tile.col][tile.row].occupant = nullptr;
    }

    // A mutation function that just checks if there are any units left to
    // move, and ends the player's turn if there aren't.
    void
    CheckForRemaining()
    {
        if(GlobalInterfaceState == NEUTRAL_OVER_DEACTIVATED_UNIT)
        {
            for(auto const &u : combatants)
            {
                if(u->is_ally && !u->is_exhausted)
                    return;
            }

            // End player turn
            GlobalInterfaceState = NO_OP;
            GlobalPlayerTurn = false;
            GlobalTurnStart = true;
        }
    }

    void
    Update()
    {
        // cleanup functions
        RemoveDeadUnits();
        CheckForRemaining();
    }
};

// ================================= Menu ======================================
struct Menu
{
    int rows = 0;
    int current = 0;

    vector<Texture> optionTextTextures;
    vector<string> optionText;

    Menu(vector<string> options_in)
    {
        for(string s : options_in)
        {
            optionTextTextures.push_back(LoadTextureText(s.c_str(), uiTextColor, 0));
            optionText.push_back(s);
            rows += 1;
        }
    }

    // Custom-build a menu based on your current options.
    void
    AddOption(string s)
    {
        rows += 1;
        optionTextTextures.push_back(LoadTextureText(s.c_str(), uiTextColor, 0));
        optionText.push_back(s);
    }
};

#endif
