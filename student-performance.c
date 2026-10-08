#include <stdio.h>
#include <string.h>

int get_total_marks(int idx);
float get_average_marks(int total);
char assign_grade(float avg);
void performance_star(char grade);
void display_student_details(int n);
void list_of_roll_no(int n);
void validate_input_details(int i);

struct student_marks {
    int roll_no;
    char name[50];
    int marks[3];
};

struct  student_marks student[100];

int main() {
    int n;
    
    printf("Enter Number of Students: ");

    while (scanf("%d", &n) != 1 || n < 1 || n > 100) {
        printf("Invalid Input. Enter Number of students between 1 and 100\n");
        while(getchar() != '\n');
    }

    printf("Enter the roll number, name, mark1, mark2, mark3 of the student (using Spaces): \n");
    for (int i = 0; i < n; i++) {
        validate_input_details(i);
    }

    display_student_details(n);
    return 0;
    
}

void display_student_details(int n) {
    for (int i = 0; i < n; i++) {
        printf("\n");
        printf("Roll: %d\n", student[i].roll_no);
        printf("Name: %s\n", student[i].name);

        int total = get_total_marks(i);
        printf("Total: %d\n", total);

        float avg = get_average_marks(total);
        printf("Average: %.2f\n", avg);

        char grade = assign_grade(avg);
        printf("Grade: %c\n", grade);

        if (avg < 35) {
            continue;
        }
        performance_star(grade);
    }
    printf("\nList of Roll Numbers (via recursion): ");
    list_of_roll_no(n);
}

int get_total_marks(int idx) {
    int total = 0;
    for (int i = 0; i < 3; i++) {
        total += student[idx].marks[i];
    }
    return total;
}

float get_average_marks(int total) {
    float avg = total / 3.0;
    return avg;
}

char assign_grade(float avg) {
    if (avg >= 85) {
        return 'A';
    }
    else if (avg >= 70) {
        return 'B';
    }
    else if (avg >= 50) {
        return 'C';
    }
    else if (avg >= 35) {
        return 'D';
    }
    else {
        return 'F';
    }
}

void performance_star(char grade) {
    printf("Performance: ");
    switch (grade)
    {
    case 'A':
        printf("*****\n");
        break;

    case 'B':
        printf("****\n");
        break;

    case 'C':
        printf("***\n");
        break;

    case 'D':
        printf("**\n");
        break;
    
    default:
        break;
    }
}

void list_of_roll_no(int n) {
    if (n <= 0) {
        return;
    }
    list_of_roll_no(n-1);
    printf("%d ", student[n-1].roll_no);
}

void validate_input_details(int i) {
    do {
        if (scanf("%d %49s %d %d %d", 
            &student[i].roll_no, 
            student[i].name, 
            &student[i].marks[0], 
            &student[i].marks[1], 
            &student[i].marks[2]) != 5) {
            
            printf("Error: Invalid Input.\n");
            printf("Roll Number and marks must be a Number\n");

            while (getchar() != '\n');
            continue;
        }

        if (student[i].marks[0] < 0 || student[i].marks[0] > 100 ||
            student[i].marks[1] < 0 || student[i].marks[1] > 100 ||
            student[i].marks[2] < 0 || student[i].marks[2] > 100) {
            
                printf("Marks must be between 1 and 100\n");
                while (getchar() != '\n');
                continue;
        }
    } while (student[i].marks[0] < 0 || student[i].marks[0] > 100 ||
            student[i].marks[1] < 0 || student[i].marks[1] > 100 ||
            student[i].marks[2] < 0 || student[i].marks[2] > 100);

}