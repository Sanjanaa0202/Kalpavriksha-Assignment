#include <stdio.h>
#include <stdlib.h>

#define MIN_POOL_SIZE 1
#define MAX_POOL_SIZE 1000000
#define MIN_CHOICE 1
#define MAX_CHOICE 7

struct block {
    int start;
    int size;
    int id;
    int free;
};

char *pool;
int pool_size;

struct block *blocks;
int block_count = 0;
int block_capacity = 4;

int current_id = 1;
int strategy = 1;

int choice, size, id;

void validate_pool_size_input();
void validate_input_choice();
void validate_size_input();
void validate_id_input();
void validate_strategy_input();
void create_memory_pool();
int extend_block();
int insert_block(int pos, int start, int size, int id, int free);
void remove_block(int pos);
int find_id(int id);
void merge_free_block();
int first_fit(int size);
int best_fit(int size);
int worst_fit(int size);
int find_free_block(int size);
int total_free_blocks();
int largest_free_block();
void check_fragmentation(int size);
void display_memory_statistics();
void display_memory_map();
void allocate_memory(int size);
void release_memory(int id);
void resize_memory(int id, int new_size);
void choose_strategy();
void operation();

int main() {
    create_memory_pool();
    choose_strategy();

    do {
        printf("\nMenu:\n");
        printf("1. Allocate memory\n");
        printf("2. Release memory\n");
        printf("3. Resize allocation\n");
        printf("4. Display memory map\n");
        printf("5. Display memory statistics\n");
        printf("6. Change allocation strategy\n");
        printf("7. Exit\n");
        validate_input_choice();
        operation();
    } while (choice != MAX_CHOICE);

    free(pool);
    free(blocks);
    printf("Terminated.\n");
    return 0;
}

void operation() {
    switch (choice) {
        case 1:
            validate_size_input();
            allocate_memory(size);
            break;
        case 2:
            validate_id_input();
            release_memory(id);
            break;
        case 3:
            validate_id_input();
            if (find_id(id) == -1) {
                printf("\nError: Unknown or already released ID.\n");
            }
            else {
                validate_size_input();
                resize_memory(id, size);
            }
            break;
        case 4:
            display_memory_map();
            break;
        case 5:
            display_memory_statistics();
            break;
        case 6:
            choose_strategy();
            break;
        default:
            break;
    }
}

void validate_pool_size_input() {
    char line[50], extra_char;
    int invalid;
    do {
        invalid = 0;
        printf("Enter the Memory Pool Size (%d-%d): ", MIN_POOL_SIZE, MAX_POOL_SIZE);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nError: Unable to read input. Exiting program.\n");
            exit(1);
        }
        if (sscanf(line, "%d %c", &pool_size, &extra_char) != 1) {
            printf("\nError: Enter a valid Number.\n");
            invalid = 1;
        }
        else if (pool_size < MIN_POOL_SIZE || pool_size > MAX_POOL_SIZE) {
            printf("\nError: Pool Size must be between %d and %d.\n", MIN_POOL_SIZE, MAX_POOL_SIZE);
            invalid = 1;
        }
    } while (invalid);
}

void validate_input_choice() {
    char line[50], extra_char;
    int invalid;
    do {
        invalid = 0;
        printf("\nEnter your Choice: ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nError: Unable to read input. Exiting program.\n");
            exit(1);
        }
        if (sscanf(line, "%d %c", &choice, &extra_char) != 1) {
            printf("\nError: Enter a valid Number.\n");
            invalid = 1;
        }
        else if (choice < MIN_CHOICE || choice > MAX_CHOICE) {
            printf("\nError: Choice must be between %d and %d.\n", MIN_CHOICE, MAX_CHOICE);
            invalid = 1;
        }
    } while (invalid);
}

void validate_size_input() {
    char line[50], extra_char;
    int invalid;
    do {
        invalid = 0;
        printf("\nRequested Size: ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nError: Unable to read input. Exiting program.\n");
            exit(1);
        }
        if (sscanf(line, "%d %c", &size, &extra_char) != 1) {
            printf("\nError: Enter a valid Number.\n");
            invalid = 1;
        }
        else if (size < 1 || size > pool_size) {
            printf("\nError: Size must be between 1 and %d.\n", pool_size);
            invalid = 1;
        }
    } while (invalid);
}

void validate_id_input() {
    char line[50], extra_char;
    int invalid;
    do {
        invalid = 0;
        printf("\nAllocation ID: ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nError: Unable to read input. Exiting program.\n");
            exit(1);
        }
        if (sscanf(line, "%d %c", &id, &extra_char) != 1) {
            printf("\nError: Enter a valid Number.\n");
            invalid = 1;
        }
        else if (id < 1) {
            printf("\nError: Allocation ID must be a positive Number.\n");
            invalid = 1;
        }
    } while (invalid);
}

