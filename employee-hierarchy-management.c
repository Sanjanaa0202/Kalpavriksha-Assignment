#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct employee_details employee;
typedef struct report_node reports;

struct employee_details {
    int emp_id;
    char name[30];
    char designation[30];
    reports *employee_reports;
    employee *reporting_manager;
    int report_count;
};

struct report_node {
    employee *reported_employee;
    struct report_node *next;
};

employee *ceo = NULL;

void operation(int choice);
void clear_input_buffer();

void add_employee();
void validate_employee_ID(employee *new_employee);
void validate_employee_name(employee *new_employee);
void validate_employee_designation(employee *new_employee); 
void validate_reporting_manager(employee *new_employee);

int find_employee_id(int id);
employee *find_employee(employee *person, int id);

void remove_employee();
employee *find_destination(employee *person);
void transfer_reports(employee *person, employee *destination);

void display_employee_hierarchy();
void display_complete_hierarchy();
int display_recursive(employee *person, int level);

void free_memory(employee *person);

int main() {
    int choice;
    while (1) {
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("\n1. Add Employee\n");       
        printf("2. Remove Employee\n");   
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("\nEnter your Choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }

        operation(choice);

    }
    return 0;
}

void operation(int choice) {
    switch (choice)
    {
    case 1:
        add_employee();
        break;
    case 2:
        remove_employee();
        break;
    case 3:
        display_employee_hierarchy();
        break;
    case 4:
        display_complete_hierarchy();
        break;
    case 5:
        free_memory(ceo);
        ceo = NULL;
        printf("Memory is freed. Exiting program.\n");
        printf("\n=== TERMINATED ===\n");
        exit(0);
    default:
        printf("Invalid Choice. Please choose number between 1-5.\n");
        break;
    }
}

int display_recursive(employee *person, int level) {
    if (person == NULL) {
        return 0;
    }

    for (int i = 0; i < level; i++) {
        printf("    ");
    }

    printf("%d - %s - %s\n", person->emp_id, person->name, person->designation);
    int count = 1;
    reports *current = person->employee_reports;

    while (current != NULL) {
        count += display_recursive(current->reported_employee, level+1);
        current = current->next;
    }

    return count;
    
}

void display_employee_hierarchy() {
    if (ceo == NULL) {
        printf("No Employees found.\n");
        return;
    }

    int id;
    printf("Enter Employee ID to dispaly their descendants: ");

    if (scanf("%d", &id) != 1) {
        printf("Invalid ID.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    employee *person = find_employee(ceo, id);

    if (person == NULL) {
        printf("Employee not Found.\n");
        return;
    }

    printf("\n===== EMPLOYEE HIERARCHY =====\n");
    int total = display_recursive(person, 0);
    printf("Total Employees under %d: %d\n", person->emp_id, total-1);
}

void display_complete_hierarchy() {
    if (ceo == NULL) {
        printf("No employees found.\n");
        return;
    }

    printf("\n===== ORGANIZATION HIERARCHY =====\n");
    int total = display_recursive(ceo, 0);
    printf("Total employees: %d\n", total);
}

employee *find_employee(employee *person, int id) {
    if (person == NULL) {
        return NULL;
    }

    if(person->emp_id == id) {
        return person;
    }

    reports *current = person->employee_reports;

    while (current != NULL) {
        employee *found = find_employee(current->reported_employee, id);
        if (found != NULL) {
            return found;
        }

        current = current->next;
    }
    return NULL;
}

int find_employee_id(int id) {
    return find_employee(ceo, id) != NULL;
}

void validate_employee_ID(employee *new_employee) {
    while (1) {
        printf("Enter Employee's id: ");

        if (scanf("%d", &new_employee->emp_id) != 1) {
            printf("Invalid ID. Employee ID must be a number.\n");
            clear_input_buffer();
            continue;
        }

        clear_input_buffer();

        if (find_employee_id(new_employee->emp_id)) {
            printf("Employee ID must be Unique.\n");
            clear_input_buffer();
            continue;
        }

        break;
    } 
}

void validate_employee_name(employee *new_employee) {
    int invalid = 1;
    do {
        printf("Enter Employee's Name: ");
        if (fgets(new_employee->name, sizeof(new_employee->name), stdin) == NULL) {
            printf("\nError: Unable to read Name.\n");
            return;
        }
        
        for (int i = 0; new_employee->name[i] != '\0'; i++) {
            if (!isspace((unsigned char)new_employee->name[i])) {
                invalid = 0;
                break;
            }
        }

        if (invalid) {
            printf("\nError: Name must not be Empty\n");
        }
        
    } while(invalid);
    new_employee->name[strcspn(new_employee->name, "\n")] = '\0';
}

void validate_employee_designation(employee *new_employee) {
    int invalid = 1;
    do {
        printf("Enter Employee's Designation: ");
        if (fgets(new_employee->designation, sizeof(new_employee->designation), stdin) == NULL) {
            printf("\nError: Unable to read Name.\n");
            return;
        }
        
        for (int i = 0; new_employee->designation[i] != '\0'; i++) {
            if (!isspace((unsigned char)new_employee->designation[i])) {
                invalid = 0;
                break;
            }
        }

        if (invalid) {
            printf("\nError: Name must not be Empty\n");
        }
        
    } while(invalid);

    new_employee->designation[strcspn(new_employee->designation, "\n")] = '\0';

    char designation[30];
    strcpy(designation, new_employee->designation);

    int start = 0;
    int end = strlen(designation) - 1;

    // Remove Whitespaces
    while (isspace((unsigned char)designation[start])) {
        start++;
    }
    while (end >= start && isspace((unsigned char)designation[end])) {
        end--;
    }

    int i = 0;
    while (start <= end) {
        designation[i++] = (char)toupper((unsigned char)designation[start++]);
    }
    designation[i] = '\0';

    if (strcmp(designation, "CEO") == 0) {
        strcpy(new_employee->designation, designation);
    }
}

void validate_reporting_manager(employee *new_employee) {
    int manager_id;
    printf("Enter Reporting Manager ID: ");

    if (scanf("%d", &manager_id) != 1) {
        printf("Invalid manager ID.\n");
        clear_input_buffer();
        free(new_employee);
        return;
    }

    clear_input_buffer();

    employee *manager = find_employee(ceo, manager_id);
    if (manager == NULL) {
        printf("Reporting manager not found.\n");
        free(new_employee);
        return;
    }

    reports *new_report = malloc(sizeof(reports));

    if (new_report == NULL) {
        printf("Memory allocation failed.\n");
        free(new_employee);
        return;
    }

    new_report->reported_employee = new_employee;
    new_report->next = NULL;

    if (manager->employee_reports == NULL) {
        manager->employee_reports = new_report;
    }
    else {
        reports *current = manager->employee_reports;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_report;
    }

    new_employee->reporting_manager = manager;
    manager->report_count++;
    
    printf("Employee added successfully.\n");
}

void add_employee() {
    // Create local variable for employee 
    employee *new_employee = malloc(sizeof(employee));

    if (new_employee == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    validate_employee_ID(new_employee);
    validate_employee_name(new_employee);
    validate_employee_designation(new_employee);

    new_employee->employee_reports = NULL;
    new_employee->reporting_manager = NULL;
    new_employee->report_count = 0;

    if (ceo == NULL) {
        if(strcmp(new_employee->designation, "CEO") != 0) {
            printf("Employee cannot be added without adding CEO\n");
            free(new_employee);
            return;
        }

        ceo = new_employee;
        printf("CEO added successfully.\n");
        return;
    }

    if (strcmp(new_employee->designation, "CEO") == 0) {
        printf("CEO already exists and cannot be updated.\n");
        free(new_employee);
        return;
    }

    // Both Reports and Repoting Manager validation
    validate_reporting_manager(new_employee);
}

employee *find_destination(employee *person) {
    employee *manager = person->reporting_manager;
    employee *destination = NULL;

    reports *current = manager-> employee_reports;

    while (current != NULL) {
        employee *new_manager = current->reported_employee;

        if (new_manager != person && (destination == NULL
        || new_manager->report_count < destination->report_count)) {
            destination = new_manager;
        }

        current = current->next;
    }

    if (destination == NULL) {
        destination = manager;
    }

    return destination;
}

void transfer_reports(employee *person, employee *destination) {
    reports *report = person->employee_reports;

    while (report != NULL) {
        reports *next_report = report->next;
        report->reported_employee->reporting_manager = destination;
        report->next = NULL;

        if (destination->employee_reports == NULL) {
            destination->employee_reports = report;
        }
        else {
            reports *last = destination->employee_reports;

            while (last->next != NULL) {
                last = last->next;
            }

            last->next = report;
        }
        destination->report_count++;
        report = next_report;
    }
    person->employee_reports = NULL;
    person->report_count = 0;
}

void remove_employee() {
    if (ceo == NULL) {
        printf("No employees to remove.\n");
        return;
    }

    int id;
    printf("Enter Employee ID to remove: ");

    if (scanf("%d", &id) != 1) {
        printf("Invalid ID.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    employee *person = find_employee(ceo, id);

    if (person == NULL) {
        printf("Employee not found.\n");
        return;
    }

    if (person == ceo) {
        printf("CEO cannot be removed.\n");
        return;
    }

    employee *destination = find_destination(person);
    transfer_reports(person, destination);

    employee *manager = person->reporting_manager;
    reports *previous = NULL;
    reports *current = manager->employee_reports;

    while(current != NULL && current->reported_employee != person) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Employee link not found.\n");
        return;
    }

    if (previous == NULL) {
        manager->employee_reports = current->next;
    }
    else {
        previous->next = current->next;
    }
    manager->report_count--;
    free(current);
    free(person);

    printf("Employee removed successfully.\n");
}

void free_memory(employee *person) {
    if (person == NULL) {
        return;
    }
    reports *current = person->employee_reports;

    while (current != NULL) {
        reports *next = current->next;
        free_memory(current->reported_employee);
        free(current);
        current = next;
    }
    free(person);
}

void clear_input_buffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}