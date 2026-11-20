#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50
#define MAX_INPUT 4096

typedef struct Filenode {
    char name[MAX_NAME + 1];
    int is_directory;
    struct Filenode *parent, *children, *next;
    int *block_pointers;
    int block_count;
    size_t content_size;
} Filenode;

typedef struct Freeblock {
    int index;
    struct Freeblock *next, *prev;
} Freeblock;

static unsigned char (*virtual_disk)[BLOCK_SIZE];
static Freeblock *free_head, *free_tail, *block_nodes;
static Filenode *root, *cwd;
static int used_blocks = 0;

void initialize_system();
void free_all_memory();
void free_tree(Filenode *directory);
Filenode *create_node(const char *name, int is_directory);
void insert_child(Filenode *parent, Filenode *node);
void remove_child(Filenode *parent, Filenode *node);
Filenode *find_child(Filenode *parent, const char *name);
void initialize_free_blocks();
int allocate_block();
void release_block(int index);
void create_directory(const char *name);
void create_file(const char *name);
void write_file(const char *filename, const char *content);
void read_file(const char *filename);
void delete_file(const char *filename);
void remove_directory(const char *name);
void list_directory(Filenode *directory);
void change_directory(const char *path);
void print_working_directory();
void display_disk_usage();
Filenode *resolve_path(const char *path);
Filenode *get_root();
void print_path(Filenode *directory);
void display_current_location();
char *skip_spaces(char *str);
int valid_name(const char *name);
int get_single_argument(char *args);

