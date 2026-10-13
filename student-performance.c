#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#define MAX_STUDENT 100
#define MAX_MARK 100
#define MIN_MARK 0
#define MAX_LINE 512

int get_total_marks(int idx);
float get_average_marks(int total);
char assign_grade(float avg);
void performance_star(char grade);
void display_student_details(int n);
void list_of_roll_no(int n);
void validate_input_details(int i);
void calculate_student_results(int idx);
int is_roll_no_exists(int roll_no, int count);
int is_valid_integer(const char *text, int *value);

struct student_marks {
    int roll_no;
    char name[50];
    int marks[3];
    int total;
    float avg;
    char grade;
};

struct  student_marks student[MAX_STUDENT];

int main() {
    int n;
    int ch;
    
    printf("Enter Number of Students: ");

    while (scanf("%d", &n) != 1 || n < 1 || n > MAX_STUDENT) {
        printf("Invalid Input. Enter Number of students between 1 and %d\n", MAX_STUDENT);
        while(ch = getchar() != '\n' && ch != EOF);
    }

    while ((ch = getchar()) != '\n' && ch != EOF);

    printf("Enter the roll number, name, mark1, mark2, mark3 of the student (using Spaces): \n");
    for (int i = 0; i < n; i++) {
        validate_input_details(i);
        calculate_student_results(i);
    }

    display_student_details(n);
    return 0;
    
}

int is_valid_integer(const char *text, int *value) {
    char *end;
    long number;

    errno = 0;
    number = strtol(text, &end, 10);

    if (text == end || *end != '\0' || errno == ERANGE || number < INT_MIN || number > INT_MAX) {
        return 0;
    }

    *value = (int)number;
    return 1;
}

int is_roll_no_exists(int roll_no, int count) {
    for (int i = 0; i < count; i++) {
        if (student[i].roll_no == roll_no) {
            return 1;
        }
    }
    return 0;
}

void display_student_details(int n) {
    for (int i = 0; i < n; i++) {
        printf("\n");
        printf("Roll: %d\n", student[i].roll_no);
        printf("Name: %s\n", student[i].name);
        printf("Total: %d\n", student[i].total);
        printf("Average: %.2f\n", student[i].avg);
        printf("Grade: %c\n", student[i].grade);

        if (student[i].avg < 35) {
            continue;
        }
        performance_star(student[i].grade);
    }
    printf("\nList of Roll Numbers (via recursion): ");
    list_of_roll_no(n);
    printf("\n");
}

void calculate_student_results(int idx) {
    student[idx].total = get_total_marks(idx);

    student[idx].avg = get_average_marks(student[idx].total);

    student[idx].grade = assign_grade(student[idx].avg);
}

int get_total_marks(int idx) {
    int total = 0;
    for (int i = 0; i < 3; i++) {
        total += student[idx].marks[i];
    }
    return total;
}

float get_average_marks(int total) {
    float avg = total / 3.0f;
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
    char line[MAX_LINE];
    char *tokens[100];
    int count;
    int roll_no, marks[3];
    int too_many_tokens;
    char name[50];

    while (1) {
        printf("Enter details for student %d: ", i + 1);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nError: Unable to read input.\n");
            exit(EXIT_FAILURE);
        }

        if (strchr(line, '\n') == NULL && !feof(stdin)) {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF);

            printf("Error: Input line is too long.\n");
            continue;
        }

        count = 0;
        too_many_tokens = 0;

        char *token = strtok(line, " \t\r\n");

        while (token != NULL) {
            if (count == 100) {
                too_many_tokens = 1;
                break;
            }

            tokens[count++] = token;
            token = strtok(NULL, " \t\r\n");
        }

        if (too_many_tokens) {
            printf("Error: Too many input fields.\n");
            continue;
        }

        if (count < 5) {
            printf("Error: Missing input fields.\n");
            continue;
        }

        if (!is_valid_integer(tokens[0], &roll_no) ||
            roll_no <= 0) {
            printf("Error: Roll number must be a positive integer.\n");
            continue;
        }

        if (!is_valid_integer(tokens[count - 3], &marks[0]) ||
            !is_valid_integer(tokens[count - 2], &marks[1]) ||
            !is_valid_integer(tokens[count - 1], &marks[2])) {
            printf("Error: All three marks must be integers.\n");
            continue;
        }

        int invalid_marks = 0;

        for (int j = 0; j < 3; j++) {
            if (marks[j] < MIN_MARK ||
                marks[j] > MAX_MARK) {
                invalid_marks = 1;
                break;
            }
        }

        if (invalid_marks) {
            printf("Error: Marks must be between %d and %d.\n",
                   MIN_MARK, MAX_MARK);
            continue;
        }

        name[0] = '\0';
        int name_length = 0;
        int invalid_name = 0;

        for (int j = 1; j < count - 3; j++) {
            int numeric_value;
            int token_length = (int)strlen(tokens[j]);
            int separator = (j > 1) ? 1 : 0;

            if (is_valid_integer(tokens[j], &numeric_value)) {
                invalid_name = 1;
                break;
            }

            if (name_length + separator + token_length
                >= (int)sizeof(name)) {
                invalid_name = 1;
                break;
            }

            if (separator) {
                name[name_length++] = ' ';
            }

            memcpy(name + name_length, tokens[j],
                   (size_t)token_length);

            name_length += token_length;
            name[name_length] = '\0';
        }

        if (invalid_name || name_length == 0) {
            printf("Error: Invalid name or unexpected extra input.\n");
            continue;
        }

        if (is_roll_no_exists(roll_no, i)) {
            printf("Error: Roll number already exists.\n");
            continue;
        }

        student[i].roll_no = roll_no;
        strcpy(student[i].name, name);

        for (int j = 0; j < 3; j++) {
            student[i].marks[j] = marks[j];
        }

        break;
    }
}