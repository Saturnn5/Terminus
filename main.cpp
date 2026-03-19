#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <__stddef_offsetof.h>

enum Difficulty {
    EASY = 0b00,
    NORMAL = 0b01,
    CHALLENGE = 0b10,
    NIGHTMARE = 0b11,
};

#define NAME_LEN 10
#define MAX_NEG_STAT (-1)
// battle statuses
#define DEFENSE_STANCE (1 << 0) // 20% chance to block incoming damage
#define ATTACK_STANCE (1 << 1)  // 20% increased damage
#define MAGIC_STANCE (1 << 2)   // 20% reduced cooldowns for magic spells
#define SPEED_STANCE (1 << 3)   // you can act three tick faster
#define POISONED (1 << 4)       // you take 2% of your max hp each time you act
#define BURNING (1 << 5)        // you take 5% of your enemies atk each time you act
#define WEBBED (1 << 6)         // your opponent acts each time you act
#define SLIMY (1 << 7)          // you act 10 ticks slower
// status flags
#define GUILD_MEMBER (1 << 2)
#define BEGINNER_WILDS_EXPERT (1 << 3)

typedef enum {
    ITEM_MISC,
    ITEM_CONSUMABLE,
    ITEM_EQUIPMENT,
    ITEM_QUEST
} ItemType;

typedef enum {
    SLOT_HELMET,
    SLOT_CHEST,
    SLOT_CHARM,
    SLOT_WEAPON
} SlotType;

typedef struct {
    int vitality;
    int strength;
    int wisdom;
    int dexterity;
} ItemStatModifier;

typedef struct {
    ItemStatModifier stat_bonus;
    SlotType slot;
} EquipmentItemData;

typedef struct {
    int quest_id;
} QuestItemData;

typedef struct {
    int id;
    const char *name;
    const char *description;
    ItemType item_type;
    union {
        EquipmentItemData equipment_item_data;
        QuestItemData quest_item_data;
    } data;
} Item;

#define ITEM_NONE (-1)
enum {
    ITEM_GUILD_BADGE,
    ITEM_RUSTY_KNIFE,
    ITEM_BEGINNER_HERB
};

Item items[] = {
    {
        ITEM_GUILD_BADGE,
        "Guild badge",
        "A badge denoting you as a member of the adventurer's guild",
        ITEM_QUEST
    },
    {
        ITEM_RUSTY_KNIFE,
        "Rusty knife",
        "A rusty knife, good enough for some beginner enemies",
        ITEM_EQUIPMENT
    },
    {
        ITEM_BEGINNER_HERB,
        "Beginner herb",
        "An herb found around the town of beginnings, restores a small amount of health when used",
        ITEM_CONSUMABLE
    }
};

typedef struct {
    int item_id;
    int quantity;
} InventorySlot;

#define INVENTORY_START_SIZE 4
#define INVENTORY_MAX_SIZE 32

typedef struct {
    InventorySlot inventory[INVENTORY_MAX_SIZE];
    int num_items;
    int curr_max_size;
} Inventory;

typedef struct {
    char name[NAME_LEN];

    // stats
    int vitality;
    int strength;
    int wisdom;
    int dexterity;

    // resources
    int level;
    int max_hitpoints;
    int current_hitpoints;

    // battle statuses
    unsigned int battle_status_effects;
} Fighter;

struct Player;

#define MAX_QUESTS 32

typedef enum {
    OBJ_GATHER_ITEM,
    OBJ_VISIT_AREA,
    OBJ_MEET_NPC,
    OBJ_REACH_LEVEL,
    OBJ_CUSTOM
} ObjectiveType;

typedef int (*CheckCompletion)(struct Objective *obj, Player *p);

typedef struct Objective {
    ObjectiveType type;
    int target_id;   // item ID, area ID, NPC ID, or level
    int quantity;
    int progress;    // track completion
    CheckCompletion completion_func;
} Objective;

