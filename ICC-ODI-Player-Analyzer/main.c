#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Players_data.h"

#define TEAM_COUNT 10
#define MAX_PLAYERS 50

/*
    Role indexes:
    0 = Batsman
    1 = Bowler
    2 = All-rounder
*/

typedef struct PlayerNode
{
    int id;
    char name[51];
    char teamName[51];
    int role;

    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;

    struct PlayerNode *next;
} PlayerNode;

typedef struct Team
{
    int id;
    char name[51];

    int totalPlayers;
    int battingPlayerCount;
    float strikeRateSum;
    float averageStrikeRate;

    PlayerNode *heads[3];
} Team;

/*
    Temporary nodes used only for merging.
    They point to original players without changing their lists.
*/
typedef struct ResultNode
{
    PlayerNode *player;
    struct ResultNode *next;
} ResultNode;

Team teamData[TEAM_COUNT];

const char *roleNames[3] =
{
    "Batsman",
    "Bowler",
    "All-rounder"
};


/* Read one complete line. */
void readLine(char text[], int size)
{
    int ch;

    if (fgets(text, size, stdin) == NULL)
    {
        printf("\nInput ended.\n");
        exit(EXIT_FAILURE);
    }

    if (strchr(text, '\n') != NULL)
    {
        text[strcspn(text, "\n")] = '\0';
    }
    else
    {
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
        }
    }
}


/* Read an integer within a specified range. */
int readInt(const char *message, int minimum, int maximum)
{
    char line[100];
    char extra;
    int value;

    while (1)
    {
        printf("%s", message);
        readLine(line, sizeof(line));

        if (sscanf(line, "%d %c", &value, &extra) == 1)
        {
            if (value >= minimum && value <= maximum)
            {
                return value;
            }
        }

        printf("Enter a number from %d to %d.\n",
               minimum, maximum);
    }
}


/* Read a non-negative decimal number. */
float readFloat(const char *message)
{
    char line[100];
    char extra;
    float value;

    while (1)
    {
        printf("%s", message);
        readLine(line, sizeof(line));

        if (sscanf(line, "%f %c", &value, &extra) == 1)
        {
            if (value >= 0 && value <= 1000000000.0f)
            {
                return value;
            }
        }

        printf("Enter a valid non-negative number.\n");
    }
}


void readName(char name[])
{
    char line[200];
    int length;

    while (1)
    {
        printf("Player name: ");
        readLine(line, sizeof(line));

        length = (int)strlen(line);

        if (length >= 1 && length <= 50 &&
            strspn(line, " \t") != strlen(line))
        {
            strcpy(name, line);
            return;
        }

        printf("Enter a name containing 1 to 50 characters.\n");
    }
}


void *allocateMemory(size_t size)
{
    void *memory = malloc(size);

    if (memory == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    return memory;
}


/* Convert the role text from the header into a role index. */
int getRole(const char *role)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        if (strcmp(role, roleNames[i]) == 0)
        {
            return i;
        }
    }

    printf("Unknown role in player data: %s\n", role);
    exit(EXIT_FAILURE);
}


float calculatePerformance(PlayerNode *player)
{
    if (player->role == 0)
    {
        return (player->battingAverage *
                player->strikeRate) / 100.0f;
    }
    else if (player->role == 1)
    {
        return (player->wickets * 2.0f) +
               (100.0f - player->economyRate);
    }
    else
    {
        return (player->battingAverage *
                player->strikeRate) / 100.0f +
               (player->wickets * 2.0f);
    }
}


/* Binary search: teamData remains sorted by team ID. */
Team *findTeam(int id)
{
    int left = 0;
    int right = TEAM_COUNT - 1;
    int middle;

    while (left <= right)
    {
        middle = left + (right - left) / 2;

        if (teamData[middle].id == id)
        {
            return &teamData[middle];
        }
        else if (teamData[middle].id < id)
        {
            left = middle + 1;
        }
        else
        {
            right = middle - 1;
        }
    }

    return NULL;
}


