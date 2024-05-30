#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_TITLE_LEN 25
#define MIN_TITLE_LEN 5
#define TABLE_SIZE 100

typedef struct Item {
    char title[MAX_TITLE_LEN];
    char genre[20];
    int stock;
    struct Item *next;
} Item;

typedef struct HashTable {
    Item **table;
    int size;
} HashTable;

unsigned int hash(char *title, int size) {
    unsigned int hashValue = 0;
    for (int i = 0; title[i] != '\0'; i++) {
        hashValue = (hashValue * 31) + title[i];
    }
    return hashValue % size;
}

Item* newItem(char title[], char genre[], int stock) {
    Item* item = (Item*) malloc(sizeof(Item));
    strcpy(item->title, title);
    strcpy(item->genre, genre);
    item->stock = stock;
    item->next = NULL;
    return item;
}

void addItem(HashTable *ht, char title[], char genre[], int stock) {
    int index = hash(title, ht->size);
    Item *new_node = newItem(title, genre, stock);
    new_node->next = ht->table[index];
    ht->table[index] = new_node;
}

Item* findItem(HashTable *ht, char title[]) {
    int index = hash(title, ht->size);
    Item *current = ht->table[index];
    while (current != NULL) {
        if (strcmp(current->title, title) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void deleteItem(HashTable *ht, char title[]) {
    int index = hash(title, ht->size);
    Item *current = ht->table[index];
    Item *prev = NULL;

    while (current != NULL && strcmp(current->title, title) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Item not found\n");
        return;
    }

    if (prev == NULL) {
        ht->table[index] = current->next;
    } else {
        prev->next = current->next;
    }

    free(current);
    printf("Item deleted successfully!\n");
}

void printTableHeader() {
    printf("------------------------------------------------------\n");
    printf("| %-24s | %-10s | %-10s |\n", "Item Title", "Item Genre", "Item Stock");
    printf("------------------------------------------------------\n");
}

void printTableRow(Item *item) {
    printf("| %-24s | %-10s | %-10d |\n", item->title, item->genre, item->stock);
}

void printTableFooter() {
    printf("------------------------------------------------------\n");
    printf("Press enter to continue...");
    getchar();  // To capture the newline left in the input buffer
    getchar();  // To wait for the actual enter key press
}

void viewItemsMenu(HashTable *ht) {
    int empty = 1;
    printTableHeader();
    for (int i = 0; i < ht->size; i++) {
        Item *current = ht->table[i];
        while (current != NULL) {
            printTableRow(current);
            current = current->next;
            empty = 0;
        }
    }
    if (empty) {
        printf("| %-44s |\n", "No items in the warehouse");
    }
    printTableFooter();
}

void pressEnterToContinue() {
    printf("Press enter to continue...");
    getchar();  // To capture the newline left in the input buffer
    getchar();  // To wait for the actual enter key press
}

void updateItemStock(HashTable *ht, char title[], int quantity, char* operation) {
    Item* item = findItem(ht, title);
    if (item == NULL) {
        printf("Data not found\n");
        pressEnterToContinue();
        return;
    }

    printf("Current stock: %d\n", item->stock);

    if (strcmp(operation, "add") == 0) {
        item->stock += quantity;
        printf("Data updated successfully!\n");
    } else if (strcmp(operation, "remove") == 0) {
        if (quantity > item->stock) {
            printf("Invalid quantity. Quantity must be between 1 and %d.\n", item->stock);
            pressEnterToContinue();
            return;
        }
        item->stock -= quantity;
        if (item->stock == 0) {
            deleteItem(ht, title);
            printf("%s is removed from the warehouse!\n", title);
        } else {
            printf("Data updated successfully!\n");
        }
    }

    pressEnterToContinue();
}

void updateItemStockMenu(HashTable *ht) {
    char title[MAX_TITLE_LEN];
    char operation[10];
    int quantity;

    printf("Enter item title: ");
    scanf("%s", title);
    Item* item = findItem(ht, title);
    if (item == NULL) {
        printf("Data not found\n");
        pressEnterToContinue();
        return;
    }

    printf("Current stock: %d\n", item->stock);

    while (1) {
        printf("Input update type [add | remove] [case insensitive]: ");
        scanf("%s", operation);
        if (strcmp(operation, "add") != 0 && strcmp(operation, "remove") != 0) {
            printf("Invalid update type. Please enter 'add' or 'remove'.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Input stock to %s [1-%d]: ", operation, strcmp(operation, "remove") == 0 ? item->stock : INT_MAX);
        scanf("%d", &quantity);
        if (strcmp(operation, "remove") == 0 && (quantity < 1 || quantity > item->stock)) {
            printf("Invalid quantity. Quantity must be between 1 and %d.\n", item->stock);
            continue;
        }
        if (strcmp(operation, "add") == 0 && quantity < 1) {
            printf("Invalid quantity. Quantity must be at least 1.\n");
            continue;
        }
        break;
    }

    updateItemStock(ht, title, quantity, operation);
}

int isTitleUnique(HashTable *ht, char title[]) {
    return findItem(ht, title) == NULL;
}

void insertItemMenu(HashTable *ht) {
    char title[MAX_TITLE_LEN];
    char genre[20];
    int stock;

    while (1) {
        printf("Input item title [5-25][unique]: ");
        scanf("%s", title);
        if (strlen(title) < MIN_TITLE_LEN || strlen(title) > MAX_TITLE_LEN) {
            printf("You must input 5-25 characters.\n");
            continue;
        }
        if (!isTitleUnique(ht, title)) {
            printf("Item title must be unique.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Input item genre [Action | RPG | Adventure | Card Game]: ");
        scanf("%s", genre);
        if (strcmp(genre, "Action") != 0 && strcmp(genre, "RPG") != 0 && strcmp(genre, "Adventure") != 0 && strcmp(genre, "Card Game") != 0) {
            printf("Inputted item genre must be either “Action”, “RPG”, “Adventure”, or “Card Game”.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Input item stock [>=1]: ");
        scanf("%d", &stock);
        if (stock < 1) {
            printf("You must input item stock must be at least 1.\n");
            continue;
        }
        break;
    }

    addItem(ht, title, genre, stock);
    printf("Insert Success!\n");
}

void saveData(HashTable *ht, const char *filename) {
    FILE *file = fopen(filename, "wb");
    for (int i = 0; i < ht->size; i++) {
        Item *current = ht->table[i];
        while (current != NULL) {
            fwrite(current, sizeof(Item), 1, file);
            current = current->next;
        }
    }
    fclose(file);
    printf("Data saved successfully!\n");
}

void loadData(HashTable *ht, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("File not found!\n");
        return;
    }
    
    Item *item = (Item *)malloc(sizeof(Item));
    while (fread(item, sizeof(Item), 1, file)) {
        addItem(ht, item->title, item->genre, item->stock);
        item = (Item *)malloc(sizeof(Item));
    }
    fclose(file);
    printf("Data loaded successfully!\n");
}

void saveDataMenu(HashTable *ht) {
    char filename[50];
    printf("Masukkan nama file untuk menyimpan data: ");
    scanf("%s", filename);
    saveData(ht, filename);
}

void loadDataMenu(HashTable *ht) {
    char filename[50];
    printf("Masukkan nama file untuk memuat data: ");
    scanf("%s", filename);
    loadData(ht, filename);
}

void displayMenu() {
    printf("Warehouse Management\n");
    printf("=====================\n");
    printf("1. Tambah Item Baru\n");
    printf("2. Cari Item\n");
    printf("3. Perbarui Informasi Item\n");
    printf("4. Hapus Item\n");
    printf("5. Tampilkan Semua Item\n");
    printf("6. Simpan Data ke File\n");
    printf("7. Muat Data dari File\n");
    printf("8. Keluar\n");
    printf("Enter your choice >> ");
}

int main() {
    HashTable ht;
    ht.size = TABLE_SIZE;
    ht.table = (Item **)malloc(ht.size * sizeof(Item *));
    for (int i = 0; i < ht.size; i++) {
        ht.table[i] = NULL;
    }

    int choice;

    do {
        displayMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                insertItemMenu(&ht);
                break;
            case 2:
                {
                    char title[MAX_TITLE_LEN];
                    printf("Masukkan judul item: ");
                    scanf("%s", title);
                    Item* item = findItem(&ht, title);
                    if (item != NULL) {
                        printTableHeader();
                        printTableRow(item);
                        printTableFooter();
                    } else {
                        printf("Item tidak ditemukan!\n");
                        pressEnterToContinue();
                    }
                }
                break;
            case 3:
                updateItemStockMenu(&ht);
                break;
            case 4:
                {
                    char title[MAX_TITLE_LEN];
                    printf("Masukkan judul item: ");
                    scanf("%s", title);
                    deleteItem(&ht, title);
                }
                break;
            case 5:
                viewItemsMenu(&ht);
                break;
            case 6:
                saveDataMenu(&ht);
                break;
            case 7:
                loadDataMenu(&ht);
                break;
            case 8:
                printf("Keluar dari program...\n");
                break;
            default:
                printf("Pilihan tidak valid\n");
                break;
        }
    } while (choice != 8);

    free(ht.table);
    return 0;
}

