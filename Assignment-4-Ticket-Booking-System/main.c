# include <stdio.h>
# include <string.h>


int main()
{
  char name[100];
  int noOfTickets;
  int categoryChoice;
  char category[20];
  float pricePerTicket;
  float discount = 0;
  float total;
  float bill;

  const int STANDARD_PRICE = 150;
  const int PREMIUM_PRICE = 250;
  const int RECLINER_PRICE = 400;

  printf("\n\n-----Welcome to the Ticket Booking System-----\n\n");

  printf("Choice    Catogery   Price per Ticket\n");
  printf("1         Standard   150\n");
  printf("2         Premium    250\n");
  printf("3         Recliner   400\n");
  
  printf("\nEnter your name: ");
  scanf("%s", name);
  printf("Enter number of Tickets (1 to 20):  ");
  scanf("%d", &noOfTickets);

  while (noOfTickets < 1 || noOfTickets > 20) 
  {
    printf("Invalid number of tickets. Please enter a value between 1 and 20.\n");
    printf("Enter number of Tickets (1 to 20): ");
    scanf("%d", &noOfTickets);
  }

  printf("Enter Ticket Category (1/2/3): ");
  scanf("%d", &categoryChoice);

  while (categoryChoice < 1 || categoryChoice > 3) 
  {
    printf("Invalid ticket category. Please enter a value between 1 and 3.\n");
    printf("Enter Ticket Category (1/2/3): ");
    scanf("%d", &categoryChoice);
  }

  switch(categoryChoice)
  {
    
    case 1:
      strcpy(category, "Standard");

      pricePerTicket = STANDARD_PRICE;
      break;
    
    case 2:
      strcpy(category, "Premium");
      pricePerTicket = PREMIUM_PRICE;
      break;
    
    case 3:
      strcpy(category, "Recliner");
      pricePerTicket = RECLINER_PRICE;
      break;
  }

  total = pricePerTicket * noOfTickets;
  bill = total;

  if (noOfTickets >= 5)
  {
    discount = total * 0.1;
    bill = total - discount;
  }

  printf("\n-----Ticket Booking Summary:-----\n\n");

  printf("Customer: %s\n", name);
  printf("Ticket Category: %s\n", category);
  printf("Tickets: %d\n", noOfTickets);
  printf("Subtotal: %.2f\n", total);
  printf("Discount: %.2f\n", discount);
  printf("Final Bill: %.2f\n", bill);

}