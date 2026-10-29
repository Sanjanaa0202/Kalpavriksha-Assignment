#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_SIZE 2
#define MAX_SIZE 10
#define START_INDEX 0

int size; // Global Variable

int get_matrix_size();
void display_matrix(int **arr);
void rotate_matrix(int **arr);
void apply_smoothing_filter(int **arr);
void calculate_avg_for_each_cell(int **arr, int r, int c, int *add, int *count);
void free_matrix(int **arr, int filled);
void clear_input_buffer();

int main() {
    if (get_matrix_size() == 0) {
        return 0;
    }

    int **matrix = malloc((size * size) * sizeof(int *));
    if (matrix == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }

    // Initialize random number generator
    srand((unsigned int) time(NULL));
    // Store random value in the matrix
    for (int row = START_INDEX; row < size; row++) {
        for (int col = START_INDEX; col < size; col++) {
            int *cell = malloc(sizeof(int));
            if (cell == NULL) {
                printf("Error: Memory allocation failed.\n");
                free_matrix(matrix, row * size + col);
                return 1;
            }
            *cell = rand() % 256;
            *(matrix + (row * size + col)) = cell;
        }
    }

    printf("\nOriginal Randomly Generated Matrix: \n");
    display_matrix(matrix);

    rotate_matrix(matrix);

    printf("\nMatrix after 90 degree Clockwise Rotation: \n");
    display_matrix(matrix);

    apply_smoothing_filter(matrix);

    printf("\nMatrix after Applying 3*3 Smoothing Filter: \n");
    display_matrix(matrix);

    free_matrix(matrix, size * size);
    printf("\nMemory is freed!\n");

    return 0;
}

int get_matrix_size() {
    int invalid = 1;
    while (invalid) {
        printf("Enter matrix size (%d-%d): ", MIN_SIZE, MAX_SIZE);

        int result = scanf("%d", &size);
        if (result == EOF) {
            printf("Error: End of File. Exiting program.\n");
            return 0;
        }

        if (result == 0) {
            printf("Error: Enter a valid integer.\n");
            clear_input_buffer();
            continue;
        }

        clear_input_buffer();

        if (size < MIN_SIZE || size > MAX_SIZE) {
            printf("Error: The size of the matrix must be between %d and %d.\n", MIN_SIZE, MAX_SIZE);
            continue;
        }
        return 1;
    }
}

void display_matrix(int **arr) {
    for (int row = START_INDEX; row < size; row++) {
        for (int col = START_INDEX; col < size; col++) {
            printf("%d ", **(arr + (row * size + col)));
        }
        printf("\n");
    }
}

void rotate_matrix(int **arr) {
    // Rotating only pointer address
    for (int row = START_INDEX; row < size / 2; row++) {
        int last = size - 1 - row;
        for (int col = row; col < last; col++) {
            int offset = col - row;

            int **top    = arr + (row * size + col);
            int **right  = arr + (col * size + last);
            int **bottom = arr + (last * size + (last - offset));
            int **left   = arr + ((last - offset) * size + row);

            int *temp = *left;     
            *left     = *bottom;   
            *bottom   = *right;    
            *right    = *top;     
            *top      = temp;     
        }
    }
}

void calculate_avg_for_each_cell(int **arr, int r, int c, int *add, int *count) {
    for (int row = r - 1; row <= r + 1; row++) {
        if (row < START_INDEX || row >= size) {
            continue;
        }
        for (int col = c - 1; col <= c + 1; col++) {
            if (col < START_INDEX || col >= size) {
                continue;
            }
            *add += **(arr + (row * size + col)) % 256;  // Access old value
            (*count)++;
        }
    }
}

void apply_smoothing_filter(int **arr) {
    int sum, count, avg;

    // store values in each cell by encoding --> (old_value + new_avg * 256)
    for (int row = START_INDEX; row < size; row++) {
        for (int col = START_INDEX; col < size; col++) {
            sum = count = 0;
            calculate_avg_for_each_cell(arr, row, col, &sum, &count);
            avg = sum / count;
            int *cell = *(arr + (row * size + col));
            *cell = (*cell % 256) + avg * 256;
        }
    }

    // Replace values to the average values in each cell
    for (int row = START_INDEX; row < size; row++) {
        for (int col = START_INDEX; col < size; col++) {
            **(arr + (row * size + col)) /= 256;
        }
    }
}

void free_matrix(int **arr, int filled) {
    for (int i = 0; i < filled; i++) {
        free(*(arr + i));
    }
    free(arr);
}

void clear_input_buffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}