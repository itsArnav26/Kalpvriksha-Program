#include <stdio.h>

struct Student 
{
    int rollNo;
    char name[100];
    int marks1;
    int marks2;
    int marks3;
};

int calculateTotal(struct Student student);
float calculateAverage(int total);
char calculateGrade(float average);
void printRollNumber(int noOfStudents, int index, struct Student students[]);

int main() 
{

    printf("Enter the number of students: ");
    int noOfStudents;

    scanf("%d", &noOfStudents);
    if (noOfStudents <= 0 || noOfStudents > 100) {
        printf("Invalid number of students. Please enter a number between 1 and 100.\n");
        return 0;
    }
    struct Student students[noOfStudents];
    printf("Enter the details of %d students (rollNo name marks1 marks2 marks3):\n", noOfStudents);

    for (int i = 0; i < noOfStudents; i++) {
        
      scanf("%d %s %d %d %d", &students[i].rollNo, students[i].name, &students[i].marks1, &students[i].marks2, &students[i].marks3);

      while (students[i].marks1 < 0 || students[i].marks1 > 100 || students[i].marks2 < 0 || students[i].marks2 > 100 || students[i].marks3 < 0 || students[i].marks3 > 100) {

        printf("Invalid marks. Enter marks between 0 and 100.\n");

        scanf("%d %s %d %d %d", &students[i].rollNo, students[i].name, &students[i].marks1, &students[i].marks2, &students[i].marks3);
        }
    }
    printf("\nStudent Details:\n\n");
    for (int i = 0; i < noOfStudents; i++){

      int total = calculateTotal(students[i]);
      float average = calculateAverage(total);
      char grade = calculateGrade(average);

      printf("Roll: %d\n", students[i].rollNo);
      printf("Name: %s\n", students[i].name);
      printf("Total: %d\n", total);
      printf("Average : %.2f\n", average);
      printf("Grade: %c\n", grade);
      if (average < 35){
        continue;
      }
     
      int noOfStars = 0;
        
      if (grade == 'A'){
        noOfStars = 5;
      }
      else if (grade == 'B'){
        noOfStars = 4;
      }
      else if (grade == 'C'){
        noOfStars = 3;
      }
      else if (grade == 'D'){
        noOfStars = 2;
      }
      printf("Performance: ");
      for (int i = 0; i < noOfStars; i++){
        printf("*");
      }
      printf("\n\n");
    }
    printf("\n");
    printf("List of Roll Numbers : ");
    printRollNumber(noOfStudents, 0, students);
    return 0;
}
int calculateTotal(struct Student student)
{
  return student.marks1 + student.marks2 + student.marks3;
}

float calculateAverage(int total)
{
  return (float)total / 3;
}

char calculateGrade(float average)
{
  if (average >= 85) {
    return 'A';
  } else if (average >= 70) {
    return 'B';
  } else if (average >= 50) {
    return 'C';
  } else if (average >= 35) {
    return 'D';
  } else {
    return 'F';
  }
}

void printRollNumber(int noOfStudents, int index, struct Student students[])
{
  if (index == noOfStudents){
    return;
  }
  printf("%d ", students[index].rollNo);
  printRollNumber(noOfStudents, index+1, students);
}