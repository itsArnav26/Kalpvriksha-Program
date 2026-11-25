# include <stdio.h>
# include <string.h>

struct city
{
  char source[500];
  char dest[500];

  struct city *next;
};

int main()
{
  int n;
  int input[500];

  printf("Enter the number of tickets: ");
  scanf("%d", &n);

  printf("Enter the string in format (source -> destination then space)");
  fgets(input, 500, stdin);
  input[strcspn(input, "\n")] = '\0';
  printf("%s", input);


  return 0;
}