int check_gather_item(Objective *obj, Player *p);
int check_visit_area(Objective *obj, Player *p);
int check_visit_npc(Objective *obj, Player *p);
int check_reach_level(Objective *obj, Player *p);

typedef struct {
    int coins;
    int experience;
    int item_id;     // use ITEM_NONE if no item reward
    int item_quantity;
} QuestReward;

#define MAX_OBJECTIVES 4

typedef struct {
    int quest_id;
    const char *name;
    const char *description;
    int num_objectives;
    Objective objectives[MAX_OBJECTIVES];
    QuestReward reward;
    int completed;
} Quest;

typedef struct Player {
    Fighter fighter;

    // stats
    int unused_stat_points;

    // resources
    int coins;
    int experience;
    int reputation;
    int guild_reputation;
    Inventory inventory;
    int num_quests;
    Quest active_quests[MAX_QUESTS];


    // statuses
    int position_id;
    unsigned int npc_flags;     // stores which NPCs the player has met
    unsigned int status_flags;
} Player;

// Used to denote the data type of stat in the stat table
typedef enum {
    STAT_INT,
    STAT_STRING
} StatType;

// Used to store and lookup stats from a fighter
typedef struct {
    char key;
    const char *name;
    size_t offset;
    StatType type;
} StatEntry;

StatEntry fighter_stat_table[] = {
    {'n', "Name", offsetof(Fighter, name), STAT_STRING},
    {'v', "Vitality", offsetof(Fighter, vitality), STAT_INT},
    {'s', "Strength", offsetof(Fighter, strength), STAT_INT},
    {'w', "Wisdom", offsetof(Fighter, wisdom), STAT_INT},
    {'d', "Dexterity", offsetof(Fighter, dexterity), STAT_INT},
    {'l', "Level", offsetof(Fighter, level), STAT_INT},
    {'m', "Max HP", offsetof(Fighter, max_hitpoints), STAT_INT},
    {'h', "HP", offsetof(Fighter, max_hitpoints), STAT_INT},
    {'e', "Effects", offsetof(Fighter, battle_status_effects), STAT_INT}
};

#define STAT_COUNT (sizeof(fighter_stat_table)/sizeof(StatEntry))
#define LEVELING_STAT_COUNT 5

typedef void (*action_type)(Player *, void *);

typedef struct {
    const char *name;
    action_type action_func;
} Action;

typedef struct {
    const char *name;
    const char *description;
    int *connections;
    Action *region_actions;
} Region;

void region_menu(Player *);

void go_to(Player *, int id);

void admire(Player *, Region *);

void check_quest_board(Player *, Region *);

void talk_to_receptionist(Player *, Region *);

void talk_to_bartender(Player *, Region *);

void explore(Player *, Region *);

void forage(Player *, Region *);

void fish(Player *, Region *);

void battle(Player *, Region *);

void menu_menu(Player *, Region *);

void become_beginner_wilds_expert(Player *, const char *);

#define NUM_REGIONS 5
Region world[NUM_REGIONS] = {
    {
        "Town of Beginnings",
        "You are in your home town, the Town of Beginnings",
        (int[]){
            1, 4, -1
        },
        (Action[]){
            {"Look around", (action_type) admire},
            {"Open menu", (action_type) menu_menu},
            {nullptr, nullptr}
        }
    },
    {
        "Town of Beginnings Guild Hall",
        "You are in the guild hall of your home town",
        (int[]){
            0, 2, 3, -1
        },
        (Action[]){
            {"Look around", (action_type) admire},
            {"Open menu", (action_type) menu_menu},
            {nullptr, nullptr}
        }
    },
    {
        "Town of Beginnings Guild Hall Desk",
        "You are speaking to the guild receptionist",
        (int[]){
            1, -1
        },
        (Action[]){
            {"Talk to guild receptionist", (action_type) talk_to_receptionist},
            {"Open menu", (action_type) menu_menu},
            {nullptr, nullptr}
        }
    },
    {
        "Town of Beginnings Guild Hall Bar",
        "You're at the guild's bar",
        (int[]){
            1, -1
        },
        (Action[]){
            {"Talk to bartender", (action_type) talk_to_bartender},
            {"Open menu", (action_type) menu_menu},
            {nullptr, nullptr}
        }
    },
    {
        "Beginner Wilds",
        "The unsafe region outside the Town of Beginnings",
        (int[]){
            0, -1
        },
        (Action[]){
            {"Explore the wilds", (action_type) explore},
            {"Open menu", (action_type) menu_menu},
            {nullptr, nullptr}
        }
    }
};

