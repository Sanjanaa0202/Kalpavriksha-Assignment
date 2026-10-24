#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MIN_TICKET_COUNT 1
#define MAX_TICKET_COUNT 20
#define MIN_TICKETS_FOR_DISCOUNT 5

enum ticket_category {
    STANDARD = 1,
    PREMIUM,
    RECLINER
};

#define MIN_CATEGORY_COUNT STANDARD
#define MAX_CATEGORY_COUNT RECLINER

// Ticket prices declared as constants
const float STANDARD_PRICE = 150.00f;
const float PREMIUM_PRICE = 250.00f;
const float RECLINER_PRICE = 400.00f;

void validate_input();
void validate_input_name();
void validate_ticket_count_input();
void validate_category_choice_input();
void choose_ticket_category(enum ticket_category choice);
void calculate_ticket_cost();
float calculate_discount();
void generate_bill();
void print_ticket_category();

struct customer_details {
    char name[30];
    int no_of_tickets;
    enum ticket_category category;
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

void print_ticket_category() {
    const char *category_name = "Unknown";

    switch (customer.category) {
        case STANDARD:
            category_name = "Standard";
            break;

        case PREMIUM:
            category_name = "Premium";
            break;

        case RECLINER:
            category_name = "Recliner";
            break;

        default:
            break;
    }

    printf("Ticket Category: %s\n", category_name);
}

void generate_bill() {
    printf("\nCustomer Name: %s\n", customer.name);
    print_ticket_category();
    printf("Tickets: %d\n", customer.no_of_tickets);
    printf("Subtotal: Rs.%.2f\n", customer.subtotal);
    printf("Discount: %.2f\n", customer.discount);
    printf("Final Bill: Rs.%.2f\n", customer.total_amount);
}

float calculate_discount() {
    float discount;

    discount = (customer.subtotal * DISCOUNT_PERCENTAGE) / 100.0f;

    return discount;
}

void calculate_ticket_cost() {
    // Calculate Subtotal
    customer.subtotal = customer.price * customer.no_of_tickets;

    // Calculate Discount
    if (customer.no_of_tickets >= MIN_TICKETS_FOR_DISCOUNT) {
        customer.discount = calculate_discount();
    }
    else {
        customer.discount = 0.00f;
    }

    // Final Bill
    customer.total_amount = customer.subtotal - customer.discount;
}

void choose_ticket_category(enum ticket_category choice) {
    switch (choice) {
        case STANDARD:
            customer.category = STANDARD;
            customer.price = STANDARD_PRICE;
            break;

        case PREMIUM:
            customer.category = PREMIUM;
            customer.price = PREMIUM_PRICE;
            break;

        case RECLINER:
            customer.category = RECLINER;
            customer.price = RECLINER_PRICE;
            break;

        default:
            // Invalid choices are checked before calling this function.
            break;
    }
}

void validate_input_name() {
    int invalid;

    do {
        invalid = 0;

        printf("Enter your Name: ");

        if (fgets(customer.name, sizeof(customer.name), stdin) == NULL) {
            printf("\nError: Unable to read Name.\n");
            return;
        }

        // Remove the newline character, if present
        customer.name[strcspn(customer.name, "\n")] = '\0';

        // Reject empty names and names containing whitespace
        if (customer.name[0] == '\0') {
            invalid = 1;
        }
        else {
            for (int i = 0; customer.name[i] != '\0'; i++) {
                if (isspace((unsigned char)customer.name[i])) {
                    invalid = 1;
                    break;
                }
            }
        }

        if (invalid) {
            printf("\nError: Enter a single name without spaces.\n");
        }

    } while (invalid);
}

void validate_ticket_count_input() {
    char ticket_count[10];
    int invalid;
    char isdecimal;

    do {
        invalid = 0;

        printf("\nDiscount of %d%% will be applied for %d or more tickets.\n",
               DISCOUNT_PERCENTAGE, MIN_TICKETS_FOR_DISCOUNT);
        printf("Enter Number of Tickets: ");

        if (fgets(ticket_count, sizeof(ticket_count), stdin) == NULL) {
            printf("\nError: Unable to read input.\n");
            return;
        }

        if (ticket_count[0] == '\n' || ticket_count[0] == '\0') {
            printf("\nError: Ticket Count is required.\n");
            invalid = 1;
            continue;
        }

        if (sscanf(ticket_count, "%d %c",
                   &customer.no_of_tickets, &isdecimal) != 1) {
            printf("\nError: Enter a valid whole number.\n");
            invalid = 1;
            continue;
        }

        if (customer.no_of_tickets < MIN_TICKET_COUNT ||
            customer.no_of_tickets > MAX_TICKET_COUNT) {
            printf("\nError: Invalid Ticket Count.\n");
            printf("Enter Ticket Count between %d and %d.\n",
                   MIN_TICKET_COUNT, MAX_TICKET_COUNT);
            invalid = 1;
        }

    } while (invalid);
}

void validate_category_choice_input() {
    char ticket_category_choice[10];
    int invalid;
    int choice;
    char isdecimal;

    do {
        invalid = 0;

        printf("\nTicket Category\n");
        printf(" 1. Standard - Rs. %.2f\n", STANDARD_PRICE);
        printf(" 2. Premium  - Rs. %.2f\n", PREMIUM_PRICE);
        printf(" 3. Recliner - Rs. %.2f\n", RECLINER_PRICE);
        printf("Choose the Ticket Category: ");

        if (fgets(ticket_category_choice,
                  sizeof(ticket_category_choice), stdin) == NULL) {
            printf("\nError: Unable to read input.\n");
            return;
        }

        if (ticket_category_choice[0] == '\n' ||
            ticket_category_choice[0] == '\0') {
            printf("\nError: Ticket Category Choice is required.\n");
            invalid = 1;
            continue;
        }

        if (sscanf(ticket_category_choice, "%d %c",
                   &choice, &isdecimal) != 1) {
            printf("\nError: Enter a valid whole number.\n");
            invalid = 1;
            continue;
        }

        // Validate the range before calling choose_ticket_category()
        if (choice < MIN_CATEGORY_COUNT ||
            choice > MAX_CATEGORY_COUNT) {
            printf("\nError: Invalid Ticket Category.\n");
            printf("Choose a category between %d and %d.\n",
                   MIN_CATEGORY_COUNT, MAX_CATEGORY_COUNT);
            invalid = 1;
            continue;
        }

        choose_ticket_category((enum ticket_category)choice);

    } while (invalid);
}

void validate_input() {
    // Validate Name Input
    validate_input_name();

    // Validate Ticket Count Input
    validate_ticket_count_input();

    // Validate Ticket Category Input
    validate_category_choice_input();

    if (customer.no_of_tickets >= MIN_TICKETS_FOR_DISCOUNT) {
        printf("%d%% discount will be applied to the subtotal.\n",
               DISCOUNT_PERCENTAGE);
    }
    else {
        printf("No discount will be applied to the subtotal.\n");
    }
}