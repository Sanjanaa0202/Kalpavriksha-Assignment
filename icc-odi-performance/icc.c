#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Players_data.h"

#define TEAM_COUNT 10
#define MAX_NAME 51
#define MAX_TEAM_PLAYERS 50
#define BATSMAN 1
#define BOWLER 2
#define ALL_ROUNDER 3
#define ROLE_COUNT 3

typedef struct {
    int player_id;
    char name[MAX_NAME];
    char team_name[MAX_NAME];
    int role;
    int total_runs;
    float batting_average;
    float strike_rate;
    int wickets;
    float economy_rate;
    float performance_index;
} odi_player;

typedef struct player_node {
    odi_player player;
    struct player_node *next;
} player_node;

typedef struct role_node {
    player_node *player_ref;
    struct role_node *next;
} role_node;

typedef struct {
    int team_id;
    char name[MAX_NAME];
    int total_players;
    float average_batting_strike_rate;
    player_node *head;
    player_node *tail;
    role_node *role_heads[ROLE_COUNT];
} team;

typedef struct {
    role_node *current;
    int team_index;
} heap_node;

static team team_list[TEAM_COUNT];
static const int header_team_ids[TEAM_COUNT] = {9, 2, 7, 3, 1, 5, 4, 6, 8, 10};

void calculate_team_average(team *team_ref);
int read_integer(const char *message);
float read_float(const char *message);
void read_string(const char *message, char *buffer, int size);
int get_role_index(int role);
float calculate_performance_index(const odi_player *player);
void initialize_teams();
int find_team_by_id(int team_id);
int find_team_by_name(const char *team_name);
void insert_role_sorted(team *team_ref, player_node *player_ref);
void insert_player(team *team_ref, const odi_player *player);
void calculate_team_average(team *team_ref);
void initialize_players();
int player_id_exists(int player_id);
void add_player();
void display_player_header(int include_team);
void display_player_row(const odi_player *player, int include_team);
void display_players_of_team();
int compare_teams(const void *first, const void *second);
void display_teams_by_strike_rate();
void display_top_k_players();
void swap_heap_nodes(heap_node *first, heap_node *second);
void heap_insert(heap_node heap[], int *heap_size, heap_node value);
void display_all_players_by_role();
void display_menu();
void free_memory();

int main() {
    int choice;
    initialize_teams();
    initialize_players();
    printf("Players and teams initialized successfully.\n");
    while (1) {
        display_menu();
        choice = read_integer("Enter your choice: ");
        if (choice == 1) {
            add_player();
        }
        else if (choice == 2) {
            display_players_of_team();
        }
        else if (choice == 3) {
            display_teams_by_strike_rate();
        }
        else if (choice == 4) {
            display_top_k_players();
        }
        else if (choice == 5) {
            display_all_players_by_role();
        }
        else if (choice == 6) {
            free_memory();
            printf("Exiting ICC ODI Player Performance Analyzer.\n");
            return 0;
        }
        else {
            printf("Invalid choice. Please try again.\n");
        }
    }
}

int read_integer(const char *message) {
    char input[100];
    char *end;
    long value;
    while (1) {
        printf("%s", message);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return -1;
        }
        value = strtol(input, &end, 10);
        while (isspace((unsigned char)*end)) {
            end++;
        }
        if (end != input && *end == '\0' &&
            value >= -2147483647L && value <= 2147483647L) {
                return (int)value;
        }
        printf("Invalid input. Please enter an integer.\n");
    }
}

float read_float(const char *message) {
    char input[100];
    char *end;
    float value;
    while (1) {
        printf("%s", message);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return -1.0f;
        }
        value = strtof(input, &end);
        while (isspace((unsigned char)*end)) {
            end++;
        }
        if (end != input && *end == '\0') {
            return value;
        }
        printf("Invalid input. Please enter a number.\n");
    }
}

