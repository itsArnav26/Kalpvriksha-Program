#include <stdio.h>
#include <string.h>

struct user
{
    int id;
    char name[100];
    int age;
};

void createUser();
void readUser();
void deleteUser();
void updateUser();

int main()
{
    char menuChoice;

    do
    {
        printf("\n------- CHOOSE -------\n");
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        printf("\n\nEnter Your Choice : ");
        scanf(" %c", &menuChoice);

        switch (menuChoice)
        {
        case '1':
            createUser();
            break;

        case '2':
            readUser();
            break;

        case '3':
            updateUser();
            break;

        case '4':
            deleteUser();
            break;

        case '5':
            break;

        default:
            printf("Invalid Choice");
            break;
        }

        if (menuChoice != '5')
        {
            char continueChoice;

            printf("\nDo you want to continue .....\n");
            printf("Enter Your choice Y/N: ");
            scanf(" %c", &continueChoice);

            if (continueChoice == 'N' || continueChoice == 'n')
            {
                menuChoice = '5';
            }
        }

    } while (menuChoice != '5');

    return 0;
}

void createUser()
{
    char recordLine[200];
    FILE *userFile = fopen("users.txt", "a+");
    struct user userData;

    if (userFile == NULL)
    {
        printf("Unable to open file");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &userData.id);

    rewind(userFile);

    while (fgets(recordLine, sizeof(recordLine), userFile) != NULL)
    {
        int currentIndex = 0;
        int existingUserId = 0;

        while (recordLine[currentIndex] >= '0' &&
               recordLine[currentIndex] <= '9')
        {
            existingUserId =
                existingUserId * 10 + (recordLine[currentIndex] - '0');

            currentIndex++;
        }

        if (existingUserId == userData.id)
        {
            printf("User already exist");
            fclose(userFile);
            return;
        }
    }

    int inputCharacter;

    while ((inputCharacter = getchar()) != '\n' &&
           inputCharacter != EOF)
    {
    }

    printf("Enter Name: ");
    fgets(userData.name, sizeof(userData.name), stdin);

    printf("Enter Age: ");
    scanf("%d", &userData.age);

    if (userData.age <= 0)
    {
        printf("Invalid Age");
        fclose(userFile);
        return;
    }

    userData.name[strcspn(userData.name, "\n")] = '\0';

    fprintf(userFile, "%d|%s|%d\n", userData.id, userData.name, userData.age);

    fclose(userFile);

    printf("User Enterd Successfully");
}

void deleteUser()
{
    char recordLine[100];
    int userId;

    FILE *userFile = fopen("users.txt", "r");

    if (userFile == NULL)
    {
        printf("Unabale to open File");
        return;
    }

    FILE *tempFile = fopen("temp.txt", "w");

    if (tempFile == NULL)
    {
        printf("Unable to create Temprory File");
        fclose(userFile);
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &userId);

    rewind(userFile);

    int userFound = 0;

    while (fgets(recordLine, sizeof(recordLine), userFile))
    {
        int currentIndex = 0;
        int currentUserId = 0;

        while (recordLine[currentIndex] >= '0' &&
               recordLine[currentIndex] <= '9')
        {
            currentUserId =
                currentUserId * 10 + (recordLine[currentIndex] - '0');

            currentIndex++;
        }

        if (currentUserId == userId)
        {
            userFound = 1;
        }
        else
        {
            fputs(recordLine, tempFile);
        }
    }

    if (!userFound)
    {
        printf("Id does not exist");
        fclose(userFile);
        fclose(tempFile);
        return;
    }

    fclose(userFile);
    fclose(tempFile);

    if (remove("users.txt") != 0)
    {
        printf("Unable to remove users.txt\n");
        return;
    }

    if (rename("temp.txt", "users.txt") != 0)
    {
        printf("Rename failed.\n");
        return;
    }

    printf("User deleted Successfully");
}

void readUser()
{
    FILE *userFile = fopen("users.txt", "r");

    int userFound = 0;
    char recordLine[200];

    if (userFile == NULL)
    {
        printf("Unable to load file");
        return;
    }

    while (fgets(recordLine, sizeof(recordLine), userFile) != NULL)
    {
        printf("%s", recordLine);
        userFound = 1;
    }

    if (!userFound)
    {
        printf("No users Exist");
    }

    fclose(userFile);
}

void updateUser(void)
{
    char recordLine[200];
    int userId;
    int userFound = 0;

    struct user userData;

    FILE *userFile = fopen("users.txt", "r");

    if (userFile == NULL)
    {
        printf("Unable to open users.txt.\n");
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &userId);

    FILE *tempFile = fopen("temp.txt", "w");

    if (tempFile == NULL)
    {
        printf("Unable to create temporary file.\n");
        fclose(userFile);
        return;
    }

    while (fgets(recordLine, sizeof(recordLine), userFile) != NULL)
    {
        int currentIndex = 0;
        int currentUserId = 0;

        while (recordLine[currentIndex] >= '0' &&
               recordLine[currentIndex] <= '9')
        {
            currentUserId =
                currentUserId * 10 + (recordLine[currentIndex] - '0');

            currentIndex++;
        }

        if (currentIndex > 0 &&
            recordLine[currentIndex] == '|' &&
            currentUserId == userId)
        {
            userFound = 1;
            userData.id = userId;

            printf("Current record: %s", recordLine);

            printf("\nEnter new name: ");
            scanf(" %99[^\n]", userData.name);

            printf("Enter new age: ");
            scanf("%d", &userData.age);

            fprintf(tempFile, "%d|%s|%d\n", userData.id, userData.name, userData.age);
        }
        else
        {
            fputs(recordLine, tempFile);
        }
    }

    fclose(userFile);
    fclose(tempFile);

    if (!userFound)
    {
        remove("temp.txt");
        printf("ID does not exist.\n");
        return;
    }

    if (remove("users.txt") != 0)
    {
        perror("Unable to remove users.txt");
        return;
    }

    if (rename("temp.txt", "users.txt") != 0)
    {
        printf("Rename failed. Updated records are in temp.txt.\n");
        return;
    }

    printf("User updated successfully.\n");
}