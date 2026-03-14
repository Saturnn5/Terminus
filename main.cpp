#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
#define GUILD_MEMBER (1 << 0)
#define BEGINNER_WILDS_EXPERT (1 << 1)

typedef struct {
    char name[NAME_LEN];

    // stats
    int vitality;
    int strength;
    int wisdom;
    int dexterity;
    int unused_stat_points;

    // resources
    int level;
    int coins;
    int max_hitpoints;
    int current_hitpoints;
    int reputation;
    int guild_reputation;

    // battle statuses
    unsigned int battle_status_effects;

    // statuses
    unsigned int status_flags;
} Player;

// Used to define world actions such as picking herbs or fishing
typedef struct {
    const char *name;
    void(*action_func)(void);
} Action;

Player p;
const char* difficulty_names[3] = {"Easy", "Normal", "Challenge"};
const float difficulty_nums[] = {0.8, 1.0, 1.2, 1.5};

void beginning_town_menu();

void print_header() {
    printf("******************************\n");
}

void print_player_stats() {
    printf("%s is level %d. %s's stats:\nVitality: %d\nStrength: %d\nWisdom: %d\nDexterity: %d\nUnassigned points: %d\n", p.name, p.level,p.name,p.vitality,p.strength,p.wisdom,p.dexterity,p.unused_stat_points);
    printf("%s has %d coins.\n", p.name, p.coins);
}

void assign_stats() {
    char str_in[10] = {0};
    char stat;
    int num_points = 0;
    int stat_assigned = 0;

    while (true) {
        print_header();
        printf("Assign your stat points. Unassigned stat points: %d\n", p.unused_stat_points);
        print_player_stats();
        printf("Enter first letter of stat name and # of points to be allocated separated by a space.\n");
        printf("Enter 'q' to quit allocation and leave unassigned stat points.\n");

        fgets(str_in, sizeof(str_in), stdin);
        sscanf(str_in, " %c %d", &stat, &num_points);

        if (stat == 'q') return;

        printf("You entered: %d points into %c\n", num_points, stat);

        if (num_points > p.unused_stat_points) {
            printf("You aren't that powerful yet...\n");
            continue;
        }
        switch (stat) {
            case 'v':
                if (p.vitality + num_points < MAX_NEG_STAT) {
                    printf("You cannot sacrifice this much vitality...\n");
                    stat_assigned = 0;
                    break;
                }
                p.vitality += num_points;
                stat_assigned = 1;
                break;
            case 's':
                if (p.strength + num_points < MAX_NEG_STAT) {
                    printf("You cannot sacrifice this much strength...\n");
                    stat_assigned = 0;
                    break;
                }
                p.strength += num_points;
                stat_assigned = 1;
                break;
            case 'w':
                if (p.wisdom + num_points < MAX_NEG_STAT) {
                    printf("You cannot sacrifice this much wisdom...\n");
                    stat_assigned = 0;
                    break;
                }
                p.wisdom += num_points;
                stat_assigned = 1;
                break;
            case 'd':
                if (p.dexterity + num_points < MAX_NEG_STAT) {
                    printf("You cannot sacrifice this much dexterity...\n");
                    stat_assigned = 0;
                    break;
                }
                p.dexterity += num_points;
                stat_assigned = 1;
                break;
            default:
                printf("Invalid stat name.\n");
        }
        if (stat_assigned) {
            p.unused_stat_points -= num_points;
        }

        if (p.unused_stat_points == 0) {
            print_player_stats();
            printf("All points allocated, returning...\n");
            return;
        }
    }
}

void read_new_name() {
    print_header();
    printf("Enter your name (10 char max):\n");
    fgets(p.name, NAME_LEN, stdin);
    // Replaces the \n character at the end of p.name with \0 to make printing easier
    p.name[strlen(p.name) - 1] = '\0';
}

void settings_menu() {
    char str_in[10] = {0};
    char setting;

    while (true) {
        print_header();
        printf("View your settings:\n");
        printf("'n' to change name,\n's' to view stats,\n'a' to allocate stats,\nor 'q' to quit settings\n");
        fgets(str_in, sizeof(str_in), stdin);
        sscanf(str_in, "%c", &setting);
        switch (setting) {
            case 'q':
                return;
            case 'n':
                read_new_name();
                break;
            case 's':
                print_player_stats();
                break;
            case 'a':
                assign_stats();
                break;
            default:
                printf("Invalid setting.\n");
        }
    }
}

void print_beginning_town_message() {
    print_header();
    printf("In the Town of Beginnings, %s has grown old enough to fulfil their dream: becoming an adventurer. It's time to begin your journey.\n", p.name);
}

void guild_receptionist_menu() {
    char str_in[10] = {0};
    char action;

    while (true) {
        print_header();
        if (p.status_flags & GUILD_MEMBER) {
            printf("Hello %s! What can I do for you?\n", p.name);
            printf("'v' to view guild status, 'r' to return to the guild hall\n");
            fgets(str_in, sizeof(str_in), stdin);
            sscanf(str_in, "%c", &action);
            switch (action) {
                case 'v':
                    printf("You are in the adventurer's guild.\n");
                    return;
                case 'r':
                    printf("See you again soon, %s!", p.name);
                    return;
                default:
                    printf("Invalid action.\n");
            }
        } else {
            printf("Hello %s! Finally ready to register with the guild?\n", p.name);
            printf("'y' to register with the guild,\n'n' to put it off\n");
            fgets(str_in, sizeof(str_in), stdin);
            sscanf(str_in, "%c", &action);
            switch (action) {
                case 'y':
                    p.status_flags |= GUILD_MEMBER;
                    printf("I'm sure you've been looking forward to this. You are now officially part of the adventurer's guild!\n");
                    return;
                case 'n':
                    printf("Not now then? Just make sure you register before leaving town!\n");
                    return;
                default:
                    printf("Invalid action.\n");
            }
        }
    }
}