void read_string(const char *message, char *buffer, int size) {
    size_t length;
    printf("%s", message);
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    }
    else {
        int character;
        while ((character = getchar()) != '\n' && character != EOF);
    }
}

int get_role_index(int role) {
    return role - 1;
}

const char *get_role_name(int role) {
    if (role == BATSMAN) {
        return "Batsman";
    }
    if (role == BOWLER) {
        return "Bowler";
    }
    if (role == ALL_ROUNDER) {
        return "All-rounder";
    }
    return "Invalid";
}

float calculate_performance_index(const odi_player *player) {
    if (player->role == BATSMAN) {
        return (player->batting_average * player->strike_rate) / 100.0f;
    }
    if (player->role == BOWLER) {
        return (player->wickets * 2.0f) + (100.0f - player->economy_rate);
    }
    return ((player->batting_average * player->strike_rate) / 100.0f)
           + (player->wickets * 2.0f);
}

void initialize_teams() {
    int header_index;
    int team_index;
    for (header_index = 0; header_index < TEAM_COUNT; header_index++) {
        team_index = header_team_ids[header_index] - 1;
        team_list[team_index].team_id = header_team_ids[header_index];
        strcpy(team_list[team_index].name, teams[header_index]);
        team_list[team_index].total_players = 0;
        team_list[team_index].average_batting_strike_rate = 0.0f;
        team_list[team_index].head = NULL;
        team_list[team_index].tail = NULL;

        for (int role_index = 0; role_index < ROLE_COUNT; role_index++) {
            team_list[team_index].role_heads[role_index] = NULL;
        }
    }
}

int find_team_by_id(int team_id) {
    int low = 0;
    int high = TEAM_COUNT - 1;
    while (low <= high) {
        int middle = low + (high - low) / 2;
        if (team_list[middle].team_id == team_id) {
            return middle;
        }
        if (team_list[middle].team_id < team_id) {
            low = middle + 1;
        }
        else {
            high = middle - 1;
        }
    }
    return -1;
}

int find_team_by_name(const char *team_name) {
    int index;
    for (index = 0; index < TEAM_COUNT; index++) {
        if (strcmp(team_list[index].name, team_name) == 0) {
            return index;
        }
    }
    return -1;
}