const char *difficulty_names[4] = {"Easy", "Normal", "Challenge", "Nightmare"};
const float difficulty_nums[4] = {0.8, 1.0, 1.2, 1.5};

void print_header() {
    printf("******************************\n");
}

void print(Player *p, int print_full_list) {
    if (print_full_list) printf("%s: %s\n", fighter_stat_table[0].name, *((char **) &p->fighter + fighter_stat_table[0].offset));
    for (int i = 0; i < (print_full_list ? STAT_COUNT : LEVELING_STAT_COUNT); i++) {
        int *stat_ptr = (int *) ((char *) &p->fighter + fighter_stat_table[i].offset);
        printf("%s: %d\n", fighter_stat_table[i].name, *stat_ptr);
    }
    if (!print_full_list) return;
    printf("%s has %d coins.\n", p->fighter.name, p->coins);
}

void print_fighter_stats(Player *p, int print_full_list) {
    for (int i = print_full_list ? 0 : 1; i < (print_full_list ? STAT_COUNT : LEVELING_STAT_COUNT); i++) {
        char *base = (char *) &p->fighter;
        char *ptr = base + fighter_stat_table[i].offset;
        switch (fighter_stat_table[i].type) {
            case STAT_INT:
                if (fighter_stat_table[i].key == 'm') {
                    // Should print HP as curr/max and properly increment i
                    printf("%s: %d/%d\n", fighter_stat_table[++i].name, *(int *) ptr, *(int *)(base + fighter_stat_table[i].offset));
                    break;
                }
                printf("%s: %d\n", fighter_stat_table[i].name, *(int *) ptr);
                break;
            case STAT_STRING: printf("%s: %s\n", fighter_stat_table[i].name, (char *) ptr);
                break;
        }
    }
}

void print_inv_change_dialogue(Player *p, int item_id, int quant, int gain) {
    Item item = items[item_id];
    const char *text = gain ? "got" : "lost";

    if (quant == 1) {
        printf("%s %s the %s!\n", p->fighter.name, text, item.name);
        return;
    }
    printf("%s %s x%d %s!\n", p->fighter.name, text, quant, item.name);
}

int add_item(Inventory *inv, int item_id, int quant) {
    // If the player is already carrying an item of that type, add to the stack
    for (int i = 0; i < inv->num_items; i++) {
        if (inv->inventory[i].item_id == item_id) {
            inv->inventory[i].quantity += quant;
            return 1;
        }
    }
    if (inv->num_items < inv->curr_max_size) {
        // If the player's inventory does have an open slot, add the item
        inv->inventory[inv->num_items++] = (InventorySlot){item_id, quant};
        return 1;
    }
    // If the player's inventory has no open slots, return
    printf("Inventory full!\n");
    return 0;
}

int remove_item(Inventory *inv, int item_id, int quant) {
    for (int i = 0; i < inv->num_items; i++) {
        if (inv->inventory[i].item_id == item_id) {
            inv->inventory[i].quantity -= quant;
            // If item has 0 or less quantity, shift every item proceeding it left to remove it
            if (inv->inventory[i].quantity <= 0) {
                for (int j = i; j < inv->num_items - 1; j++) {
                    if (j == INVENTORY_MAX_SIZE - 1) {
                        // If the last slot needs to be emptied, put an ITEM_NONE, 0 in it
                        inv->inventory[j] = (InventorySlot){ITEM_NONE, 0};
                    } else {
                        // Copy item from the right
                        inv->inventory[j] = inv->inventory[j + 1];
                    }
                }
            }
            return 1;
        }
    }
    return 0;
}

