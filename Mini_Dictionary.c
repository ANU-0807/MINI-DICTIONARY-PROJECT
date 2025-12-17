#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Node {
    char word[50];
    char meaning[200];
    struct Node *left, *right;
};

// Function to create a new node
struct Node* createNode(char word[], char meaning[]) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (!newNode) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strcpy(newNode->word, word);
    strcpy(newNode->meaning, meaning);
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Case-insensitive string comparison
int compareIgnoreCase(char a[], char b[]) {
    for (int i = 0; a[i] && b[i]; i++) {
        if (tolower(a[i]) != tolower(b[i]))
            return tolower(a[i]) - tolower(b[i]);
    }
    return tolower(a[0]) - tolower(b[0]);
}

// Insert word in BST
struct Node* insert(struct Node *root, char word[], char meaning[]) {
    if (root == NULL) {
        return createNode(word, meaning);
    }

    int cmp = compareIgnoreCase(word, root->word);
    if (cmp < 0)
        root->left = insert(root->left, word, meaning);
    else if (cmp > 0)
        root->right = insert(root->right, word, meaning);
    else {
        printf("Word already exists! Updating meaning.\n");
        strcpy(root->meaning, meaning);
    }

    return root;
}

// Search word in BST
struct Node* search(struct Node *root, char word[]) {
    if (root == NULL)
        return NULL;

    int cmp = compareIgnoreCase(word, root->word);
    if (cmp == 0)
        return root;
    else if (cmp < 0)
        return search(root->left, word);
    else
        return search(root->right, word);
}

// Display dictionary in alphabetical order
void display(struct Node *root) {
    if (root != NULL) {
        display(root->left);
        printf("%s : %s\n", root->word, root->meaning);
        display(root->right);
    }
}

// Save words to file
void saveToFile(struct Node *root, FILE *fp) {
    if (root == NULL) return;

    saveToFile(root->left, fp);
    fprintf(fp, "%s|%s\n", root->word, root->meaning);
    saveToFile(root->right, fp);
}

// Load dictionary from file
struct Node* loadFromFile(struct Node *root) {
    FILE *fp = fopen("dictionary.txt", "r");
    if (fp == NULL) {
        printf("No existing dictionary found. Starting fresh.\n");
        return root;
    }

    char line[300];
    while (fgets(line, sizeof(line), fp)) {
        char *sep = strchr(line, '|');
        if (!sep) continue;

        *sep = '\0'; // split word and meaning
        char *word = line;
        char *meaning = sep + 1;

        // Remove newline from meaning
        meaning[strcspn(meaning, "\n")] = '\0';

        root = insert(root, word, meaning);
    }

    fclose(fp);
    return root;
}

// Free all nodes
void freeTree(struct Node* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main() {
    struct Node *root = NULL;
    root = loadFromFile(root);

    int choice;
    char word[50], meaning[200];

    while (1) {
        printf("\n--- MINI DICTIONARY ---\n");
        printf("1. Add Word\n");
        printf("2. Search Word\n");
        printf("3. Display All Words\n");
        printf("4. Save & Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Exiting...\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("Enter word: ");
                getchar(); // clear newline
                fgets(word, sizeof(word), stdin);
                word[strcspn(word, "\n")] = '\0';

                printf("Enter meaning: ");
                fgets(meaning, sizeof(meaning), stdin);
                meaning[strcspn(meaning, "\n")] = '\0';

                root = insert(root, word, meaning);
                printf("Word added successfully!\n");
                break;

            case 2:
                printf("Enter word to search: ");
                getchar(); // clear newline
                fgets(word, sizeof(word), stdin);
                word[strcspn(word, "\n")] = '\0';

                struct Node* found = search(root, word);
                if (found)
                    printf("Meaning: %s\n", found->meaning);
                else
                    printf("Word not found!\n");
                break;

            case 3:
                printf("\n--- DICTIONARY CONTENT ---\n");
                display(root);
                break;

            case 4: {
                FILE *fp = fopen("dictionary.txt", "w");
                if (fp == NULL) {
                    printf("Error saving file.\n");
                    freeTree(root);
                    return 1;
                }
                saveToFile(root, fp);
                fclose(fp);
                printf("Dictionary saved. Exiting...\n");
                freeTree(root);
                return 0;
            }

            default:
                printf("Invalid choice!\n");
        }
    }

    freeTree(root);
    return 0;
}
