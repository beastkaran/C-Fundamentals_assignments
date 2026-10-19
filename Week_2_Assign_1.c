#include <stdio.h>
#include <string.h>

int main() {
    const int standardPrice=150;
    const int premiumPrice=250;
    const int reclinerPrice=400;

    char name[50];
    int tickets, choice;
    int price;
    float subtotal, discount, finalBill;
    char category[20];

    printf("Enter customer name: ");
    scanf("%s", name);

    printf("Enter number of tickets: ");
    scanf("%d", &tickets);

    printf("Enter ticket category (1-Standard, 2-Premium, 3-Recliner): ");
    scanf("%d", &choice);

    if (tickets<1 || tickets>20) {
        printf("Error: Invalid number of tickets.\n");
        return 0;
    }

    switch (choice) {
        case 1:
            price=standardPrice;
            strcpy(category, "Standard");
            break;
        case 2:
            price=premiumPrice;
            strcpy(category, "Premium");
            break;
        case 3:
            price=reclinerPrice;
            strcpy(category, "Recliner");
            break;
        default:
            printf("Error: Invalid ticket category.\n");
            return 0;
    }

    subtotal=tickets*price;

    if (tickets>=5) {
        discount=subtotal*0.10;
    } else {
        discount=0;
    }

    finalBill=subtotal-discount;

    printf("Customer: %s\n", name);
    printf("Ticket Category: %s\n", category);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalBill);

    return 0;
}