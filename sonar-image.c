#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_SIZE 2
#define MAX_SIZE 10
#define START_INDEX 0

int size; // Global Variable

int get_matrix_size();
void display_matrix(int *arr);
void rotate_matrix(int *arr);
void apply_smoothing_filter(int *arr);
void calculate_avg_for_each_cell(int *arr, int r, int c, int *add, int *count);
void clear_input_buffer();

int main() {
    if (get_matrix_size() == 0) {
        return 0;
    }
    int *matrix;
    matrix = malloc((size * size) * sizeof(int));
    if (matrix == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }

    // Initialize random number generator
    srand((unsigned int) time(NULL));
    // Store random value in the matrix
    for (int row = START_INDEX; row < size; row++) {
        for (int col = START_INDEX; col < size; col++) {
            *(matrix + (row * size + col)) = rand() % 256;
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

    free(matrix);
    printf("\nMemory is freed!\n");

    return 0;
}

int get_matrix_size() {
    int invalid = 1;
    while (invalid) {
        printf("Enter matrix size (%d-%d): ", MIN_SIZE, MAX_SIZE);

        int result = scanf("%d", &size);
        if (result == 0) {
            printf("Error: Enter a valid integer.\n");
            clear_input_buffer();
            continue;
        }

        if (result == EOF) {
            printf("Error: End of File. Exiting program.\n");
            return 0;
        }

        clear_input_buffer();

        if (size < MIN_SIZE || size > MAX_SIZE) {
            printf("Error: The size of the matrix must be between %d and %d.\n", MIN_SIZE, MAX_SIZE);
            continue;
        }
        return 1;
    }
}

void display_matrix(int *arr) {
    for (int row = START_INDEX; row < size; row++) {
        for (int col = START_INDEX; col < size; col++) {
            printf("%d ", *(arr + (row * size + col)));
        }
        printf("\n");
    }
}

void rotate_matrix(int *arr) {
    // Transpose matrix
    for (int row = START_INDEX; row < size; row++) {
        for (int col = row+1; col < size; col++) {
            int temp = *(arr + (row * size + col));
            *(arr + (row * size + col)) = *(arr + (col * size + row));
            *(arr + (col * size + row)) = temp;
        }
    }

    // Reverse matrix
    for (int row = START_INDEX; row < size; row++) {
        for (int col = START_INDEX; col < size/2; col++) {
            int temp = *(arr + (row * size + col));
            *(arr + (row * size + col)) = *(arr + (row * size + (size - col - 1)));
            *(arr + (row * size + (size - col - 1))) = temp;
        }
    }
}

void calculate_avg_for_each_cell(int *arr, int r, int c, int *add, int *count) {
    for (int row = r-1; row <= r+1; row++) {
        if (row < START_INDEX || row >= size) {
            continue;
        }
        for (int col = c-1; col <= c+1; col++) {
            if (col < START_INDEX || col >= size) {
                continue;
            }
            *add += *(arr + (row * size + col)) % 256; // Access old value
            (*count)++;
        }
    }
}

void apply_smoothing_filter(int *arr) {
    int sum, count, avg;

    // store values in each cell by calculating --> (old_value + new_avg * 256)
    for (int row = START_INDEX; row < size; row++) { 
        for (int col = START_INDEX; col < size; col++) {
            sum = count = 0;
            calculate_avg_for_each_cell(arr, row, col, &sum, &count);
            avg = sum / count;
            *(arr + (row * size + col)) = (*(arr + (row * size + col)) % 256 ) + avg * 256;
        }
    }

    // Replace values to the average values in each cell
    for (int row = START_INDEX; row < size; row++) {
        for (int col = START_INDEX; col < size; col++) {
            *(arr + (row * size + col)) /= 256;
        }
    }
} 

void clear_input_buffer() {
    int ch;
    while((ch = getchar()) != '\n' && ch != EOF);
}