int main() {
    char input[MAX_INPUT];
    initialize_system();

    while (1) {
        display_current_location();
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        if (strchr(input, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Input is too long.\n");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';
        char *command = skip_spaces(input);
        if (*command == '\0')
            continue;

        char *args = command;
        while (*args && !isspace((unsigned char)*args))
            args++;

        if (*args != '\0')
            *args++ = '\0';
        args = skip_spaces(args);

        if (strcmp(command, "exit") == 0) {
            if (*args != '\0')
                printf("Usage: exit\n");
            else {
                free_all_memory();
                return 0;
            }
        }
        else if (strcmp(command, "mkdir") == 0) {
            if (get_single_argument(args))
                create_directory(args);
            else
                printf("Usage: mkdir directory_name\n");
        }
        else if (strcmp(command, "create") == 0) {
            if (get_single_argument(args))
                create_file(args);
            else
                printf("Usage: create filename\n");
        }
        else if (strcmp(command, "write") == 0) {
            char *filename = args;
            while (*args && !isspace((unsigned char)*args))
                args++;

            if (*args == '\0') {
                printf("Usage: write filename \"content\"\n");
                continue;
            }

            *args++ = '\0';
            args = skip_spaces(args);

            if (!valid_name(filename) || *args != '"') {
                printf("Usage: write filename \"content\"\n");
                continue;
            }

            char *content = ++args;
            char *end = strrchr(content, '"');
            if (end == NULL) {
                printf("Missing closing quotation mark.\n");
                continue;
            }

            *end = '\0';
            if (*skip_spaces(end + 1) != '\0') {
                printf("Unexpected input after content.\n");
                continue;
            }
            write_file(filename, content);
        }
        else if (strcmp(command, "read") == 0) {
            if (get_single_argument(args))
                read_file(args);
            else
                printf("Usage: read filename\n");
        }
        else if (strcmp(command, "delete") == 0) {
            if (get_single_argument(args))
                delete_file(args);
            else
                printf("Usage: delete filename\n");
        }
        else if (strcmp(command, "rmdir") == 0) {
            if (get_single_argument(args))
                remove_directory(args);
            else
                printf("Usage: rmdir directory_name\n");
        }
        else if (strcmp(command, "ls") == 0) {
            if (*args == '\0')
                list_directory(cwd);
            else {
                Filenode *directory = resolve_path(args);
                if (directory == NULL)
                    printf("Directory not found.\n");
                else
                    list_directory(directory);
            }
        }
        else if (strcmp(command, "cd") == 0) {
            if (*args == '\0')
                printf("Usage: cd directory_name\n");
            else
                change_directory(args);
        }
        else if (strcmp(command, "pwd") == 0) {
            if (*args != '\0')
                printf("Usage: pwd\n");
            else
                print_working_directory();
        }
        else if (strcmp(command, "df") == 0) {
            if (*args != '\0')
                printf("Usage: df\n");
            else
                display_disk_usage();
        }
        else
            printf("Unknown command: %s\n", command);
    }
    free_all_memory();
    return 0;
}


Filenode *create_node(const char *name, int is_directory) {
    Filenode *new_node = malloc(sizeof(Filenode));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    strcpy(new_node->name, name);
    new_node->is_directory = is_directory;
    new_node->parent = new_node->children = new_node->next = NULL;
    new_node->block_pointers = NULL;
    new_node->block_count = 0;
    new_node->content_size = 0;
    return new_node;
}

void insert_child(Filenode *parent, Filenode *node) {
    if (parent->children == NULL) {
        parent->children = node;
        node->next = node;
    }
    else {
        Filenode *last = parent->children;
        while (last->next != parent->children)
            last = last->next;
        last->next = node;
        node->next = parent->children;
    }
    node->parent = parent;
}

void remove_child(Filenode *parent, Filenode *node) {
    if (parent->children == NULL)
        return;

    Filenode *current = parent->children, *previous = NULL;
    do {
        if (current == node)
            break;
        previous = current;
        current = current->next;
    } while (current != parent->children);

    if (current != node)
        return;

    if (node->next == node)
        parent->children = NULL;
    else if (parent->children == node) {
        Filenode *last = parent->children;
        while (last->next != node)
            last = last->next;
        parent->children = node->next;
        last->next = node->next;
    }
    else
        previous->next = node->next;

    node->next = node->parent = NULL;
}

Filenode *find_child(Filenode *parent, const char *name) {
    if (parent == NULL || parent->children == NULL)
        return NULL;

    Filenode *current = parent->children;
    do {
        if (strcmp(current->name, name) == 0)
            return current;
        current = current->next;
    } while (current != parent->children);

    return NULL;
}

void initialize_free_blocks() {
    free_head = &block_nodes[0];
    free_tail = &block_nodes[NUM_BLOCKS - 1];

    for (int i = 0; i < NUM_BLOCKS; i++) {
        block_nodes[i].index = i;
        block_nodes[i].prev = (i == 0) ? NULL : &block_nodes[i - 1];
        block_nodes[i].next = (i == NUM_BLOCKS - 1) ? NULL : &block_nodes[i + 1];
    }
}

int allocate_block() {
    if (free_head == NULL)
        return -1;

    Freeblock *available = free_head;
    int index = available->index;
    free_head = available->next;

    if (free_head != NULL)
        free_head->prev = NULL;
    else
        free_tail = NULL;

    available->next = available->prev = NULL;
    used_blocks++;
    return index;
}

void release_block(int index) {
    if (index < 0 || index >= NUM_BLOCKS)
        return;

    Freeblock *released = &block_nodes[index];
    released->next = NULL;
    released->prev = free_tail;

    if (free_tail != NULL)
        free_tail->next = released;
    else
        free_head = released;

    free_tail = released;
    if (used_blocks > 0)
        used_blocks--;
}

void create_directory(const char *name) {
    if (!valid_name(name)) {
        printf("Invalid directory name.\n");
        return;
    }
    if (find_child(cwd, name) != NULL) {
        printf("An entry with this name already exists.\n");
        return;
    }

    Filenode *new_node = create_node(name, 1);
    if (new_node == NULL)
        return;

    insert_child(cwd, new_node);
    printf("Directory '%s' created successfully.\n", name);
}

void create_file(const char *name) {
    if (!valid_name(name)) {
        printf("Invalid file name.\n");
        return;
    }
    if (find_child(cwd, name) != NULL) {
        printf("An entry with this name already exists.\n");
        return;
    }

    Filenode *new_node = create_node(name, 0);
    if (new_node == NULL)
        return;

    insert_child(cwd, new_node);
    printf("File '%s' created successfully.\n", name);
}

void write_file(const char *filename, const char *content) {
    if (!valid_name(filename)) {
        printf("Invalid file name.\n");
        return;
    }

    Filenode *file = find_child(cwd, filename);
    if (file == NULL) {
        printf("File not found.\n");
        return;
    }
    if (file->is_directory) {
        printf("Cannot write to a directory.\n");
        return;
    }

    size_t size = strlen(content);
    int required = (int)((size + BLOCK_SIZE - 1) / BLOCK_SIZE);
    if (required > NUM_BLOCKS) {
        printf("File is too large for the virtual disk.\n");
        return;
    }

    int old_count = file->block_count;
    int *new_pointers = required ? malloc(required * sizeof(int)) : NULL;

    if (required && new_pointers == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    int common = old_count < required ? old_count : required;
    for (int i = 0; i < common; i++)
        new_pointers[i] = file->block_pointers[i];

    for (int i = old_count; i < required; i++) {
        new_pointers[i] = allocate_block();
        if (new_pointers[i] == -1) {
            for (int j = old_count; j < i; j++)
                release_block(new_pointers[j]);
            free(new_pointers);
            printf("Disk is full. Write operation failed.\n");
            return;
        }
    }

    for (int i = required; i < old_count; i++)
        release_block(file->block_pointers[i]);

    free(file->block_pointers);
    file->block_pointers = new_pointers;
    file->block_count = required;
    file->content_size = size;

    size_t written = 0;
    for (int i = 0; i < required; i++) {
        int block = new_pointers[i];
        size_t remaining = size - written;
        size_t bytes = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

        memset(virtual_disk[block], 0, BLOCK_SIZE);
        if (bytes > 0)
            memcpy(virtual_disk[block], content + written, bytes);
        written += bytes;
    }

    printf("Data written successfully (size=%zu bytes).\n", size);
}

void read_file(const char *filename) {
    if (!valid_name(filename)) {
        printf("Invalid file name.\n");
        return;
    }

    Filenode *file = find_child(cwd, filename);
    if (file == NULL) {
        printf("File not found.\n");
        return;
    }
    if (file->is_directory) {
        printf("Cannot read a directory.\n");
        return;
    }
    if (file->content_size == 0) {
        printf("File is empty.\n");
        return;
    }

    size_t remaining = file->content_size;
    for (int i = 0; i < file->block_count; i++) {
        size_t bytes = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        fwrite(virtual_disk[file->block_pointers[i]], 1, bytes, stdout);
        remaining -= bytes;
    }
    printf("\n");
}

void delete_file(const char *filename) {
    if (!valid_name(filename)) {
        printf("Invalid file name.\n");
        return;
    }

    Filenode *file = find_child(cwd, filename);
    if (file == NULL) {
        printf("File not found.\n");
        return;
    }
    if (file->is_directory) {
        printf("Use rmdir to remove a directory.\n");
        return;
    }

    for (int i = 0; i < file->block_count; i++)
        release_block(file->block_pointers[i]);

    free(file->block_pointers);
    remove_child(cwd, file);
    free(file);
    printf("File deleted successfully.\n");
}

void remove_directory(const char *name) {
    if (!valid_name(name)) {
        printf("Invalid directory name.\n");
        return;
    }

    Filenode *directory = find_child(cwd, name);
    if (directory == NULL) {
        printf("Directory not found.\n");
        return;
    }
    if (!directory->is_directory) {
        printf("The specified entry is not a directory.\n");
        return;
    }
    if (directory->children != NULL) {
        printf("Directory is not empty.\n");
        return;
    }

    remove_child(cwd, directory);
    free(directory);
    printf("Directory removed successfully.\n");
}

void list_directory(Filenode *directory) {
    if (directory == NULL || !directory->is_directory) {
        printf("Invalid directory.\n");
        return;
    }
    if (directory->children == NULL) {
        printf("(empty)\n");
        return;
    }

    Filenode *current = directory->children;
    do {
        printf(current->is_directory ? "[DIR]  %s/\n" : "[FILE] %s\n", current->name);
        current = current->next;
    } while (current != directory->children);
}

Filenode *get_root() {
    Filenode *current = cwd;
    while (current->parent != NULL)
        current = current->parent;
    return current;
}

Filenode *resolve_path(const char *path) {
    if (path == NULL || *path == '\0')
        return cwd;

    Filenode *current = path[0] == '/' ? get_root() : cwd;
    char copy[MAX_INPUT];

    if (strlen(path) >= sizeof(copy))
        return NULL;
    strcpy(copy, path);

    char *part = strtok(copy, "/");
    while (part != NULL) {
        if (strcmp(part, ".") == 0) {
            part = strtok(NULL, "/");
            continue;
        }
        if (strcmp(part, "..") == 0) {
            if (current->parent != NULL)
                current = current->parent;
        }
        else {
            Filenode *next_node = find_child(current, part);
            if (next_node == NULL || !next_node->is_directory)
                return NULL;
            current = next_node;
        }
        part = strtok(NULL, "/");
    }
    return current;
}

void change_directory(const char *path) {
    if (path == NULL || *path == '\0') {
        printf("Please specify a directory.\n");
        return;
    }

    Filenode *directory = resolve_path(path);
    if (directory == NULL) {
        printf("Directory not found.\n");
        return;
    }

    cwd = directory;
    printf("Moved to ");
    print_path(cwd);
    printf("\n");
}

void print_path(Filenode *directory) {
    if (directory->parent == NULL) {
        printf("/");
        return;
    }

    print_path(directory->parent);
    if (directory->parent->parent != NULL)
        printf("/");
    printf("%s", directory->name);
}

void print_working_directory() {
    print_path(cwd);
    printf("\n");
}

void display_disk_usage() {
    int free_blocks = NUM_BLOCKS - used_blocks;
    double usage = (double)used_blocks / NUM_BLOCKS * 100;

    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", used_blocks);
    printf("Free Blocks: %d\n", free_blocks);
    printf("Disk Usage: %.2f%%\n", usage);
}

char *skip_spaces(char *str) {
    while (*str && isspace((unsigned char)*str))
        str++;
    return str;
}

int valid_name(const char *name) {
    if (name == NULL || *name == '\0' || strlen(name) > MAX_NAME)
        return 0;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        return 0;

    for (int i = 0; name[i] != '\0'; i++)
        if (isspace((unsigned char)name[i]) || name[i] == '/' || name[i] == '"')
            return 0;

    return 1;
}

int get_single_argument(char *args) {
    if (*args == '\0')
        return 0;

    for (int i = 0; args[i] != '\0'; i++)
        if (isspace((unsigned char)args[i]))
            return 0;

    return 1;
}

void initialize_system() {
    virtual_disk = calloc(NUM_BLOCKS, sizeof(*virtual_disk));
    block_nodes = calloc(NUM_BLOCKS, sizeof(Freeblock));

    if (virtual_disk == NULL || block_nodes == NULL) {
        printf("Memory allocation failed.\n");
        free(virtual_disk);
        free(block_nodes);
        exit(EXIT_FAILURE);
    }

    initialize_free_blocks();
    root = create_node("/", 1);

    if (root == NULL) {
        free(block_nodes);
        free(virtual_disk);
        exit(EXIT_FAILURE);
    }

    cwd = root;
    printf("Compact VFS - ready. Type 'exit' to quit.\n");
}

void free_tree(Filenode *directory) {
    if (directory == NULL || directory->children == NULL)
        return;

    Filenode *current = directory->children, *last = current;
    while (last->next != current)
        last = last->next;
    last->next = NULL;

    while (current != NULL) {
        Filenode *next_node = current->next;
        if (current->is_directory)
            free_tree(current);
        free(current->block_pointers);
        free(current);
        current = next_node;
    }
    directory->children = NULL;
}

void free_all_memory() {
    free_tree(root);
    free(root);
    root = cwd = NULL;

    free(block_nodes);
    free(virtual_disk);
    block_nodes = free_head = free_tail = NULL;
    virtual_disk = NULL;
    used_blocks = 0;

    printf("Memory released. Exiting program...\n");
}

void display_current_location() {
    print_path(cwd);
    printf(" > ");
}