void beginning_guild_menu() {
    char str_in[10] = {0};
    char action;

    while (true) {
        print_header();
        printf("You are in the Town of Beginnings guild hall.\n");
        printf("'r' to speak to the receptionist,\n'l' to leave the guild hall and return to the Town of Beginnings,\n's' to view your settings\n");
        fgets(str_in, sizeof(str_in), stdin);
        sscanf(str_in, "%c", &action);
        switch (action) {
            case 'r':
                guild_receptionist_menu();
                break;
            case 'l':
                return;
            case 's':
                settings_menu();
                break;
            default:
                printf("Invalid action.\n");
        }
    }
}

void fight();

void become_beginner_wilds_expert(int action) {
    const char *action_names[4] = {"herb collecting", "fishing", "battling", "vision"};

    p.status_flags |= BEGINNER_WILDS_EXPERT;

    printf("Your %s made you an expert on the beginner wilds!\n", action_names[action]);
    printf("You can now choose which actions you wish to perform in the beginner wilds.\n");
}

void herb_collecting() {
    printf("You gather herbs.\n");
}

void fishing() {
    printf("You go fishing.\n");
}

void battling() {
    printf("You do battle with some monsters!\n");
}

void beginner_wilds_menu() {
    char str_in[10] = {0};
    char action;

    Action world_actions[] = {
        {"herb collecting", herb_collecting},
        {"fishing", fishing},
        {"battling", battling}
    };
    int action_count = sizeof(world_actions)/sizeof(world_actions[0]);
    int fortune;

    while (true) {
        fortune = rand() % 10;

        print_header();
        printf("You are in the beginner wilds, outside of the Town of Beginnings.\n");
        if (p.status_flags & BEGINNER_WILDS_EXPERT) {
            printf("'b' to do battle in the wilds, 'g' to gather herbs, 'f' to go fishing\n");
        } else {
            printf("'e' to explore the wilds,\n");
        }
        printf("'l' to leave the beginner wilds and return to the Town of Beginnings,\n's' to view your settings\n");
        fgets(str_in, sizeof(str_in), stdin);
        sscanf(str_in, "%c", &action);
        switch (action) {
            case 'e':
                if (p.status_flags & BEGINNER_WILDS_EXPERT) {
                    printf("You already know the beginner wilds like the back of your hand.\n");
                    break;
                }
                if (fortune < 5) {
                    // herb collecting
                    world_actions[0].action_func();
                } else if (fortune < 7) {
                    // fishing
                    world_actions[1].action_func();
                } else {
                    // battling
                    world_actions[2].action_func();
                }
                break;
            case 'b':
                if (!(p.status_flags & BEGINNER_WILDS_EXPERT)) {
                    printf("As if you'd seen a vision, you knew exactly where the monsters were!\n");
                    become_beginner_wilds_expert(3);
                }
                world_actions[2].action_func();
                break;
            case 'g':
                if (p.status_flags & BEGINNER_WILDS_EXPERT) {
                    // gather herbs
                } else {
                    printf("As if you'd seen a vision, you knew exactly where the herbs were!\n");
                    become_beginner_wilds_expert(3);
                }
                break;
            case 'f':
                if (p.status_flags & BEGINNER_WILDS_EXPERT) {
                    // go fishing
                } else {
                    printf("As if you'd seen a vision, you knew exactly where the fishing spots were!\n");
                    become_beginner_wilds_expert(3);
                }
                break;
            case 'l':
                return;
            case 's':
                settings_menu();
                break;
            default:
                printf("Invalid action.\n");
        }
    }
}

void beginning_town_menu() {
    char str_in[10] = {0};
    char action;

    while (true) {
        print_header();
        printf("You are in the Town of Beginnings.\n");
        printf("'g' to visit the guild,\n'l' to leave and enter beginner wilds,\n's' to view your settings\n");
        fgets(str_in, sizeof(str_in), stdin);
        sscanf(str_in, "%c", &action);
        switch (action) {
            case 'g':
                beginning_guild_menu();
                break;
            case 'l':
                beginner_wilds_menu();
                break;
            case 's':
                settings_menu();
                break;
            default:
                printf("Invalid action.\n");
        }
    }
}

int main(int argsc, char* argsv[]) {
    // Seed random number generation
    srand(time(nullptr));

    p = {"DOE", 1, 1, 1, 1, 3, 1, rand() % 3 + 3, 10, 10, 1, 1, 00000000, 00000000};

    printf("\n******************************\n********** TERMINUS **********\n");
    read_new_name();
    printf("\nWelcome to TERMINUS, %s.\n", p.name);
    assign_stats();
    print_beginning_town_message();
    beginning_town_menu();

    return 0;
}

/* Battle sequence is determined by dex: player gets one action per 20(50/(50+DEX))+5.
 * So absolute minimum turns per battle tick is 5 (bc rounding), but requires 950 dex to achieve
 * Soft caps actions at one per 10/11 ticks
 */