void validate_strategy_input() {
    char line[50], extra_char;
    int invalid;
    do {
        invalid = 0;
        printf("\nAllocation Strategy\n");
        printf(" 1. First Fit\n 2. Best Fit\n 3. Worst Fit\n");
        printf("Choose the Allocation Strategy: ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nError: Unable to read input. Exiting program.\n");
            exit(1);
        }
        if (sscanf(line, "%d %c", &strategy, &extra_char) != 1) {
            printf("\nError: Enter a valid Number.\n");
            invalid = 1;
        }
        else if (strategy < 1 || strategy > 3) {
            printf("\nError: Choose 1, 2 or 3.\n");
            invalid = 1;
        }
    } while (invalid);
}

void create_memory_pool() {
    validate_pool_size_input();
    pool = malloc(pool_size);
    blocks = malloc(block_capacity * sizeof(struct block));
    if (pool == NULL || blocks == NULL) {
        printf("Could not allocate memory.\n");
        free(pool);
        free(blocks);
        exit(1);
    }
    blocks[0].start = 0;
    blocks[0].size = pool_size;
    blocks[0].id = 0;
    blocks[0].free = 1;
    block_count = 1;
}

void choose_strategy() {
    validate_strategy_input();
    if (strategy == 1) {
        printf("Strategy set to First Fit.\n");
    }
    else if (strategy == 2) {
        printf("Strategy set to Best Fit.\n");
    }
    else {
        printf("Strategy set to Worst Fit.\n");
    }
}

int extend_block() {
    struct block *temp;
    if (block_count < block_capacity) {
        return 1;
    }
    temp = realloc(blocks, block_capacity * 2 * sizeof(struct block));
    if (temp == NULL) {
        return 0;
    }
    blocks = temp;
    block_capacity = block_capacity * 2;
    return 1;
}

int insert_block(int pos, int start, int size, int id, int free) {
    int i;
    if (!extend_block()) {
        return 0;
    }
    for (i = block_count; i > pos; i--) {
        blocks[i] = blocks[i - 1];
    }
    blocks[pos].start = start;
    blocks[pos].size = size;
    blocks[pos].id = id;
    blocks[pos].free = free;
    block_count++;
    return 1;
}

void remove_block(int pos) {
    int i;
    for (i = pos; i < block_count - 1; i++) {
        blocks[i] = blocks[i + 1];
    }
    block_count--;
}

int find_id(int id) {
    int i;
    for (i = 0; i < block_count; i++) {
        if (blocks[i].free == 0 && blocks[i].id == id) {
            return i;
        }
    }
    return -1;
}

void merge_free_block() {
    int i = 0;
    while (i < block_count - 1) {
        if (blocks[i].free && blocks[i + 1].free) {
            blocks[i].size += blocks[i + 1].size;
            remove_block(i + 1);
        }
        else {
            i++;
        }
    }
}

int first_fit(int size) {
    int i;
    for (i = 0; i < block_count; i++) {
        if (blocks[i].free && blocks[i].size >= size) {
            return i;
        }
    }
    return -1;
}

int best_fit(int size) {
    int i;
    int best = -1;
    for (i = 0; i < block_count; i++) {
        if (blocks[i].free && blocks[i].size >= size) {
            if (best == -1 || blocks[i].size < blocks[best].size) {
                best = i;
            }
        }
    }
    return best;
}

int worst_fit(int size) {
    int i;
    int worst = -1;
    for (i = 0; i < block_count; i++) {
        if (blocks[i].free && blocks[i].size >= size) {
            if (worst == -1 || blocks[i].size > blocks[worst].size) {
                worst = i;
            }
        }
    }
    return worst;
}

int find_free_block(int size) {
    if (strategy == 1) {
        return first_fit(size);
    }
    else if (strategy == 2) {
        return best_fit(size);
    }
    else {
        return worst_fit(size);
    }
}

int total_free_blocks() {
    int i, total = 0;
    for (i = 0; i < block_count; i++) {
        if (blocks[i].free) {
            total += blocks[i].size;
        }
    }
    return total;
}

int largest_free_block() {
    int i, largest = 0;
    for (i = 0; i < block_count; i++) {
        if (blocks[i].free && blocks[i].size > largest) {
            largest = blocks[i].size;
        }
    }
    return largest;
}

void check_fragmentation(int size) {
    int i;
    if (total_free_blocks() < size) {
        return;
    }
    printf("External fragmentation: %d bytes are free in total, but only in separate pieces of size:", total_free_blocks());
    for (i = 0; i < block_count; i++) {
        if (blocks[i].free) {
            printf(" %d", blocks[i].size);
        }
    }
    printf("\nThe largest piece is %d bytes, so %d bytes cannot fit.\n", largest_free_block(), size);
}

