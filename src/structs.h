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

    Texture() = default;
};

// NOTE: this corresponds to the sprite sheets themselves.
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
struct Growths
{
    int health;
    int strength;
    int magic;
    int speed;
    int skill;
    int luck;
    int defense;
    int resistance;
};

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
    STAT_MAGIC,
    STAT_SPEED,
    STAT_SKILL,
    STAT_LUCK,
    STAT_DEFENSE,
    STAT_RESISTANCE,
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
    bool is_ally;
    int movement;
    int health;
    int max_health;

    int strength;
    int magic;
    int skill;
    int speed;
    int luck;
    int defense;
    int resistance;

    int level;
    Ability ability;

    Growths growths = {};
    int experience = 0;

    Item *primary_item;
    Item *secondary_item;

    int turns_active = -1;
    int xp_value = 0;
    AIBehavior ai_behavior = NO_BEHAVIOR;
    position pos = {0, 0};
    bool is_exhausted = false;
    bool should_die = false;
    position animation_offset = {0, 0};
    bool is_boss = false;

    Buff *buff = nullptr;

    Spritesheet sheet;
    Texture neutral;
    Texture happy;
    Texture angry;
    Texture wince;

    ~Unit()
    {
        delete buff;
        delete primary_item;
        delete secondary_item;
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

    Unit(
         string name_in,
         bool is_ally_in, int movement_in,
         int health_in, int max_health_in,
         int strength_in, int magic_in,
         int speed_in, int skill_in,
         int luck_in,
         int defense_in, int resistance_in,
         int level_in,
         Ability ability_in,
         AIBehavior ai_behavior_in,
         int xp_value_in,

         int health_growth_in,
         int strength_growth_in,
         int magic_growth_in,
         int speed_growth_in,
         int skill_growth_in,
         int luck_growth_in,
         int defense_growth_in,
         int resistance_growth_in,

         ItemType primary_item_type_in,
         ItemType secondary_item_type_in,

         Spritesheet sheet_in,
         Texture neutral_in,
         Texture happy_in,
         Texture angry_in,
         Texture wince_in
         )
    : name(name_in),
      is_ally(is_ally_in),
      movement(movement_in),
      health(health_in),
      max_health(max_health_in),

      strength(strength_in),
      magic(magic_in),
      speed(speed_in),
      skill(skill_in),
      luck(luck_in),
      defense(defense_in),
      resistance(resistance_in),

      level(level_in),
      ability(ability_in),
      ai_behavior(ai_behavior_in),
      xp_value(xp_value_in),
      sheet(sheet_in),
      neutral(neutral_in),
      happy(happy_in),
      angry(angry_in),
      wince(wince_in)
    {
      growths.health = health_growth_in;
      growths.strength = strength_growth_in;
      growths.magic = magic_growth_in;
      growths.speed = speed_growth_in;
      growths.skill = skill_growth_in;
      growths.luck = luck_growth_in;
      growths.defense = defense_growth_in;
      growths.resistance = resistance_growth_in;

      primary_item = GetItem(primary_item_type_in);
      secondary_item = GetItem(secondary_item_type_in);
    }

    Unit(const Unit &other)
    : name(other.name),
      is_ally(other.is_ally),
      movement(other.movement),
      health(other.health),
      max_health(other.max_health),
      strength(other.strength),
      magic(other.magic),
      speed(other.speed),
      skill(other.skill),
      luck(other.luck),
      defense(other.defense),
      resistance(other.resistance),
      level(other.level),
      ability(other.ability),
      ai_behavior(other.ai_behavior),
      xp_value(other.xp_value),
      sheet(other.sheet),
      neutral(other.neutral),
      happy(other.happy),
      angry(other.angry),
      wince(other.wince)
    {
      growths.health = other.growths.health;
      growths.strength = other.growths.strength;
      growths.magic = other.growths.magic;
      growths.speed = other.growths.speed;
      growths.skill = other.growths.skill;
      growths.luck = other.growths.luck;
      growths.defense = other.growths.defense;
      growths.resistance = other.growths.resistance;

      if(other.primary_item)
          primary_item = new Item(*other.primary_item);
      if(other.secondary_item)
          secondary_item = new Item(*other.secondary_item);
    }

    // Switches the primary item with the secondary item
    void
    SwitchItems()
    {
        Item *tmp = primary_item;
        primary_item = secondary_item;
        secondary_item = tmp;
    }

    // Uses the primary item
    void
    Use()
    {
        assert(primary_item);
        if(!primary_item->consumable)
        {
            cout << "WARNING: Item.Use() not right. Item type: " << GetItemString(primary_item->type) << "\n";
            return;
        }

        switch(primary_item->consumable->type)
        {
            case CONS_NOTHING: cout << "WARNING: Item.Use() not right. Item type: " << GetItemString(primary_item->type) << "\n";
            case CONS_POTION: Heal(primary_item->consumable->amount); break;
            case CONS_STATBOOST: cout << "Unimplemented STATBOOST item in Use()\n"; break;
            case CONS_BUFF: cout << "Unimplemented BUFF item in Use()\n"; break;
            default: cout << "UNIMPLEMENTED DEFAULT Item in USE()\n"; break;
        }
        cout << "Used item: " << GetItemString(primary_item->type) << "\n";
        Discard();
    }

    // Deletes the unit's primary item.
    void
    Discard()
    {
        delete primary_item;
        primary_item = nullptr;
    }

    bool
    PrimaryRange(int distance) const
    {
        if(primary_item && primary_item->weapon && 
           primary_item->weapon->min_range <= distance &&
           primary_item->weapon->max_range >= distance)
        {
            return true;
        }
        return false;
    }

    bool
    SecondaryRange(int distance) const
    {
        if(secondary_item && secondary_item->weapon && 
           secondary_item->weapon->min_range <= distance &&
           secondary_item->weapon->max_range >= distance)
        {
            return true;
        }
        return false;
    }

    int
    MinRange() const
    {
        if(primary_item && primary_item->weapon)
            return primary_item->weapon->min_range;
        return 0;
    }
    int
    MaxRange() const
    {
        if(primary_item && primary_item->weapon)
            return primary_item->weapon->max_range;
        return 0;
    }

    int
    OverallMinRange() const
    {
        int result = 0;
        if(primary_item && primary_item->weapon)
        {
            result = primary_item->weapon->min_range;
        }
        if(secondary_item && secondary_item->weapon)
        {
            result = min(result, secondary_item->weapon->min_range);
        }
        return result;
    }

    int
    OverallMaxRange() const
    {
        int result = 0;
        if(primary_item && primary_item->weapon)
        {
            result = primary_item->weapon->max_range;
        }
        if(secondary_item && secondary_item->weapon)
        {
            result = max(result, secondary_item->weapon->max_range);
        }
        return result;
    }

    // Damages a unit and resolves things involved with that process.
    void
    Damage(int amount)
    {
        health = clamp(health - amount, 0, max_health);
    }

    // Heals a unit and resolves things involved with that process.
    void
    Heal(int amount)
    {
        health = clamp(health + amount, 0, max_health);
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

    int
    StatBoost(int growth)
    {
        int result = 0;
        while(growth > 100)
        {
            result += 1;
            growth -= 100;
        }

        if(d100() < growth)
        {
            result += 1;
        }

        return result;
    }

    void
    LevelUp()
    {
        level += 1;
        
        max_health += StatBoost(growths.health);
        strength += StatBoost(growths.strength);
        magic += StatBoost(growths.magic);
        speed += StatBoost(growths.speed);
        skill += StatBoost(growths.skill);
        luck += StatBoost(growths.luck);
        defense += StatBoost(growths.defense);
        resistance += StatBoost(growths.resistance);

        experience -= 100;
        if(level == 10)
            experience = 0;
    }

    void
    GrantExperience(int amount)
    {
        if(level == 10)
            return;

        experience += amount;
        while(experience >= 100) // Just in case you get a ton of experience at once haha
            LevelUp();
    }

    int
    Hit() const
    {
        if(!Armed())
            return 0;
        return primary_item->weapon->hit + 2 * skill;
    }
    // COMMENT

    int
    Avoid() const
    {
        return 2 * AttackSpeed();
    }

    int
    Attack() const
    {
        if(!Armed())
            return 0;
        return primary_item->weapon->might + strength;
    }

    bool
    Armed() const
    {
        return primary_item && primary_item->weapon;
    }

    int
    AttackSpeed() const
    {
        if(Armed())
        {
            return speed - primary_item->weapon->weight; // TODO; + CON!!!
        }
        return speed;
    }

    int
    Crit() const
    {
        return skill * 2;
        // TODO: Make this less for enemies.
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
    position pos = {-1, -1};
    pair<bool, bool> active = {true, false};
    pair<Expression, Expression> expressions = {EXPR_NEUTRAL, EXPR_NEUTRAL};
    int current = 0;
    bool done = false;
    Sound *song = nullptr;
    vector<Sentence> prose;
    Texture words_texture;
    Texture speaker_texture;

    Conversation() = default;

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
    vector<Conversation> villages = {};
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

    // NOTE: For AI decision-making purposes
    vector<position> double_range = {};

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

enum Objective
{
    OBJECTIVE_ROUT,
    OBJECTIVE_CAPTURE,
    OBJECTIVE_BOSS,
};

struct Level
{
    Objective objective;
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

    bool
    IsBossDead()
    {
        for(const shared_ptr<Unit> &unit : combatants)
        {
            if(unit->is_boss)
                return false;
        }
        return true;
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
            {
                tiles.push_back(unit->pos);
            }
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

    int
    GetNumberOf(bool is_ally = true) const
    {
        int result = 0;
        for(shared_ptr<Unit> unit : combatants)
        {
            if(unit->is_ally == is_ally)
                ++result;
        }
        return result;
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
