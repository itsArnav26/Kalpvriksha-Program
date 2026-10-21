#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

struct Employee
{
  int EmployeeID;
  char name[100];
  char Designation[100];

  struct Employee *firstReport;
  struct Employee *nextReport;
  struct Employee *managerReport;
};

struct Employee *root = NULL;

struct Employee *getLastReport(struct Employee *employee);
struct Employee *isEmployeeExist(int EmployeeID, struct Employee *employee);
struct Employee *managerReplacement(struct Employee *employee, struct Employee *replacedBy);
void add_employee();
void displayEmployeeHierarchy();
void displayCompleteHierarchy();
void traverseCompleteHierarchy(struct Employee *employee, int level);
void removeEmployee();
int countTotalEmployee(struct Employee *employee);
int countDirectReports(struct Employee *employee);
void removeFromManager(struct Employee *employee);
void updateReportingManager(struct Employee *employee, struct Employee *newManager);

int main(void)
{
  int choice;

  do
  {
    system("cls");
    printf("----- EMPLOYEE HIERARCHY MANAGEMENT -----\n\n");
    printf("1. Add Employee\n");
    printf("2. Remove Employee\n");
    printf("3. Display Employee Hierarchy\n");
    printf("4. Display Complete Hierarchy\n");
    printf("5. exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch(choice)
  {
    case 1:
      add_employee();
      break;
    
    case 2:
      removeEmployee();
      break;
    
    case 3:
      displayEmployeeHierarchy();
      break;
    
    case 4:
      displayCompleteHierarchy();
      break;
    
    case 5:
      return 0;

    default:
      printf("Enter Valid Choice\n");
      break;
    }
    if (choice != 5)
  {
    printf("\nPress Enter to continue....");
    getchar();
    getchar();
  }

  } while (choice != 5);

  return 0;
}

void add_employee()
{
  struct Employee *newEmployee =  malloc(sizeof(struct Employee));

  newEmployee->firstReport = NULL;
  newEmployee->managerReport = NULL;
  newEmployee->nextReport = NULL;

  int managerID;

  printf("Enter EmployeeId: ");
  scanf("%d", &newEmployee->EmployeeID);

  if (isEmployeeExist(newEmployee->EmployeeID, root) != NULL)
  {
    printf("Employee already Exist\n");
    free(newEmployee);
    return;
  }

  printf("Enter Your Name: ");
  scanf("%s", newEmployee->name);

  printf("Enter Designation: ");
  scanf(" %s", newEmployee->Designation);

  printf("Enter id of Reporting Manager (0 for no Manager): "); // if zero that means the Employee is of top level
  scanf(" %d", &managerID);

  if (managerID == newEmployee->EmployeeID)
  {
    printf("Cannot add this Employee due to self reporting\n");
    free(newEmployee);
    return;
  }

  if (managerID == 0)
  {
    if (root == NULL)
    {
        root = newEmployee;
    }
    else
    {
        struct Employee *lastRoot = getLastReport(root);
        lastRoot->nextReport = newEmployee;
    }
 }

  else
  {
    struct Employee *managerNode = isEmployeeExist(managerID, root);

    if (managerNode == NULL)
    {
      printf("Cannot add this Employee beacause Manager does not Exist\n");
      free(newEmployee);
      return;
    }

    else
    {
      newEmployee->managerReport = managerNode;

      if(managerNode->firstReport == NULL)
      {
        managerNode->firstReport = newEmployee;
      }

      else
      {
        struct Employee *nextNode = getLastReport(managerNode->firstReport);
        nextNode->nextReport = newEmployee;
      }
    }
  }

  printf("Employee Added Successfully \n");
}

struct Employee *getLastReport(struct Employee *employee)
{
  while(employee->nextReport != NULL)
  {
    employee = employee->nextReport;
  }
  return employee;
}

struct Employee *isEmployeeExist(int EmployeeID, struct Employee *employee)
{
  if (employee == NULL || employee->EmployeeID == EmployeeID)
  {
    return employee;
  }

  struct Employee *found = isEmployeeExist(EmployeeID, employee->firstReport);
  if (found != NULL)
  {
    return found;
  }

   return isEmployeeExist(EmployeeID, employee->nextReport);
}

void displayEmployeeHierarchy()
{
  int employeeID;
  printf("Enter Employee ID: ");
  scanf("%d", &employeeID);

  struct Employee *employee = isEmployeeExist(employeeID, root);

  if (employee == NULL)
  {
    printf("\nEmployee does not Exist\n");
    return;
  }
  printf("%d - %s - %s\n", employee->EmployeeID, employee->name, employee->Designation);

  traverseCompleteHierarchy(employee->firstReport, 1);

  printf("\nTotal Employees Under %d : %d\n", employee->EmployeeID, countTotalEmployee(employee->firstReport));
}

void traverseCompleteHierarchy(struct Employee *employee, int level)
{
  if (employee == NULL)
  {
    return;
  }

  for (int i = 0; i < level; i++)
  {
    printf("  ");
  }

  printf("%d - %s - %s\n", employee->EmployeeID, employee->name, employee->Designation);

  traverseCompleteHierarchy(employee->firstReport, level + 1);
  traverseCompleteHierarchy(employee->nextReport, level);
}

int countTotalEmployee(struct Employee *employee)
{
  if (employee == NULL)
  {
    return 0;
  }
  return 1 + countTotalEmployee(employee->firstReport) + countTotalEmployee(employee->nextReport);
}

void displayCompleteHierarchy()
{
  if (root == NULL)
  {
    printf("No Highest Hierarchy till now\n");
    return;
  }
  traverseCompleteHierarchy(root, 0);

  printf("Total Employees : %d", countTotalEmployee(root));
}

void removeEmployee()
{
  int employeeID;
  printf("Enter Employee ID: ");
  scanf("%d", &employeeID);
  struct Employee *employee = isEmployeeExist(employeeID, root);

  if (employee == NULL)
  {
    printf("Employee does not exist\n");
    return;
  }

  if (employee->managerReport == NULL)
  {
    printf("Cannot Delete Top-Level Employee\n");
    return;
  }

  struct Employee *manager = employee->managerReport;
  struct Employee *replacement = managerReplacement(employee, manager->firstReport);

  if (replacement == NULL)
  {
    replacement = manager;
  }

  if (replacement->firstReport == NULL)
  {
    replacement->firstReport = employee->firstReport;
  }

  else
  {
    struct Employee *lastReport = getLastReport(replacement->firstReport);
    lastReport->nextReport = employee->firstReport;
  }

  updateReportingManager(employee->firstReport, replacement);

  removeFromManager(employee);

  printf("Employee Deleted Successfully\n");
}

struct Employee *managerReplacement(struct Employee *employee, struct Employee *replacedBy)
{

  int minDirectReports = INT_MAX;
  struct Employee *bestReplacemnet = NULL;

  while(replacedBy != NULL)
  {
    if (replacedBy == employee)
    {
      replacedBy = replacedBy->nextReport;
      continue;
    }

    int directReports = countDirectReports(replacedBy->firstReport);

    if (directReports < minDirectReports)
    {
      bestReplacemnet = replacedBy;
      minDirectReports = directReports;
    }
    replacedBy = replacedBy->nextReport;
  }
  return bestReplacemnet;
}

int countDirectReports(struct Employee *employee)
{
  if (employee == NULL)
  {
    return 0;
  }
  return 1 + countDirectReports(employee->nextReport);
}

void removeFromManager(struct Employee *employee)
{
  struct Employee *manager = employee->managerReport;
  struct Employee *current = manager->firstReport;
  struct Employee *prev = NULL;

  while(current != NULL)
  {
    if (current == employee)
    {
      if (prev == NULL)
      {
        manager->firstReport = current->nextReport;
        
      }
      else
      {
        prev->nextReport = current->nextReport;
      }
      current->nextReport = NULL;
      current->firstReport = NULL;

      free(current);
      return;
    }
    
    prev = current;
    current = current->nextReport;
  }
}

void updateReportingManager(struct Employee *employee, struct Employee *newManager)
{
  while (employee != NULL)
  {
    employee->managerReport = newManager;
    employee = employee->nextReport;
  }
}