player_node *create_player_node(const odi_player *player) {
    player_node *new_node = malloc(sizeof(player_node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    new_node->player = *player;
    new_node->next = NULL;
    return new_node;
}

role_node *create_role_node(player_node *player_ref) {
    role_node *new_node = malloc(sizeof(role_node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    new_node->player_ref = player_ref;
    new_node->next = NULL;
    return new_node;
}

void insert_role_sorted(team *team_ref, player_node *player_ref) {
    int role_index = get_role_index(player_ref->player.role);
    role_node **current = &team_ref->role_heads[role_index];
    role_node *new_node = create_role_node(player_ref);
    while (*current != NULL &&
           (*current)->player_ref->player.performance_index >=
                player_ref->player.performance_index) {
        current = &(*current)->next;
    }
    new_node->next = *current;
    *current = new_node;
}

void insert_player(team *team_ref, const odi_player *player) {
    player_node *new_node = create_player_node(player);
    if (team_ref->head == NULL) {
        team_ref->head = new_node;
        team_ref->tail = new_node;
    }
    else {
        team_ref->tail->next = new_node;
        team_ref->tail = new_node;
    }
    team_ref->total_players++;
    insert_role_sorted(team_ref, new_node);
    calculate_team_average(team_ref);
}

void calculate_team_average(team *team_ref) {
    player_node *current = team_ref->head;
    float total_strike_rate = 0.0f;
    int batter_count = 0;
    while (current != NULL) {
        if (current->player.role == BATSMAN ||
            current->player.role == ALL_ROUNDER) {
                total_strike_rate += current->player.strike_rate;
                batter_count++;
        }
        current = current->next;
    }
    if (batter_count == 0) {
        team_ref->average_batting_strike_rate = 0.0f;
    }
    else {
        team_ref->average_batting_strike_rate = total_strike_rate / batter_count;
    }
}

void initialize_players() {
    int player_index;
    int team_index;
    odi_player new_player;
    for (player_index = 0; player_index < playerCount; player_index++) {
        team_index = find_team_by_name(players[player_index].team);
        if (team_index == -1) {
            continue;
        }
        new_player.player_id = players[player_index].id;
        snprintf(new_player.name, MAX_NAME, "%s", players[player_index].name);
        snprintf(new_player.team_name, MAX_NAME, "%s", players[player_index].team);

        if (strcmp(players[player_index].role, "Batsman") == 0) {
            new_player.role = BATSMAN;
        }
        else if (strcmp(players[player_index].role, "Bowler") == 0) {
            new_player.role = BOWLER;
        }
        else {
            new_player.role = ALL_ROUNDER;
        }

        new_player.total_runs = players[player_index].totalRuns;
        new_player.batting_average = players[player_index].battingAverage;
        new_player.strike_rate = players[player_index].strikeRate;
        new_player.wickets = players[player_index].wickets;
        new_player.economy_rate = players[player_index].economyRate;
        new_player.performance_index = calculate_performance_index(&new_player);
        insert_player(&team_list[team_index], &new_player);
    }
}

int player_id_exists(int player_id) {
    int team_index;
    player_node *current;
    for (team_index = 0; team_index < TEAM_COUNT; team_index++) {
        current = team_list[team_index].head;
        while (current != NULL) {
            if (current->player.player_id == player_id) {
                return 1;
            }
            current = current->next;
        }
    }
    return 0;
}

void add_player() {
    int team_id;
    int team_index;
    odi_player new_player;
    team *team_ref;
    team_id = read_integer("\nEnter Team ID to add player: ");
    team_index = find_team_by_id(team_id);
    if (team_index == -1) {
        printf("Invalid Team ID.\n");
        return;
    }
    team_ref = &team_list[team_index];
    if (team_ref->total_players >= MAX_TEAM_PLAYERS) {
        printf("This team has reached the maximum of %d players.\n",
               MAX_TEAM_PLAYERS);
        return;
    }
    new_player.player_id = read_integer("Player ID: ");
    if (new_player.player_id < 1 || new_player.player_id > 1000) {
        printf("Player ID must be between 1 and 1000.\n");
        return;
    }
    if (player_id_exists(new_player.player_id)) {
        printf("This Player ID already exists.\n");
        return;
    }
    read_string("Name: ", new_player.name, MAX_NAME);
    if (strlen(new_player.name) == 0) {
        printf("Name cannot be empty.\n");
        return;
    }
    strcpy(new_player.team_name, team_ref->name);
    new_player.role = read_integer("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (new_player.role < BATSMAN || new_player.role > ALL_ROUNDER) {
        printf("Invalid role.\n");
        return;
    }

    new_player.total_runs = read_integer("Total Runs: ");
    new_player.batting_average = read_float("Batting Average: ");
    new_player.strike_rate = read_float("Strike Rate: ");
    new_player.wickets = read_integer("Wickets: ");
    new_player.economy_rate = read_float("Economy Rate: ");
    if (new_player.total_runs < 0 ||
        new_player.batting_average < 0.0f ||
        new_player.strike_rate < 0.0f ||
        new_player.wickets < 0 ||
        new_player.economy_rate < 0.0f) {
            printf("Player statistics cannot be negative.\n");
            return;
    }
    new_player.performance_index = calculate_performance_index(&new_player);
    insert_player(team_ref, &new_player);
    printf("Player added successfully to Team %s.\n", team_ref->name);
}

void display_player_header(int include_team) {
    printf("\n");
    if (include_team) {
        printf("%-6s %-22s %-16s %-13s %-7s %-7s %-7s %-7s %-7s %-10s\n",
               "ID", "Name", "Team", "Role", "Runs", "Avg",
               "SR", "Wkts", "ER", "Perf.Index");
    }
    else {
        printf("%-6s %-22s %-13s %-7s %-7s %-7s %-7s %-7s %-10s\n",
               "ID", "Name", "Role", "Runs", "Avg",
               "SR", "Wkts", "ER", "Perf.Index");
    }
}

void display_player_row(const odi_player *player, int include_team) {
    if (include_team) {
        printf("%-6d %-22s %-16s %-13s %-7d %-7.1f %-7.1f %-7d %-7.1f %-10.2f\n",
               player->player_id, player->name, player->team_name,
               get_role_name(player->role), player->total_runs,
               player->batting_average, player->strike_rate,
               player->wickets, player->economy_rate,
               player->performance_index);
    }
    else {
        printf("%-6d %-22s %-13s %-7d %-7.1f %-7.1f %-7d %-7.1f %-10.2f\n",
               player->player_id, player->name,
               get_role_name(player->role), player->total_runs,
               player->batting_average, player->strike_rate,
               player->wickets, player->economy_rate,
               player->performance_index);
    }
}

void display_players_of_team() {
    int team_id;
    int team_index;
    player_node *current;
    team *team_ref;
    team_id = read_integer("\nEnter Team ID: ");
    team_index = find_team_by_id(team_id);
    if (team_index == -1) {
        printf("Invalid Team ID.\n");
        return;
    }
    team_ref = &team_list[team_index];
    printf("\nPlayers of Team %s:\n", team_ref->name);
    display_player_header(0);
    current = team_ref->head;
    while (current != NULL) {
        display_player_row(&current->player, 0);
        current = current->next;
    }
    printf("Total Players: %d\n", team_ref->total_players);
    printf("Average Batting Strike Rate: %.2f\n", team_ref->average_batting_strike_rate);
}

int compare_teams(const void *first, const void *second) {
    const team *team_a = *(const team **)first;
    const team *team_b = *(const team **)second;
    if (team_a->average_batting_strike_rate <
        team_b->average_batting_strike_rate) {
            return 1;
    }
    if (team_a->average_batting_strike_rate >
        team_b->average_batting_strike_rate) {
            return -1;
    }
    return 0;
}

void display_teams_by_strike_rate() {
    team *sorted_teams[TEAM_COUNT];
    int index;
    for (index = 0; index < TEAM_COUNT; index++) {
        sorted_teams[index] = &team_list[index];
    }
    qsort(sorted_teams, TEAM_COUNT, sizeof(team *), compare_teams);
    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("%-8s %-20s %-15s %-12s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    for (index = 0; index < TEAM_COUNT; index++) {
        printf("%-8d %-20s %-15.2f %-12d\n",
               sorted_teams[index]->team_id,
               sorted_teams[index]->name,
               sorted_teams[index]->average_batting_strike_rate,
               sorted_teams[index]->total_players);
    }
}

void display_top_k_players() {
    int team_id;
    int team_index;
    int role;
    int k;
    int displayed = 0;
    role_node *current;
    team *team_ref;
    team_id = read_integer("\nEnter Team ID: ");
    team_index = find_team_by_id(team_id);

    if (team_index == -1) {
        printf("Invalid Team ID.\n");
        return;
    }

    role = read_integer("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (role < BATSMAN || role > ALL_ROUNDER) {
        printf("Invalid role.\n");
        return;
    }

    k = read_integer("Enter number of players (K): ");
    if (k <= 0) {
        printf("K must be greater than zero.\n");
        return;
    }

    team_ref = &team_list[team_index];
    current = team_ref->role_heads[get_role_index(role)];
    if (current == NULL) {
        printf("No players found for this role in %s.\n", team_ref->name);
        return;
    }
    printf("\nTop %d %s of Team %s:\n", k, get_role_name(role), team_ref->name);
    display_player_header(0);
    while (current != NULL && displayed < k) {
        display_player_row(&current->player_ref->player, 0);
        current = current->next;
        displayed++;
    }
    if (displayed < k) {
        printf("Only %d players are available for this role.\n", displayed);
    }
}

void swap_heap_nodes(heap_node *first, heap_node *second) {
    heap_node temporary = *first;
    *first = *second;
    *second = temporary;
}

void heap_insert(heap_node heap[], int *heap_size, heap_node value) {
    int index = *heap_size;
    heap[index] = value;
    while (index > 0) {
        int parent = (index - 1) / 2;
        float parent_value =
            heap[parent].current->player_ref->player.performance_index;
        float current_value =
            heap[index].current->player_ref->player.performance_index;
        if (parent_value >= current_value) {
            break;
        }
        swap_heap_nodes(&heap[parent], &heap[index]);
        index = parent;
    }
    (*heap_size)++;
}

heap_node heap_remove_max(heap_node heap[], int *heap_size) {
    heap_node maximum = heap[0];
    int index = 0;
    (*heap_size)--;
    if (*heap_size == 0) {
        return maximum;
    }
    heap[0] = heap[*heap_size];
    while (1) {
        int left = (2 * index) + 1;
        int right = (2 * index) + 2;
        int largest = index;
        if (left < *heap_size &&
            heap[left].current->player_ref->player.performance_index >
            heap[largest].current->player_ref->player.performance_index) {
                largest = left;
        }
        if (right < *heap_size &&
            heap[right].current->player_ref->player.performance_index >
            heap[largest].current->player_ref->player.performance_index) {
                largest = right;
        }
        if (largest == index) {
            break;
        }
        swap_heap_nodes(&heap[index], &heap[largest]);
        index = largest;
    }
    return maximum;
}

void display_all_players_by_role() {
    int role;
    int team_index;
    int heap_size = 0;
    heap_node heap[TEAM_COUNT];
    role = read_integer("\nEnter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (role < BATSMAN || role > ALL_ROUNDER) {
        printf("Invalid role.\n");
        return;
    }
    for (team_index = 0; team_index < TEAM_COUNT; team_index++) {
        role_node *first_node = team_list[team_index].role_heads[get_role_index(role)];
        if (first_node != NULL) {
            heap_node entry;
            entry.current = first_node;
            entry.team_index = team_index;
            heap_insert(heap, &heap_size, entry);
        }
    }
    if (heap_size == 0) {
        printf("No players found for this role.\n");
        return;
    }
    printf("\n%s of All Teams:\n", get_role_name(role));
    display_player_header(1);
    while (heap_size > 0) {
        heap_node maximum = heap_remove_max(heap, &heap_size);
        player_node *player_ref = maximum.current->player_ref;
        role_node *next_node = maximum.current->next;
        display_player_row(&player_ref->player, 1);
        if (next_node != NULL) {
            heap_node next_entry;
            next_entry.current = next_node;
            next_entry.team_index = maximum.team_index;
            heap_insert(heap, &heap_size, next_entry);
        }
    }
}

void display_menu() {
    printf("\nICC ODI PLAYER PERFORMANCE ANALYZER\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display All Players of Specific Role Across All Teams\n");
    printf("6. Exit\n");
}

void free_memory() {
    int team_index;
    int role_index;
    for (team_index = 0; team_index < TEAM_COUNT; team_index++) {
        player_node *player_current = team_list[team_index].head;
        while (player_current != NULL) {
            player_node *next_player = player_current->next;
            free(player_current);
            player_current = next_player;
        }
        for (role_index = 0; role_index < ROLE_COUNT; role_index++) {
            role_node *role_current =
                team_list[team_index].role_heads[role_index];
            while (role_current != NULL) {
                role_node *next_role = role_current->next;
                free(role_current);
                role_current = next_role;
            }
        }
    }
}

