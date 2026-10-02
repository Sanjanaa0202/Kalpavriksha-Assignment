#include <stdio.h>
#include <stdlib.h>

#define FILE_NAME "users.txt"

void operation(int choice);
void create_file();
void create_user();
void read_file();
void update_user();
void delete_user();

struct User {
    int id;
    char name[50];
    int age;
} user;

int main() {
    int choice;
    create_file();
    printf("CRUD Operations in a File\n");
    while (1)
    {
        printf("1. Create User\n");
        printf("2. Read User File\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        operation(choice);
    }
    
}

void operation(int choice) {
    switch (choice)
    {
    case 1:
        create_user();
        break;
    case 2:
        read_file();
        break;
    case 3:
        update_user();
        break;
    case 4:
        delete_user();
        break;
    case 5:
        printf("Terminated\n");
        exit(0);
    default:
        printf("Invalid Choice. Please Enter 1-5.\n");
        break;
    }
}

void create_file() {
    FILE *file;
    file = fopen(FILE_NAME, "a");
    if (file == NULL) {
        printf("Error: Unable to create file.\n");
        return;
    }
    fclose(file);
}

void create_user() {

    do {
        printf("Enter User ID: ");
        if (scanf("%d", &user.id) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            user.id = -1;
            continue;
        }
        if (user.id <= 0) {
            printf("User ID must be positive.\n");
        }
    } while (user.id <= 0);
    
    printf("Enter User Name: ");
    scanf("%49s", user.name);

    do {
        printf("Enter User Age: ");
        if (scanf("%d", &user.age) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            user.age = -1;
            continue;
        }
        if (user.age <= 0 || user.age > 120) {
            printf("User Age must be between 1 and 120.\n");
        }
    } while(user.age <= 0 || user.age > 120);

    FILE *file;
    file = fopen(FILE_NAME, "r");

    if (file == NULL) {
        printf("Error: Unable to open the file.\n");
        return;
    }

    struct User temp;
    while (fscanf(file, "%d %*s %*d", &temp.id) == 1) {
        if (temp.id == user.id) {
            printf("User ID already exists\n");
            fclose(file);
            return;
        }
    }

    fclose(file);

    file = fopen(FILE_NAME, "a");
    if (file == NULL) {
        printf("Error: Unable to open the file.\n");
        return;
    }

    fprintf(file, "%d\t%s\t%d\n", user.id, user.name, user.age);
    fclose(file);
    printf("User Added Successfully!\n");
}

void read_file() {
    FILE *file;
    file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        printf("Error: File does not exist\n");
        return;
    }

    int found = 0;

    printf("UserID \t Name \t Age\n");
    while (fscanf(file, "%d %49s %d", &user.id, user.name, &user.age) == 3) {
        found = 1;
        printf("%d\t%s\t%d\n", user.id, user.name, user.age);
    }
    fclose(file);
    if (!found) {
        printf("No records of the User\n");
    }
    else {
        printf("File has been read successfully\n");
    }
}

void update_user() {
    FILE *file, *temp;
    file = fopen(FILE_NAME, "r");
    temp = fopen("temp.txt", "w");

    if (file == NULL){
        printf("Error: File does not exist\n");
        if (temp != NULL) {
            fclose(temp);
        }
        return;
    }

    if (temp == NULL) {
        printf("Error: File Couldn't Open/Created\n");
        if (file != NULL) {
            fclose(file);
        }
        return;
    }

    int id, found = 0;

    do {
        printf("Enter User ID to Update: ");
        if (scanf("%d", &id) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            id = -1;
            continue;
        }
        if (id <= 0) {
            printf("User ID must be positive.\n");
        }
    } while (id <= 0);

    while (fscanf(file, "%d %49s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) {
            printf("Enter User Name: ");
            scanf("%49s", user.name);

            do {
                printf("Enter User Age: ");
                if (scanf("%d", &user.age) != 1) {
                    printf("Invalid input. Please enter a number.\n");
                    while (getchar() != '\n');
                    user.age = -1;
                    continue;
                }
                if (user.age <= 0 || user.age > 120) {
                    printf("User Age must be between 1 and 120.\n");
                }
            } while(user.age <= 0 || user.age > 120);

            found = 1;
        }
        fprintf(temp, "%d\t%s\t%d\n", user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    if (found) {
        printf("User details has been Updated\n");
        remove(FILE_NAME);
        rename("temp.txt", FILE_NAME);
    }
    else {
        remove("temp.txt");
        printf("User Id not found\n");
    }
}

void delete_user() {
    FILE *file, *temp;
    file = fopen(FILE_NAME, "r");
    temp = fopen("temp.txt", "w");

    if (file == NULL){
        printf("Error: File does not exist\n");
        if (temp != NULL) {
            fclose(temp);
        }
        return;
    }

    if (temp == NULL) {
        printf("Error: File Couldn't Open/Created\n");
        if (file != NULL) {
            fclose(file);
        }
        return;
    }

    int id, found = 0;

    do {
        printf("Enter User ID: ");
        if (scanf("%d", &id) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            id = -1;
            continue;
        }
        if (id <= 0) {
            printf("User ID must be positive.\n");
        }
    } while (id <= 0);

    while (fscanf(file, "%d %49s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) {
            found = 1;
            continue;
        }
        fprintf(temp, "%d\t%s\t%d\n", user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    if (found) {
        printf("User ID deleted Successfully\n");
        remove(FILE_NAME);
        rename("temp.txt", FILE_NAME);
    }
    else {
        printf("User ID not found\n");
        remove("temp.txt");
    }
}