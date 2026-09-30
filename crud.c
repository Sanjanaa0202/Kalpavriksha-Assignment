#include <stdio.h>
#include <stdlib.h>

void operation(int choice);
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
    printf("CRUD Operations in a File\n");
    while (1)
    {
        printf("1. Create User\n");
        printf("2. Read User File\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

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
        printf("Enter a Valid Choice\n");
        break;
    }
}

void create_user() {
    printf("Enter User ID: ");
    scanf("%d", &user.id);
    printf("Enter User Name: ");
    scanf("%49s", user.name);
    printf("Enter User Age: ");
    scanf("%d", &user.age);

    FILE *file;
    file = fopen("users.txt", "r");
    if (file != NULL) {
        struct User temp;
        while (fscanf(file, "%d %*s %*d", &temp.id) == 1) {
            if (temp.id == user.id) {
                printf("User ID already exists\n");
                fclose(file);
                return;
            }
        }
    }
    fclose(file);

    file = fopen("users.txt", "a");
    if (file == NULL) {
        printf("Error: File does not exist\n");
        return;
    }

    fprintf(file, "%d\t%s\t%d\n", user.id, user.name, user.age);
    fclose(file);
    printf("User Added Successfully!\n");
}

void read_file() {
    FILE *file;
    file = fopen("users.txt", "r");
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
    file = fopen("users.txt", "r");
    temp = fopen("temp.txt", "w");

    if (file == NULL){
        printf("Error: File does not exist\n");
        fclose(temp);
        return;
    }

    if (temp == NULL) {
        printf("Error: File Couldn't Open/Created\n");
        fclose(file);
        return;
    }

    int id, found = 0;
    printf("Enter the User ID to update: ");
    scanf("%d", &id);

    while (fscanf(file, "%d %49s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) {
            printf("Enter User Name: ");
            scanf("%49s", user.name);
            printf("Enter User Age: ");
            scanf("%d", &user.age);
            found = 1;
        }
        fprintf(temp, "%d\t%s\t%d\n", user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    if (found) {
        printf("User details has been Updated\n");
        remove("users.txt");
        rename("temp.txt", "users.txt");
    }
    else {
        remove("temp.txt");
        printf("User Id not found\n");
    }
}

void delete_user() {
    FILE *file, *temp;
    file = fopen("users.txt", "r");
    temp = fopen("temp.txt", "w");

    if (file == NULL){
        printf("Error: File does not exist\n");
        fclose(temp);
        return;
    }

    if (temp == NULL) {
        printf("Error: File Couldn't Open/Created\n");
        fclose(file);
        return;
    }

    int id, found = 0;
    printf("Enter User ID to delete: ");
    scanf("%d", &id);

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
        remove("users.txt");
        rename("temp.txt", "users.txt");
    }
    else {
        printf("User ID not found\n");
        remove("temp.txt");
    }
}
