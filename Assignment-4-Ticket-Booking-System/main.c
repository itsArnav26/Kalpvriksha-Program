# include <stdio.h>
# include <string.h>
# include <stdbool.h>

const int STANDARD_PRICE = 150;
const int PREMIUM_PRICE = 250;
const int RECLINER_PRICE = 400;

void showBookingDetails(char name[], char category[], int noOfTickets, float total, float discount, float bill);

void showBookingMenu(void);

bool isValidTicketCount(int noOfTickets);

bool isValidTicketCategory(int categoryChoice);

void getTicketPriceAndCategory(int categoryChoice, float *pricePerTicket, char category[]);

void calculateBillAndDiscount(float *discount, float *bill, float total, int noOfTickets);

int main(void)
{
  char name[100];
  int noOfTickets;
  int categoryChoice;
  char category[20];
  float pricePerTicket;
  float discount = 0;
  float total;
  float bill;

  showBookingMenu();

  printf("\nEnter your name: ");
  scanf("%s", name);
  printf("Enter number of Tickets (1 to 20):  ");
  scanf("%d", &noOfTickets);

  while (!isValidTicketCount(noOfTickets)) 
  {
    printf("Invalid number of tickets. Please enter a value between 1 and 20.\n");
    printf("Enter number of Tickets (1 to 20): ");
    scanf("%d", &noOfTickets);
  }

  printf("Enter Ticket Category (1/2/3): ");
  scanf("%d", &categoryChoice);

  while (!isValidTicketCategory(categoryChoice)) 
  {
    printf("Invalid ticket category. Please enter a value between 1 and 3.\n");
    printf("Enter Ticket Category (1/2/3): ");
    scanf("%d", &categoryChoice);
  }

  getTicketPriceAndCategory(categoryChoice, &pricePerTicket, category);

  total = pricePerTicket * noOfTickets;

  calculateBillAndDiscount(&discount, &bill, total, noOfTickets);

  showBookingDetails(name, category, noOfTickets, total, discount, bill);
  
  return 0;
}

void showBookingDetails(char name[], char category[], int noOfTickets, float total, float discount, float bill)
{
  printf("\n-----Ticket Booking Summary:-----\n\n");
  printf("Customer: %s\n", name);
  printf("Ticket Category: %s\n", category);
  printf("Tickets: %d\n", noOfTickets);
  printf("Subtotal: %.2f\n", total);
  printf("Discount: %.2f\n", discount);
  printf("Final Bill: %.2f\n", bill);
}

bool isValidTicketCount(int noOfTickets)
{
  return noOfTickets >= 1 && noOfTickets <= 20;
}

bool isValidTicketCategory(int categoryChoice)
{
  return categoryChoice >= 1 && categoryChoice <= 3;
}

void getTicketPriceAndCategory(int categoryChoice, float *pricePerTicket, char category[])
{
    switch(categoryChoice)
  {
    case 1:
      strcpy(category, "Standard");
      *pricePerTicket = STANDARD_PRICE;
      break;
    
    case 2:
      strcpy(category, "Premium");
      *pricePerTicket = PREMIUM_PRICE;
      break;
    
    case 3:
      strcpy(category, "Recliner");
      *pricePerTicket = RECLINER_PRICE;
      break;
  }
}

void calculateBillAndDiscount(float *discount, float *bill, float total, int noOfTickets)
{
   if (noOfTickets >= 5)
  {
    *discount = total * 0.1;
  }

  *bill = total - *discount;
}

void showBookingMenu(void)
{
  printf("\n\n-----Welcome to the Ticket Booking System-----\n\n");

  printf("Choice    Category   Price per Ticket\n");
  printf("1         Standard   150\n");
  printf("2         Premium    250\n");
  printf("3         Recliner   400\n");
}