int change_player_inventory(Player *p, int item_id, int quant) {
    if (quant == 0) return 0;
    // Check if item is being added or removed
    int adding_to_inventory = quant > 0 ? 1 : 0;
    // Based on addition or removal, attempt inventory change and store result
    int result = adding_to_inventory ? add_item(&p->inventory, item_id, quant) : remove_item(&p->inventory, item_id, quant);
    // If inventory change is successful, print the change
    if (result) print_inv_change_dialogue(p, item_id, quant, adding_to_inventory);
    return result;
}

int has_item(Inventory *inv, int item_id, int quant) {
    for (int i = 0; i < inv->num_items; i++) {
        if (inv->inventory[i].item_id == item_id && inv->inventory[i].quantity >= quant) {
            return 1;
        }
    }
    return 0;
}

void print_inventory(Inventory *inv, int print_full_list) {
    printf("Inventory:\n");
    if (inv->num_items <= 0) {
        printf("Empty...\n");
        return;
    }
    for (int i = 0; i < inv->num_items; i++) {
        Item item = items[inv->inventory[i].item_id];
        // Always print 'name xQuantity'
        printf("%s x%d", item.name, inv->inventory[i].quantity);
        // If detailed print, then add ' - 'description'' and a newline
        if (print_full_list) {
            printf(" - '%s'\n", item.description);
            continue;
        }
        // Otherwise, add ', '
        if (i == inv->num_items - 1) continue;
        printf(", ");
    }
    if (!print_full_list) printf("\n");
}

void assign_stats(Player *p) {
    char str_in[10] = {0};
    char stat;
    int num_points = 0;
    int *stat_ptr = nullptr;

    while (true) {
        print_header();
        printf("Assign your stat points. Unassigned stat points: %d\n", p->unused_stat_points);
        print_fighter_stats(p, 0);
        printf("Enter first letter of stat name and # of points to be allocated separated by a space.\n");
        printf("Enter 'q' to quit allocation and leave unassigned stat points.\n");

        fgets(str_in, sizeof(str_in), stdin);
        sscanf(str_in, " %c %d", &stat, &num_points);

        if (stat == 'q') return;

        printf("You entered: %d points into %c\n", num_points, stat);

        if (num_points > p->unused_stat_points) {
            printf("You aren't that powerful yet...\n");
            continue;
        }
        switch (stat) {
            case 'v': stat_ptr = &p->fighter.vitality;
                break;
            case 's': stat_ptr = &p->fighter.strength;
                break;
            case 'w': stat_ptr = &p->fighter.wisdom;
                break;
            case 'd': stat_ptr = &p->fighter.dexterity;
                break;
            default:
                printf("Invalid stat name.\n");
                continue;
        }
        if (*stat_ptr + num_points < MAX_NEG_STAT) {
            printf("You cannot sacrifice that much of this stat...\n");
            continue;
        }
        *stat_ptr += num_points;
        p->unused_stat_points -= num_points;
    }
}

int check_gather_item(Objective *obj, Player *p) {
    for (int i = 0; i < p->inventory.num_items; i++) {
        if (p->inventory.inventory[i].item_id == obj->target_id) {
            obj->progress = p->inventory.inventory[i].quantity > obj->quantity;
        }
    }
    return obj->progress;
}
int check_visit_area(Objective *obj, Player *p) {
    obj->progress = p->position_id == obj->target_id;
    return obj->progress;
}
int check_visit_npc(Objective *obj, Player *p) {
    // Won't work - modify to get npc from a list
    obj->progress = p->npc_flags & obj->target_id;
    return obj->progress;
}
int check_reach_level(Objective *obj, Player *p) {
    obj->progress = p->fighter.level >= obj->target_id;
    return obj->progress;
}

