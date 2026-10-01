#include <stdio.h>
#include <string.h>

struct user {
    int id;
    char name[100];
    int age;
};
void createUser();
void readUser();
void deleteUser();
void updateUser();

int main(){
  char ch;
  do {
        printf("\n------- CHOOSE -------\n");
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        printf("\n\nEnter Your Choice : ");
        scanf(" %c", &ch);

        switch (ch)
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
        if (ch != '5'){
          char choice;
          printf("\nDo you want to continue .....\n");
          printf("Enter Your choice Y/N: ");
          scanf(" %c", &choice);

          if (choice == 'N' || choice == 'n'){
            return 0;
          }
        }
  } while (ch != '5');
  return 0;
}

void createUser(){
  char line[200];
  FILE *fp = fopen("users.txt", "a+");
  struct user u;

  if (fp == NULL){
    printf("Unable to open file");
    return;
  }

  printf("Enter ID: ");
  scanf("%d", &u.id);
  rewind(fp);
  while(fgets(line, sizeof(line), fp) != NULL){
    int i = 0;
    int exisiting_id = 0;
    while(line[i] >= '0' && line[i] <= '9'){
      exisiting_id = exisiting_id * 10 + (line[i] - '0');
      i++;
    }
    if (exisiting_id == u.id){
      printf("User already exist");
      fclose(fp);
      return;
    }
  }
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF) {
}
  printf("Enter Name: ");
  fgets(u.name, sizeof(u.name), stdin);
  printf("Enter Age: ");
  scanf("%d", &u.age);
  if (u.age <= 0){
    printf("Invalid Age");
    fclose(fp);
    return;
  }
  u.name[strcspn(u.name, "\n")] = '\0';
  fprintf(fp, "%d|%s|%d\n", u.id, u.name, u.age);
  fclose(fp);
  printf("User Enterd Successfully");
}



void deleteUser(){
  char line[100];
  int id;
  FILE *fp = fopen("users.txt", "r");
  if (fp == NULL){
    printf("Unabale to open File");
    return;
  }
  FILE *temp = fopen("temp.txt", "w");
  if (temp == NULL){
    printf("Unable to create Temprory File");
    fclose(fp);
    return;
  }

  printf("Enter ID: ");
  scanf("%d", &id);
  rewind(fp);
  int found = 0;
  while(fgets(line, sizeof(line), fp)){
    int i = 0;
    int user_id = 0;

    while(line[i] >= '0' && line[i] <= '9'){
      user_id = user_id * 10 + (line[i] - '0');
      i++;
    }
    if (user_id == id){
      found = 1;
    }
    else{
      fputs(line, temp);
    }
  }
  if (!found){
    printf("Id does not exist");
    fclose(fp);
    fclose(temp);
    return;
  }
  fclose(fp);
  fclose(temp);
  if (remove("users.txt") != 0) {
    printf("Unable to remove users.txt\n");
    return;
  }

  if (rename("temp.txt", "users.txt") != 0) {
    printf("Rename failed.\n");
    return;
  }

  printf("User deleted Successfully");
  
}


void readUser(){
  FILE *fp = fopen("users.txt", "r");
  int found = 0;
  char line[200];
  if (fp == NULL){
    printf("Unable to load file");
    return;
  }
  while(fgets(line, sizeof(line), fp) != NULL){
    printf("%s", line);
    found = 1;
  }
  if (!found){
    printf("No users Exist");
  }
  fclose(fp);
}

void updateUser(void)
{
    char line[200];
    int id;
    int found = 0;
    struct user u;

    FILE *fp = fopen("users.txt", "r");

    if (fp == NULL) {
        printf("Unable to open users.txt.\n");
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &id);

    FILE *temp = fopen("temp.txt", "w");

    if (temp == NULL) {
        printf("Unable to create temporary file.\n");
        fclose(fp);
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        int i = 0;
        int user_id = 0;

        // Extract the ID using your approach.
        while (line[i] >= '0' && line[i] <= '9') {
            user_id = user_id * 10 + (line[i] - '0');
            i++;
        }

        if (i > 0 && line[i] == '|' && user_id == id) {
            found = 1;
            u.id = id;

            printf("Current record: %s", line);

            printf("\nEnter new name: ");
            scanf(" %99[^\n]", u.name);

            printf("Enter new age: ");

            scanf("%d", &u.age);

            fprintf(temp, "%d|%s|%d\n", u.id, u.name, u.age);
        } else {
            fputs(line, temp);
        }
    }


   fclose(fp);
   fclose(temp);


    if (!found) {
        remove("temp.txt");
        printf("ID does not exist.\n");
        return;
    }

    if (remove("users.txt") != 0) {
        perror("Unable to remove users.txt");
        return;
    }

    if (rename("temp.txt", "users.txt") != 0) {
        printf("Rename failed. Updated records are in temp.txt.\n");
        return;
    }

    printf("User updated successfully.\n");
}