/* Used during initialization because source records have team names. */
Team *findTeamByName(const char *name)
{
    int i;

    for (i = 0; i < TEAM_COUNT; i++)
    {
        if (strcmp(teamData[i].name, name) == 0)
        {
            return &teamData[i];
        }
    }

    return NULL;
}


/*
    Higher performance comes first.
    For equal scores, smaller player ID comes first.
*/
int comesBefore(PlayerNode *first, PlayerNode *second)
{
    if (first->performanceIndex == second->performanceIndex)
    {
        return first->id < second->id;
    }

    return first->performanceIndex > second->performanceIndex;
}


/* Insert a player into the correct sorted role list. */
void insertPlayer(Team *team, PlayerNode *player)
{
    int role = player->role;
    PlayerNode *current;

    player->performanceIndex = calculatePerformance(player);
    player->next = NULL;

    if (team->heads[role] == NULL ||
        comesBefore(player, team->heads[role]))
    {
        player->next = team->heads[role];
        team->heads[role] = player;
    }
    else
    {
        current = team->heads[role];

        while (current->next != NULL &&
               !comesBefore(player, current->next))
        {
            current = current->next;
        }

        player->next = current->next;
        current->next = player;
    }

    team->totalPlayers++;

    if (role == 0 || role == 2)
    {
        team->battingPlayerCount++;
        team->strikeRateSum += player->strikeRate;

        team->averageStrikeRate =
            team->strikeRateSum / team->battingPlayerCount;
    }
}