void evaluate_quest(Quest *q, Player *p) {
    int completed = 1;
    for (int i = 0; i < q->num_objectives; i++) {
        Objective *obj = &q->objectives[i];
        if (!obj->completion_func(obj, p)) {
            completed = 0;
        }
    }
    if (completed && !q->completed) {
        printf("Quest completed: %s\n", q->name);
        q->completed = 1;

        QuestReward *reward = &q->reward;
        // Give reward
        if (reward->coins) p->coins += reward->coins;
        if (reward->experience) p->experience += reward->experience;
        if (reward->item_id != ITEM_NONE) change_player_inventory(p, reward->item_id, reward->item_quantity);
    }
}

void read_new_name(Player *p) {
    print_header();
    printf("Enter your name (10 char max):\n");
    fgets(p->fighter.name, NAME_LEN, stdin);
    // Replaces the \n character at the end of p->pfighter.name with \0 to make printing easier
    p->fighter.name[strlen(p->fighter.name) - 1] = '\0';
}

void menu_menu(Player *p, Region *r) {
    char str_in[10] = {0};
    char setting;

    while (true) {
        print_header();
        printf("View your settings:\n");
        printf("'n' to change name,\n's' to view stats,\n'a' to allocate stats,\n"
            "'i' to view inventory ('d' for detailed mode)\nor 'q' to quit settings\n");

        fgets(str_in, sizeof(str_in), stdin);
        sscanf(str_in, "%c", &setting);

        if (setting == 'q') return;

        switch (setting) {
            case 'n':
                read_new_name(p);
                break;
            case 's':
                print_header();
                print_fighter_stats(p, 1);
                break;
            case 'a':
                assign_stats(p);
                break;
            case 'i':
                print_inventory(&p->inventory, 0);
                break;
            case 'd':
                print_inventory(&p->inventory, 1);
                break;
            default:
                printf("Invalid setting.\n");
        }
    }
}

void region_menu(Player *p) {
    char str_in[10] = {0};
    char input;

    while (true) {
        Region *region = &world[p->position_id];

        char menu_option = 'a';
        print_header();
        printf("%s\n%s\n", region->name, region->description);
        for (int i = 0; region->region_actions[i].name != nullptr; i++) {
            printf("'%c': %s\n", menu_option++, region->region_actions[i].name);
        }
        for (int i = 0; region->connections[i] != -1; i++) {
            printf("'%c': Travel to %s\n", menu_option++, world[region->connections[i]].name);
        }

        fgets(str_in, sizeof(str_in), stdin);
        sscanf(str_in, "%c", &input);

        menu_option = 'a';
        for (int i = 0; region->region_actions[i].name != nullptr; i++) {
            if (menu_option++ == input) {
                region->region_actions[i].action_func(p, region);
            }
        }
        for (int i = 0; region->connections[i] != -1; i++) {
            if (menu_option++ == input) {
                go_to(p, region->connections[i]);
            }
        }
    }
}

void go_to(Player *p, int id) {
    p->position_id = id;
    // check for position related quest completion
    for (int i = 0; i < p->num_quests; i++) {
        evaluate_quest(&p->active_quests[i], p);
    }
}

void admire(Player *p, Region *r) {
    print_header();
    if (strcmp(r->name, world[0].name) == 0) {
        printf("In the Town of Beginnings, %s has grown old enough to fulfil their dream: becoming an adventurer. "
            "It's time to begin your journey.\n",
            p->fighter.name);
    }
}



void check_quest_board(Player *p, Region *r) {
    char str_in[10] = {0};
    char action;

    print_header();
    if (strcmp(r->name, world[2].name) == 0) {
        printf("The guild's quest board is covered in quest notices. Three of them catch your eye:");

    }
}

