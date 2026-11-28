
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_CAPACITY 1
#define MAX_CAPACITY 1000
#define DATA_SIZE 30
#define CREATECACHE "createCache"
#define PUT "put"
#define GET "get"
#define EXIT "exit"

typedef struct Node {
    struct Node *prev;
    int key;
    char data[DATA_SIZE];
    struct Node *next;
} Node;

Node *head = NULL; // Front - MRU
Node *tail = NULL; // Rear - LRU

typedef enum {
    EMPTY = 0,
    OCCUPIED,
    DELETED
} Status;

typedef struct Hashmap {
    Node *value;
    Status status;
} Hashmap;

Hashmap *create_cache();
int get_capacity();
void get_input(Hashmap *cache);
void insert_DLL(Node *data, Hashmap *cache);
void delete_dll(Hashmap *cache);
void update_dll(Node *data);
void put(Hashmap *cache, int key, char *value);
Node *get(Hashmap *cache, int key);
static void print_value(Hashmap *cache, int key);
void free_memory();
void clear_input_buffer();

int capacity, count = 0;

int main() {
    capacity = get_capacity();

    if (capacity == 0) {
        return 1;
    }

    Hashmap *cache = create_cache();

    if (cache == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }

    get_input(cache);

    free_memory();
    free(cache);

    return 0;
}

Hashmap *create_cache() {
    Hashmap *arr = malloc(capacity * sizeof(Hashmap));

    if (arr == NULL) {
        return NULL;
    }

    for (int i = 0; i < capacity; i++) {
        arr[i].value = NULL;
        arr[i].status = EMPTY;
    }

    return arr;
}

int get_capacity() {
    int size;
    char line[100], word[20], extra;

    while (1) {
        printf("Enter the capacity size in the format : %s <size>\n", CREATECACHE);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            return 0;
        }

        if (strchr(line, '\n') == NULL && !feof(stdin)) {
            clear_input_buffer();
            printf("Error: Enter a valid input format.\n");
            continue;
        }

        if (sscanf(line, "%19s %d %c", word, &size, &extra) != 2) {
            printf("Error: Enter a valid input format.\n");
            continue;
        }

        if (strcmp(word, CREATECACHE) != 0) {
            printf("Error: Command must be %s.\n", CREATECACHE);
            continue;
        }

        if (size < MIN_CAPACITY || size > MAX_CAPACITY) {
            printf("Error: Capacity must be between %d-%d.\n", MIN_CAPACITY, MAX_CAPACITY);
            continue;
        }

        return size;
    }
}

void get_input(Hashmap *cache) {
    int key;
    char line[100], command[20], value[DATA_SIZE], extra;

    while (1) {
        printf("\nFollow the format based on the operation you want to perform:\n");
        printf("%s <key> <data>\n", PUT);
        printf("%s <key>\n", GET);
        printf("%s\n", EXIT);
        printf("Perform: \n\n");

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        if (strchr(line, '\n') == NULL && !feof(stdin)) {
            clear_input_buffer();
            printf("Error: Input is too long.\n");
            continue;
        }

        if (sscanf(line, "%19s", command) != 1) {
            printf("Error: Enter a valid command.\n");
            continue;
        }

        if (strcmp(command, PUT) == 0) {
            if (sscanf(line, "%*s %d %29s %c", &key, value, &extra) != 2) {
                printf("Error: Enter an integer key and a valid string.\n");
                continue;
            }

            put(cache, key, value);
        }
        else if (strcmp(command, GET) == 0) {
            if (sscanf(line, "%*s %d %c", &key, &extra) != 1) {
                printf("Error: Key must be an integer.\n");
                continue;
            }

            print_value(cache, key);
        }
        else if (strcmp(command, EXIT) == 0) {
            if (sscanf(line, "%*s %c", &extra) != EOF) {
                printf("Error: Invalid exit command.\n");
                continue;
            }

            break;
        }
        else {
            printf("Error: Invalid command.\n");
        }
    }
}

void insert_DLL(Node *data, Hashmap *cache) {
    if (count == capacity) {
        delete_dll(cache);
    }

    data->prev = NULL;
    data->next = head;

    if (head == NULL) {
        tail = data;
    }
    else {
        head->prev = data;
    }

    head = data;
    count++;
    printf("PUT: Successful!\n");
}

void delete_dll(Hashmap *cache) {
    Node *deletenode = tail;

    int start = deletenode->key % capacity;

    if (start < 0) {
        start += capacity;
    }

    for (int i = 0; i < capacity; i++) {
        int idx = (start + i) % capacity;

        if (cache[idx].status == EMPTY) {
            break;
        }

        if (cache[idx].status == OCCUPIED &&
            cache[idx].value->key == deletenode->key) {
            cache[idx].value = NULL;
            cache[idx].status = DELETED;
            break;
        }
    }

    if (head == tail) {
        head = NULL;
        tail = NULL;
    }
    else {
        tail = deletenode->prev;
        tail->next = NULL;
    }

    deletenode->prev = NULL;
    deletenode->next = NULL;

    free(deletenode);
    count--;
    printf("Removed LRU Node.\n");
}

void update_dll(Node *data) {
    if (head == data) {
        return;
    }

    data->prev->next = data->next;

    if (data->next != NULL) {
        data->next->prev = data->prev;
    }
    else {
        tail = data->prev;
    }

    data->prev = NULL;
    data->next = head;

    head->prev = data;
    head = data;
}

void put(Hashmap *cache, int key, char *value) {
    Node *newnode = get(cache, key);

    if (newnode != NULL) {
        strcpy(newnode->data, value);
        return;
    }

    newnode = malloc(sizeof(Node));

    if (newnode == NULL) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    newnode->key = key;
    strcpy(newnode->data, value);

    insert_DLL(newnode, cache);

    int start = key % capacity;

    if (start < 0) {
        start += capacity;
    }

    int deleted_idx = -1;

    for (int i = 0; i < capacity; i++) {
        int idx = (start + i) % capacity;

        if (cache[idx].status == DELETED) {
            if (deleted_idx == -1) {
                deleted_idx = idx;
            }
        }
        else if (cache[idx].status == EMPTY) {
            if (deleted_idx != -1) {
                idx = deleted_idx;
            }

            cache[idx].value = newnode;
            cache[idx].status = OCCUPIED;
            return;
        }
    }

    if (deleted_idx != -1) {
        cache[deleted_idx].value = newnode;
        cache[deleted_idx].status = OCCUPIED;
    }
}

Node *get(Hashmap *cache, int key) {
    int start = key % capacity;

    if (start < 0) {
        start += capacity;
    }

    for (int i = 0; i < capacity; i++) {
        int idx = (start + i) % capacity;

        if (cache[idx].status == OCCUPIED) {
            if (cache[idx].value->key == key) {
                update_dll(cache[idx].value);
                return cache[idx].value;
            }
        }
        else if (cache[idx].status == DELETED) {
            continue;
        }
        else {
            return NULL;
        }
    }

    return NULL;
}

static void print_value(Hashmap *cache, int key) {
    Node *get_node = get(cache, key);

    if (get_node != NULL) {
        printf("%s\n", get_node->data);
    }
    else {
        printf("NULL\n");
    }
}

void free_memory() {
    Node *curr = head;

    while (curr != NULL) {
        Node *nextnode = curr->next;
        free(curr);
        curr = nextnode;
    }

    head = NULL;
    tail = NULL;
    count = 0;
}

void clear_input_buffer() {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}
