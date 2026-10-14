#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MIN_TICKET_COUNT 1
#define MAX_TICKET_COUNT 20
#define MIN_CATEGORY_COUNT 1
#define MAX_CATEGORY_COUNT 3

void validate_input();
void validate_input_name();
void validate_ticket_count_input();
void validate_category_choice_input();
void choose_ticket_category(int choice);
void calculate_ticket_cost();
float calculate_discount();
void generate_bill();

struct customer_details {
    char name[30];
    int no_of_tickets;
    char ticket_category[10];
    float price;
    float subtotal;
    float discount;
    float total_amount;
};

// Global Variables
struct customer_details customer; 
const int DISCOUNT_PERCENTAGE = 10;

int main() {
    validate_input();
    calculate_ticket_cost();
    generate_bill();
    return 0;
}

void generate_bill() {
    printf("\nCustomer Name: %s", customer.name);
    printf("Ticket Category: %s\n", customer.ticket_category);
    printf("Tickets: %d\n", customer.no_of_tickets);
    printf("Subtotal: Rs.%.2f\n", customer.subtotal);
    printf("Discount: %.2f\n", customer.discount);
    printf("Final Bill: Rs.%.2f\n", customer.total_amount);
}

float calculate_discount() {
    float discount;
    
    discount = (customer.subtotal * DISCOUNT_PERCENTAGE) / 100.0;

    return discount;
}

void calculate_ticket_cost() {
    // Calculate Subtotal
    customer.subtotal = customer.price * customer.no_of_tickets;

    // Calculate Discount
    if (customer.no_of_tickets >= 5) {
        customer.discount = calculate_discount();
    }
    else {
        customer.discount = 0.00;
    }

    // Final Bill
    customer.total_amount = customer.subtotal - customer.discount;
}

void choose_ticket_category(int choice) {
    switch (choice)
    {
    case 1:
        strcpy(customer.ticket_category, "Standard");
        customer.price = 150.00;
        break;
    
    case 2:
        strcpy(customer.ticket_category, "Premium");
        customer.price = 250.00;
        break;
    
    case 3:
        strcpy(customer.ticket_category, "Recliner");
        customer.price = 400.00;
        break;
    
    default:
        printf("Invalid Ticket Category.\n");
        printf("\nError: Invalid Choice. Ticket Category choice must be between %d and %d.\n", MIN_CATEGORY_COUNT, MAX_CATEGORY_COUNT);
        break;
    }
}

void validate_input_name() {
    int invalid = 1;
    do {
        printf("Enter your Name: ");
        if (fgets(customer.name, sizeof(customer.name), stdin) == NULL) {
            printf("\nError: Unable to read Name.\n");
            return;
        }
        
        for (int i = 0; customer.name[i] != '\0'; i++) {
            if (!isspace((unsigned char)customer.name[i])) {
                invalid = 0;
                break;
            }
        }

        if (invalid) {
            printf("\nError: Name must not be Empty\n");
        }
        
    } while(invalid);
}

void validate_ticket_count_input() {
    char ticket_count[10];
    int ch, invalid, isdecimal;
    do {
        invalid = 0;
        printf("\nDiscount of %d%% will be applied for 5 or more customers to the subtotal.\n", DISCOUNT_PERCENTAGE);
        printf("Enter Number of Tickets: ");
        if (fgets(ticket_count, sizeof(ticket_count), stdin) == NULL) {
            printf("\nError: Unable to read input\n");
            return;
        }

        if (ticket_count[0] == '\n' || ticket_count[0] == '\0') {
            printf("\nError: Ticket Count is required.\n");
            invalid = 1;
            continue;
        }

        if (sscanf(ticket_count, "%d %c", &customer.no_of_tickets, &isdecimal) != 1) {
            printf("\nError: Enter a valid Number.\n");
            invalid = 1;
            continue;
        }

        if (customer.no_of_tickets < MIN_TICKET_COUNT || customer.no_of_tickets > MAX_TICKET_COUNT) {
            printf("\nError: Invalid Ticket Count. \nEnter Ticket Count between %d and %d\n", MIN_TICKET_COUNT, MAX_TICKET_COUNT);
            invalid = 1;
            continue;
        }
    } while (invalid);
}

void validate_category_choice_input() {
    char ticket_category_choice[10];
    int ch, invalid, choice, isdecimal; // Local Variable
    do {
        invalid = 0;
        printf("\nTicket Category\n");
        printf(" 1. Standard - Rs. 150 \n 2. Premium - Rs. 250 \n 3. Recliner - Rs. 400 \n");
        printf("Choose the Ticket Category: ");
        if (fgets(ticket_category_choice, sizeof(ticket_category_choice), stdin) == NULL) {
            printf("\nError: Unable to read input\n");
            return;
        }

        if (ticket_category_choice[0] == '\n' || ticket_category_choice[0] == '\0') {
            printf("\nError: Ticket Category Choice is required.\n");
            invalid = 1;
            continue;
        }

        if (sscanf(ticket_category_choice, "%d %c", &choice, &isdecimal) != 1) {
            printf("\nError: Enter a valid Number.\n");
            invalid = 1;
            continue;
        }
        
        choose_ticket_category(choice);
        
    } while (invalid || choice < MIN_CATEGORY_COUNT || choice > MAX_CATEGORY_COUNT);
}

void validate_input() {
    // Validate Name Input
    validate_input_name();
    
    // Validate Ticket Count Input
    validate_ticket_count_input();

    // Validate Ticket Category Input
    validate_category_choice_input();

    if (customer.no_of_tickets >= 5) {
        printf("%d%% Discount will be applied to the subtotal\n", DISCOUNT_PERCENTAGE);
    }
    else {
        printf("No Discount will be applied to the subtotal\n");
    }
}