void initializeData(void)
{
    int i;
    int role;
    Team *team;
    PlayerNode *player;

    if (teamCount != TEAM_COUNT)
    {
        printf("Expected exactly 10 teams.\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < TEAM_COUNT; i++)
    {
        teamData[i].id = i + 1;
        strcpy(teamData[i].name, teams[i]);

        teamData[i].totalPlayers = 0;
        teamData[i].battingPlayerCount = 0;
        teamData[i].strikeRateSum = 0;
        teamData[i].averageStrikeRate = 0;

        for (role = 0; role < 3; role++)
        {
            teamData[i].heads[role] = NULL;
        }
    }

    for (i = 0; i < playerCount; i++)
    {
        team = findTeamByName(players[i].team);

        if (team == NULL || team->totalPlayers >= MAX_PLAYERS)
        {
            printf("Invalid team data for %s.\n", players[i].name);
            exit(EXIT_FAILURE);
        }

        player = allocateMemory(sizeof(PlayerNode));

        player->id = players[i].id;
        strcpy(player->name, players[i].name);
        strcpy(player->teamName, players[i].team);

        player->role = getRole(players[i].role);
        player->totalRuns = players[i].totalRuns;
        player->battingAverage = players[i].battingAverage;
        player->strikeRate = players[i].strikeRate;
        player->wickets = players[i].wickets;
        player->economyRate = players[i].economyRate;

        insertPlayer(team, player);
    }
}


int playerIdExists(int id)
{
    int i;
    int role;
    PlayerNode *current;

    for (i = 0; i < TEAM_COUNT; i++)
    {
        for (role = 0; role < 3; role++)
        {
            current = teamData[i].heads[role];

            while (current != NULL)
            {
                if (current->id == id)
                {
                    return 1;
                }

                current = current->next;
            }
        }
    }

    return 0;
}


void showTeamIds(void)
{
    int i;

    printf("\nAvailable teams:\n");

    for (i = 0; i < TEAM_COUNT; i++)
    {
        printf("%d. %s\n", teamData[i].id, teamData[i].name);
    }
}


Team *selectTeam(void)
{
    int id;
    Team *team;

    id = readInt("Enter team ID: ", 1, 1000);
    team = findTeam(id);

    if (team == NULL)
    {
        printf("Team not found.\n");
    }

    return team;
}


int selectRole(void)
{
    printf("\n1. Batsman\n");
    printf("2. Bowler\n");
    printf("3. All-rounder\n");

    return readInt("Select role: ", 1, 3) - 1;
}


void printPlayerHeader(void)
{
    printf("\n%-5s %-25s %-14s %-12s %7s %8s %8s %6s %7s %10s\n",
           "ID", "Name", "Team", "Role", "Runs",
           "Average", "SR", "Wkts", "Economy", "Index");
}


void printPlayer(PlayerNode *player)
{
    printf("%-5d %-25s %-14s %-12s %7d %8.2f %8.2f %6d %7.2f %10.2f\n",
           player->id,
           player->name,
           player->teamName,
           roleNames[player->role],
           player->totalRuns,
           player->battingAverage,
           player->strikeRate,
           player->wickets,
           player->economyRate,
           player->performanceIndex);
}


/* Option 1 */
void addPlayer(void)
{
    int id;
    Team *team = selectTeam();
    PlayerNode *player;

    if (team == NULL)
    {
        return;
    }

    if (team->totalPlayers >= MAX_PLAYERS)
    {
        printf("This team already has 50 players.\n");
        return;
    }

    id = readInt("Player ID: ", 1, 1000);

    if (playerIdExists(id))
    {
        printf("This player ID already exists.\n");
        return;
    }

    player = allocateMemory(sizeof(PlayerNode));

    player->id = id;
    strcpy(player->teamName, team->name);

    readName(player->name);
    player->role = selectRole();

    player->totalRuns =
        readInt("Total runs: ", 0, 1000000000);

    player->battingAverage = readFloat("Batting average: ");
    player->strikeRate = readFloat("Strike rate: ");

    player->wickets =
        readInt("Wickets: ", 0, 1000000000);

    player->economyRate = readFloat("Economy rate: ");

    insertPlayer(team, player);

    printf("Player added to %s successfully.\n", team->name);
}


/* Option 2 */
void displayTeam(void)
{
    int role;
    Team *team = selectTeam();
    PlayerNode *current;

    if (team == NULL)
    {
        return;
    }

    printf("\nPlayers of %s:\n", team->name);
    printPlayerHeader();

    for (role = 0; role < 3; role++)
    {
        current = team->heads[role];

        while (current != NULL)
        {
            printPlayer(current);
            current = current->next;
        }
    }

    printf("\nTotal players: %d\n", team->totalPlayers);

    printf("Average batting strike rate: %.2f\n",
           team->averageStrikeRate);
}


/*
    Insertion sort is sufficient for the fixed 10 teams.
    This option has no specified complexity requirement.
*/

/* Option 3 */
void displayRankedTeams(void)
{
    Team *order[TEAM_COUNT];
    Team *current;
    int i;
    int j;

    for (i = 0; i < TEAM_COUNT; i++)
    {
        order[i] = &teamData[i];
    }

    for (i = 1; i < TEAM_COUNT; i++)
    {
        current = order[i];
        j = i - 1;

        while (j >= 0 &&
               order[j]->averageStrikeRate <
               current->averageStrikeRate)
        {
            order[j + 1] = order[j];
            j--;
        }

        order[j + 1] = current;
    }

    printf("\n%-5s %-16s %-10s %-12s\n",
           "ID", "Team", "Players", "Average SR");

    for (i = 0; i < TEAM_COUNT; i++)
    {
        printf("%-5d %-16s %-10d %-12.2f\n",
               order[i]->id,
               order[i]->name,
               order[i]->totalPlayers,
               order[i]->averageStrikeRate);
    }
}


/* Option 4 */
void displayTopPlayers(void)
{
    Team *team = selectTeam();
    PlayerNode *current;
    int role;
    int count;
    int shown = 0;

    if (team == NULL)
    {
        return;
    }

    role = selectRole();
    count = readInt("How many players? ", 1, MAX_PLAYERS);

    current = team->heads[role];

    if (current == NULL)
    {
        printf("No players found for this role.\n");
        return;
    }

    printPlayerHeader();

    while (current != NULL && shown < count)
    {
        printPlayer(current);
        current = current->next;
        shown++;
    }

    if (shown < count)
    {
        printf("\nOnly %d players exist for this role.\n", shown);
    }
}


/* Create a temporary list pointing to the original players. */
ResultNode *createResultList(PlayerNode *player)
{
    ResultNode *head = NULL;
    ResultNode *tail = NULL;
    ResultNode *node;

    while (player != NULL)
    {
        node = allocateMemory(sizeof(ResultNode));
        node->player = player;
        node->next = NULL;

        if (head == NULL)
        {
            head = node;
        }
        else
        {
            tail->next = node;
        }

        tail = node;
        player = player->next;
    }

    return head;
}


/* Merge two sorted temporary lists. */
ResultNode *mergeLists(ResultNode *first, ResultNode *second)
{
    ResultNode dummy;
    ResultNode *tail = &dummy;

    dummy.next = NULL;

    while (first != NULL && second != NULL)
    {
        if (comesBefore(first->player, second->player))
        {
            tail->next = first;
            first = first->next;
        }
        else
        {
            tail->next = second;
            second = second->next;
        }

        tail = tail->next;
    }

    if (first != NULL)
    {
        tail->next = first;
    }
    else
    {
        tail->next = second;
    }

    return dummy.next;
}


/* Option 5: balanced pairwise merging, O(N log t). */
void displayAllByRole(void)
{
    ResultNode *lists[TEAM_COUNT];
    ResultNode *current;
    ResultNode *next;

    int role = selectRole();
    int listCount = TEAM_COUNT;
    int newCount;
    int i;

    for (i = 0; i < TEAM_COUNT; i++)
    {
        lists[i] = createResultList(teamData[i].heads[role]);
    }

    /*
        Merge in rounds:
        10 lists -> 5 lists -> 3 lists -> 2 lists -> 1 list
    */
    while (listCount > 1)
    {
        newCount = 0;

        for (i = 0; i < listCount; i += 2)
        {
            if (i + 1 < listCount)
            {
                lists[newCount] =
                    mergeLists(lists[i], lists[i + 1]);
            }
            else
            {
                lists[newCount] = lists[i];
            }

            newCount++;
        }

        listCount = newCount;
    }

    current = lists[0];

    if (current == NULL)
    {
        printf("No players found for this role.\n");
        return;
    }

    printPlayerHeader();

    while (current != NULL)
    {
        printPlayer(current->player);

        next = current->next;
        free(current);
        current = next;
    }
}


/* Release the original player lists when exiting. */
void freePlayers(void)
{
    int i;
    int role;
    PlayerNode *current;
    PlayerNode *next;

    for (i = 0; i < TEAM_COUNT; i++)
    {
        for (role = 0; role < 3; role++)
        {
            current = teamData[i].heads[role];

            while (current != NULL)
            {
                next = current->next;
                free(current);
                current = next;
            }

            teamData[i].heads[role] = NULL;
        }
    }
}


int main(void)
{
    int choice;

    initializeData();

    printf("Loaded %d players from %d teams.\n",
           playerCount, teamCount);

    showTeamIds();

    do
    {
        printf("\nICC ODI Player Performance Analyzer\n");
        printf("1. Add player to a team\n");
        printf("2. Display players of a team\n");
        printf("3. Rank teams by average batting strike rate\n");
        printf("4. Display top K players by team and role\n");
        printf("5. Display all players of one role\n");
        printf("6. Exit\n");

        choice = readInt("Enter your choice: ", 1, 6);

        switch (choice)
        {
            case 1:
                addPlayer();
                break;

            case 2:
                displayTeam();
                break;

            case 3:
                displayRankedTeams();
                break;

            case 4:
                displayTopPlayers();
                break;

            case 5:
                displayAllByRole();
                break;

            case 6:
                freePlayers();
                printf("Program closed.\n");
                break;
        }
    }
    while (choice != 6);

    return 0;
}