void talk_to_receptionist(Player *p, Region *r) {
    char str_in[10] = {0};
    char action;

    while (true) {
        int is_guild_member = p->status_flags & GUILD_MEMBER;

        print_header();

        if (is_guild_member) {
            printf("Hello %s! What can I do for you?\n", p->fighter.name);
            printf("'v' to view guild status, 'r' to return to the guild hall\n");
        } else {
            printf("Hello %s! Finally ready to register with the guild?\n", p->fighter.name);
            printf("'y' to register with the guild,\n'n' to put it off\n");
        }

        fgets(str_in, sizeof(str_in), stdin);
        sscanf(str_in, "%c", &action);

        switch (action) {
            // Dialoge for guild member
            case 'v':
                if (!is_guild_member) break;
                printf("You are in the adventurer's guild.\n");
                if (has_item(&p->inventory, ITEM_GUILD_BADGE, 1)) {
                    printf("Great job holding on to your guild badge!\n");
                } else {
                    printf("Looks like you lost your guild badge, %s...\n", p->fighter.name);
                }
                continue;
            case 'r':
                printf("See you again soon, %s!\n", p->fighter.name);
                return;
            // Dialogue for non-guild member
            case 'y':
                if (is_guild_member) break;
                if (change_player_inventory(p, ITEM_GUILD_BADGE, 1)) {
                    p->status_flags |= GUILD_MEMBER;
                    printf("I'm sure you've been looking forward to this. "
                        "You are now officially part of the adventurer's guild!\n");
                    printf("If you lose your guild badge, I can replace it for you for only a few coins.");
                } else {
                    printf("Come back when your hands aren't full, %s!", p->fighter.name);
                }
                continue;
            case 'n':
                if (is_guild_member) break;
                printf("Not now then? Just make sure you register before leaving town!\n");
                return;
            default:
                printf("Invalid action.\n");
        }

        if (p->status_flags & GUILD_MEMBER) {
            printf("Hello %s! What can I do for you?\n", p->fighter.name);
            printf("'v' to view guild status, 'r' to return to the guild hall\n");
            fgets(str_in, sizeof(str_in), stdin);
            sscanf(str_in, "%c", &action);
            switch (action) {
                case 'v':
                    printf("You are in the adventurer's guild.\n");
                    continue;
                case 'r':
                    printf("See you again soon, %s!\n", p->fighter.name);
                    return;
                default:
                    printf("Invalid action.\n");
            }
        } else {
            fgets(str_in, sizeof(str_in), stdin);
            sscanf(str_in, "%c", &action);
            switch (action) {
                case 'y':
                    if (change_player_inventory(p, ITEM_GUILD_BADGE, 1)) {
                        p->status_flags |= GUILD_MEMBER;
                        printf("I'm sure you've been looking forward to this. "
                            "You are now officially part of the adventurer's guild!\n");
                        printf("If you lose your guild badge, I can replace it for you for only a few coins.");
                    } else {
                        printf("Come back when your hands aren't full, %s!", p->fighter.name);
                    }
                    continue;
                case 'n':
                    printf("Not now then? Just make sure you register before leaving town!\n");
                    return;
                default:
                    printf("Invalid action.\n");
            }
        }
    }
}

void talk_to_bartender(Player *p, Region *r) {
}

void explore(Player *p, Region *r) {
}

void forage(Player *p, Region *r) {
}

void fish(Player *p, Region *r) {
}

void battle(Player *p, Region *r) {
}

void become_beginner_wilds_expert(Player *p, const char *action_name) {
    p->status_flags |= BEGINNER_WILDS_EXPERT;

    printf("Your %s made you an expert on the beginner wilds!\n", action_name);
    printf("You can now choose which actions you wish to perform in the beginner wilds.\n");
}