void display_memory_statistics() {
    int i;
    int alloc_bytes = 0, free_bytes = 0;
    int alloc_blocks = 0, free_blocks = 0;
    for (i = 0; i < block_count; i++) {
        if (blocks[i].free) {
            free_bytes += blocks[i].size;
            free_blocks++;
        }
        else {
            alloc_bytes += blocks[i].size;
            alloc_blocks++;
        }
    }
    printf("Total memory: %d bytes\n", pool_size);
    printf("Allocated: %d bytes\n", alloc_bytes);
    printf("Free: %d bytes\n", free_bytes);
    printf("Allocated blocks: %d\n", alloc_blocks);
    printf("Free blocks: %d\n", free_blocks);
    printf("Largest free block: %d bytes\n", largest_free_block());
}

void display_memory_map() {
    int i;
    printf("Memory Map (%d bytes):\n", pool_size);
    for (i = 0; i < block_count; i++) {
        if (blocks[i].free) {
            printf("[Free: %d bytes, Start: %d]\n", blocks[i].size, blocks[i].start);
        }
        else {
            printf("[Allocated #%d: %d bytes, Start: %d]\n", blocks[i].id, blocks[i].size, blocks[i].start);
        }
    }
}

void allocate_memory(int size) {
    int idx = find_free_block(size);
    int start, leftover, i;
    if (idx == -1) {
        printf("Allocation failed: no contiguous free block is large enough.\n");
        check_fragmentation(size);
        return;
    }
    start = blocks[idx].start;
    leftover = blocks[idx].size - size;
    if (leftover > 0) {
        if (!insert_block(idx + 1, start + size, leftover, 0, 1)) {
            printf("Allocation failed: out of memory for block list.\n");
            return;
        }
    }
    blocks[idx].size = size;
    blocks[idx].free = 0;
    blocks[idx].id = current_id;
    current_id++;
    for (i = 0; i < size; i++) {
        pool[start + i] = 'A' + (blocks[idx].id - 1) % 26;
    }
    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n", blocks[idx].id, start, size);
}

void release_memory(int id) {
    int idx = find_id(id);
    if (idx == -1) {
        if (id < current_id) {
            printf("Error: allocation %d was already released.\n", id);
        }
        else {
            printf("Error: unknown allocation ID %d.\n", id);
        }
        return;
    }
    blocks[idx].free = 1;
    blocks[idx].id = 0;
    merge_free_block();
    printf("Allocation released.\n");
}

void resize_memory(int id, int new_size) {
    int i = find_id(id);
    int old_size, old_start, new_start, j, k, diff;
    if (i == -1) {
        printf("Error: unknown or already released ID %d.\n", id);
        return;
    }
    old_size = blocks[i].size;
    old_start = blocks[i].start;
    if (new_size == old_size) {
        printf("Allocation already has that size.\n");
        return;
    }
    if (new_size < old_size) {
        diff = old_size - new_size;
        if (i + 1 < block_count && blocks[i + 1].free) {
            blocks[i + 1].start -= diff;
            blocks[i + 1].size += diff;
        }
        else {
            if (!insert_block(i + 1, old_start + new_size, diff, 0, 1)) {
                printf("Resize failed. Original allocation unchanged.\n");
                return;
            }
        }
        blocks[i].size = new_size;
        merge_free_block();
        printf("Allocation resized. ID: %d, Start: %d, Size: %d\n", id, old_start, new_size);
        return;
    }
    diff = new_size - old_size;
    if (i + 1 < block_count && blocks[i + 1].free && blocks[i + 1].size >= diff) {
        if (blocks[i + 1].size == diff) {
            remove_block(i + 1);
        }
        else {
            blocks[i + 1].start += diff;
            blocks[i + 1].size -= diff;
        }
        blocks[i].size = new_size;
        printf("Allocation expanded in place. ID: %d, Start: %d, Size: %d\n", id, old_start, new_size);
        return;
    }
    j = find_free_block(new_size);
    if (j == -1) {
        printf("Resize failed: no contiguous free block is large enough. Original allocation unchanged.\n");
        check_fragmentation(new_size);
        return;
    }
    if (!extend_block()) {
        printf("Resize failed: out of memory for block list.\n");
        return;
    }
    new_start = blocks[j].start;
    for (k = 0; k < old_size; k++) {
        pool[new_start + k] = pool[old_start + k];
    }
    if (blocks[j].size > new_size) {
        insert_block(j + 1, new_start + new_size, blocks[j].size - new_size, 0, 1);
        if (j < i) {
            i++;
        }
    }
    blocks[j].size = new_size;
    blocks[j].free = 0;
    blocks[j].id = id;
    blocks[i].free = 1;
    blocks[i].id = 0;
    merge_free_block();
    printf("Allocation moved. ID: %d, Old start: %d, New start: %d, Size: %d\n", id, old_start, new_start, new_size);
}