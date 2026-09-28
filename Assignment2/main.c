#include <stdio.h>

struct user{
  int id;
  char name[100];
  int age;
};
int main(){
  char ch;
  printf("CHOOSE\n");
  printf("Create User : 1\n");
  printf("Read File : 2\n");
  printf("Update User : 3\n");
  printf("Delete User : 4\n");
  printf("Enter Your Choise : ");
  scanf("%c", &ch);
  switch (ch)
  {
  case '1':
    
  default:
    break;
  }
  return 0;
}