// void beginner_wilds_menu() {
//     char str_in[10] = {0};
//     char action;
//
//     Action world_actions[] = {};
//     //        {"herb collecting", (action_type) herb_collecting},
//     //        {"fishing", (action_type) fishing},
//     //        {"battling", (action_type) battling}
//     //    };
//
//     while (true) {
//         int fortune = rand() % 10;
//
//         print_header();
//         printf("You are in the beginner wilds, outside of the Town of Beginnings.\n");
//         if (p->status_flags & BEGINNER_WILDS_EXPERT) {
//             printf("'b' to do battle in the wilds, 'g' to gather herbs, 'f' to go fishing\n");
//         } else {
//             printf("'e' to explore the wilds,\n");
//         }
//         printf("'l' to leave the beginner wilds and return to the Town of Beginnings,\n's' to view your settings\n");
//         fgets(str_in, sizeof(str_in), stdin);
//         sscanf(str_in, "%c", &action);
//
//         if (action == 'l') return;
//
//         switch (action) {
//             case 'e':
//                 if (p->status_flags & BEGINNER_WILDS_EXPERT) {
//                     printf("You already know the beginner wilds like the back of your hand.\n");
//                     break;
//                 }
//                 if (fortune < 5) {
//                     // herb collecting
//                     world_actions[0].action_func(&world[0], p);
//                 } else if (fortune < 7) {
//                     // fishing
//                     world_actions[1].action_func(&world[0]);
//                 } else {
//                     // battling
//                     world_actions[2].action_func(&world[0]);
//                 }
//                 break;
//             case 'g':
//                 if (p->status_flags & BEGINNER_WILDS_EXPERT) {
//                     // gather herbs
//                     world_actions[0].action_func(&world[0]);
//                 } else {
//                     printf("As if you'd seen a vision, you knew exactly where the herbs were!\n");
//                     become_beginner_wilds_expert(world_actions[0].name);
//                 }
//                 break;
//             case 'f':
//                 if (p->status_flags & BEGINNER_WILDS_EXPERT) {
//                     // go fishing
//                     world_actions[1].action_func(&world[0]);
//                 } else {
//                     printf("As if you'd seen a vision, you knew exactly where the fishing spots were!\n");
//                     become_beginner_wilds_expert(world_actions[1].name);
//                 }
//                 break;
//             case 'b':
//                 if (!(p->status_flags & BEGINNER_WILDS_EXPERT)) {
//                     printf("As if you'd seen a vision, you knew exactly where the monsters were!\n");
//                     become_beginner_wilds_expert(world_actions[2].name);
//                 }
//                 world_actions[2].action_func(&world[0]);
//                 break;
//             case 's':
//                 menu_menu();
//                 break;
//             default:
//                 printf("Invalid action.\n");
//         }
//     }
// }

int main(int argsc, char *argsv[]) {
    // Seed random number generation
    srand(time(nullptr));

    Player p = {
        {"DOE", 1, 1, 1, 1, 1, 10, 10, 00000000}, 3, (rand() % 3 + 3), 0, 0, 0, {{}, 0, 32}, 0, 0, {}, 00000000, 00000000
    };

    printf("\n");
    print_header();
    printf("********** TERMINUS **********\n");
    read_new_name(&p);
    printf("Welcome to TERMINUS, %s.\n", p.fighter.name);
    assign_stats(&p);
    region_menu(&p);

    return 0;
}

/* TODO
 * Maybe? Remove global world: put world in player or put world in main and pass it around
 * Maybe? make stat allocation char key list to be consistent with region_menu
 *
 * Battle sequence is determined by dex: player gets one action per 20(50/(50+DEX))+5.
 * So absolute minimum turns per battle tick is 5 (bc rounding), but requires 950 dex to achieve
 * Soft caps actions at one per 10/11 ticks
 *
 * Equipment
 *
 * Store NPCs and their dialogue in a table and only have one method for every NPC, like regions
 * use in quest completion checks
 */
