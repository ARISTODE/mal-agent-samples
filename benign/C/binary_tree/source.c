#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_VALUE_LEN 64
#define MAX_NODES 1000

// Binary tree node structure
typedef struct TreeNode {
    int key;
    char value[MAX_VALUE_LEN];
    struct TreeNode* left;
    struct TreeNode* right;
    int height;  // For AVL balancing (if needed)
} TreeNode;

// Tree statistics
typedef struct {
    int total_nodes;
    int max_depth;
    int insert_count;
    int delete_count;
    int search_count;
    time_t created;
} TreeStats;

// Global tree and statistics
static TreeNode* root = NULL;
static TreeStats stats = {0};

void print_usage() {
    printf("Usage: binary.exe [COMMAND] [ARGS...]\n");
    printf("Binary Tree Data Structure Demonstration\n\n");
    printf("Commands:\n");
    printf("  --help, -h         Show this help message\n");
    printf("  --test             Run comprehensive tree tests\n");
    printf("  --insert K V       Insert key K with value V\n");
    printf("  --search K         Search for key K\n");
    printf("  --delete K         Delete key K\n");
    printf("  --print            Print tree structure\n");
    printf("  --stats            Show tree statistics\n");
    printf("  --min              Find minimum key\n");
    printf("  --max              Find maximum key\n");
    printf("  --height           Show tree height\n");
    printf("  --balance          Check if tree is balanced\n");
    printf("  --traverse TYPE    Traverse tree (inorder, preorder, postorder, level)\n");
    printf("  --benchmark N      Run performance benchmark with N operations\n");
    printf("  --load FILE        Load tree from file\n");
    printf("  --save FILE        Save tree to file\n");
    printf("\nFeatures:\n");
    printf("  - Binary search tree operations\n");
    printf("  - Multiple traversal algorithms\n");
    printf("  - Tree statistics and analysis\n");
    printf("  - File I/O for persistence\n");
    printf("  - Performance benchmarking\n");
}

// Create a new tree node
TreeNode* create_node(int key, const char* value) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    node->key = key;
    strncpy(node->value, value, MAX_VALUE_LEN - 1);
    node->value[MAX_VALUE_LEN - 1] = '\0';
    node->left = NULL;
    node->right = NULL;
    node->height = 1;

    return node;
}

// Insert a node into the tree
TreeNode* insert_node(TreeNode* node, int key, const char* value) {
    // Standard BST insertion
    if (node == NULL) {
        stats.insert_count++;
        stats.total_nodes++;
        printf("Inserted key %d with value '%s'\n", key, value);
        return create_node(key, value);
    }

    if (key < node->key) {
        node->left = insert_node(node->left, key, value);
    } else if (key > node->key) {
        node->right = insert_node(node->right, key, value);
    } else {
        // Key already exists, update value
        printf("Updated key %d: '%s' -> '%s'\n", key, node->value, value);
        strncpy(node->value, value, MAX_VALUE_LEN - 1);
        node->value[MAX_VALUE_LEN - 1] = '\0';
        return node;
    }

    // Update height for potential AVL operations
    int left_height = node->left ? node->left->height : 0;
    int right_height = node->right ? node->right->height : 0;
    node->height = 1 + (left_height > right_height ? left_height : right_height);

    return node;
}

// Search for a key in the tree
TreeNode* search_node(TreeNode* node, int key) {
    stats.search_count++;

    if (node == NULL) {
        printf("Key %d not found\n", key);
        return NULL;
    }

    if (key == node->key) {
        printf("Found key %d with value '%s'\n", key, node->value);
        return node;
    }

    if (key < node->key) {
        return search_node(node->left, key);
    } else {
        return search_node(node->right, key);
    }
}

// Find minimum node in a subtree
TreeNode* find_min(TreeNode* node) {
    if (node == NULL) return NULL;

    while (node->left != NULL) {
        node = node->left;
    }

    return node;
}

// Find maximum node in a subtree
TreeNode* find_max(TreeNode* node) {
    if (node == NULL) return NULL;

    while (node->right != NULL) {
        node = node->right;
    }

    return node;
}

// Delete a node from the tree
TreeNode* delete_node(TreeNode* node, int key) {
    if (node == NULL) {
        printf("Key %d not found for deletion\n", key);
        return node;
    }

    if (key < node->key) {
        node->left = delete_node(node->left, key);
    } else if (key > node->key) {
        node->right = delete_node(node->right, key);
    } else {
        // Node to be deleted found
        printf("Deleting key %d with value '%s'\n", key, node->value);
        stats.delete_count++;
        stats.total_nodes--;

        // Case 1: Node with only right child or no children
        if (node->left == NULL) {
            TreeNode* temp = node->right;
            free(node);
            return temp;
        }
        // Case 2: Node with only left child
        else if (node->right == NULL) {
            TreeNode* temp = node->left;
            free(node);
            return temp;
        }

        // Case 3: Node with two children
        TreeNode* temp = find_min(node->right);

        // Copy the inorder successor's key and value
        node->key = temp->key;
        strcpy(node->value, temp->value);

        // Delete the inorder successor
        node->right = delete_node(node->right, temp->key);
    }

    // Update height
    int left_height = node->left ? node->left->height : 0;
    int right_height = node->right ? node->right->height : 0;
    node->height = 1 + (left_height > right_height ? left_height : right_height);

    return node;
}

// Calculate tree height
int calculate_height(TreeNode* node) {
    if (node == NULL) return 0;

    int left_height = calculate_height(node->left);
    int right_height = calculate_height(node->right);

    return 1 + (left_height > right_height ? left_height : right_height);
}

// Check if tree is balanced (height difference <= 1)
int is_balanced(TreeNode* node, int* height) {
    if (node == NULL) {
        *height = 0;
        return 1;
    }

    int left_height = 0, right_height = 0;

    int left_balanced = is_balanced(node->left, &left_height);
    int right_balanced = is_balanced(node->right, &right_height);

    *height = 1 + (left_height > right_height ? left_height : right_height);

    int height_diff = abs(left_height - right_height);

    return left_balanced && right_balanced && (height_diff <= 1);
}

// Inorder traversal (Left, Root, Right)
void inorder_traversal(TreeNode* node) {
    if (node != NULL) {
        inorder_traversal(node->left);
        printf("(%d:'%s') ", node->key, node->value);
        inorder_traversal(node->right);
    }
}

// Preorder traversal (Root, Left, Right)
void preorder_traversal(TreeNode* node) {
    if (node != NULL) {
        printf("(%d:'%s') ", node->key, node->value);
        preorder_traversal(node->left);
        preorder_traversal(node->right);
    }
}

// Postorder traversal (Left, Right, Root)
void postorder_traversal(TreeNode* node) {
    if (node != NULL) {
        postorder_traversal(node->left);
        postorder_traversal(node->right);
        printf("(%d:'%s') ", node->key, node->value);
    }
}

// Level order traversal (breadth-first)
void level_order_traversal(TreeNode* node) {
    if (node == NULL) return;

    // Simple queue implementation using array
    TreeNode* queue[MAX_NODES];
    int front = 0, rear = 0;

    queue[rear++] = node;

    while (front < rear) {
        TreeNode* current = queue[front++];
        printf("(%d:'%s') ", current->key, current->value);

        if (current->left) queue[rear++] = current->left;
        if (current->right) queue[rear++] = current->right;
    }
}

// Print tree structure visually
void print_tree_structure(TreeNode* node, int depth, char* prefix) {
    if (node == NULL) return;

    if (depth == 0) {
        printf("Tree Structure:\n");
        printf("%s(%d:'%s')\n", prefix, node->key, node->value);
    }

    char new_prefix[256];
    snprintf(new_prefix, sizeof(new_prefix), "%s  ", prefix);

    if (node->left) {
        printf("%s├─L─(%d:'%s')\n", prefix, node->left->key, node->left->value);
        print_tree_structure(node->left, depth + 1, new_prefix);
    }

    if (node->right) {
        printf("%s└─R─(%d:'%s')\n", prefix, node->right->key, node->right->value);
        print_tree_structure(node->right, depth + 1, new_prefix);
    }
}

// Free all nodes in the tree
void free_tree(TreeNode* node) {
    if (node != NULL) {
        free_tree(node->left);
        free_tree(node->right);
        free(node);
    }
}

// Show tree statistics
void show_statistics() {
    printf("\n=== Binary Tree Statistics ===\n");
    printf("Total nodes: %d\n", stats.total_nodes);
    printf("Tree height: %d\n", calculate_height(root));

    if (stats.total_nodes > 0) {
        TreeNode* min_node = find_min(root);
        TreeNode* max_node = find_max(root);
        printf("Minimum key: %d ('%s')\n", min_node->key, min_node->value);
        printf("Maximum key: %d ('%s')\n", max_node->key, max_node->value);

        int height = 0;
        int balanced = is_balanced(root, &height);
        printf("Balanced: %s\n", balanced ? "Yes" : "No");
    }

    printf("Operations performed:\n");
    printf("  Insertions: %d\n", stats.insert_count);
    printf("  Deletions: %d\n", stats.delete_count);
    printf("  Searches: %d\n", stats.search_count);

    if (stats.created > 0) {
        printf("Tree age: %d seconds\n", (int)(time(NULL) - stats.created));
    }
}

// Save tree to file (preorder format)
void save_tree_to_file(TreeNode* node, FILE* file) {
    if (node != NULL) {
        fprintf(file, "%d %s\n", node->key, node->value);
        save_tree_to_file(node->left, file);
        save_tree_to_file(node->right, file);
    }
}

// Load tree from file
int load_tree_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file '%s' for reading\n", filename);
        return 0;
    }

    printf("Loading tree from '%s'...\n", filename);
    int key;
    char value[MAX_VALUE_LEN];
    int count = 0;

    while (fscanf(file, "%d %63s", &key, value) == 2) {
        root = insert_node(root, key, value);
        count++;
    }

    fclose(file);
    printf("Loaded %d nodes from file\n", count);
    return count;
}

// Performance benchmark
void run_benchmark(int operations) {
    printf("Binary Tree Performance Benchmark\n");
    printf("=================================\n");
    printf("Running %d operations...\n\n", operations);

    // Clear existing tree
    if (root) {
        free_tree(root);
        root = NULL;
        memset(&stats, 0, sizeof(stats));
        stats.created = time(NULL);
    }

    clock_t start = clock();

    // Insert phase
    printf("Insert phase: ");
    for (int i = 0; i < operations / 2; i++) {
        int key = rand() % (operations * 2);  // Allow some duplicates
        char value[32];
        snprintf(value, sizeof(value), "val_%d", key);
        root = insert_node(root, key, value);
        if (i % 100 == 0) printf(".");
    }
    printf(" done\n");

    clock_t insert_end = clock();

    // Search phase
    printf("Search phase: ");
    for (int i = 0; i < operations / 4; i++) {
        int key = rand() % (operations * 2);
        search_node(root, key);
        if (i % 50 == 0) printf(".");
    }
    printf(" done\n");

    clock_t search_end = clock();

    // Delete phase
    printf("Delete phase: ");
    for (int i = 0; i < operations / 4; i++) {
        int key = rand() % (operations * 2);
        root = delete_node(root, key);
        if (i % 50 == 0) printf(".");
    }
    printf(" done\n");

    clock_t end = clock();

    double insert_time = ((double)(insert_end - start)) / CLOCKS_PER_SEC;
    double search_time = ((double)(search_end - insert_end)) / CLOCKS_PER_SEC;
    double delete_time = ((double)(end - search_end)) / CLOCKS_PER_SEC;
    double total_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\nBenchmark Results:\n");
    printf("==================\n");
    printf("Total operations: %d\n", operations);
    printf("Insert time: %.4f seconds (%d ops)\n", insert_time, operations / 2);
    printf("Search time: %.4f seconds (%d ops)\n", search_time, operations / 4);
    printf("Delete time: %.4f seconds (%d ops)\n", delete_time, operations / 4);
    printf("Total time: %.4f seconds\n", total_time);
    printf("Operations per second: %.0f\n", operations / total_time);

    show_statistics();
}

// Run comprehensive tests
void run_tests() {
    printf("Binary Tree Test Suite\n");
    printf("======================\n\n");

    stats.created = time(NULL);

    // Test 1: Basic insertion
    printf("--- Test 1: Basic Insertion ---\n");
    root = insert_node(root, 50, "root");
    root = insert_node(root, 30, "left_child");
    root = insert_node(root, 70, "right_child");
    root = insert_node(root, 20, "left_left");
    root = insert_node(root, 40, "left_right");
    root = insert_node(root, 60, "right_left");
    root = insert_node(root, 80, "right_right");

    print_tree_structure(root, 0, "");

    // Test 2: Search operations
    printf("\n--- Test 2: Search Operations ---\n");
    search_node(root, 40);
    search_node(root, 90);  // Not found
    search_node(root, 20);

    // Test 3: Traversals
    printf("\n--- Test 3: Tree Traversals ---\n");
    printf("Inorder:   ");
    inorder_traversal(root);
    printf("\n");

    printf("Preorder:  ");
    preorder_traversal(root);
    printf("\n");

    printf("Postorder: ");
    postorder_traversal(root);
    printf("\n");

    printf("Level-order: ");
    level_order_traversal(root);
    printf("\n");

    // Test 4: Min/Max operations
    printf("\n--- Test 4: Min/Max Operations ---\n");
    TreeNode* min_node = find_min(root);
    TreeNode* max_node = find_max(root);
    printf("Minimum: %d ('%s')\n", min_node->key, min_node->value);
    printf("Maximum: %d ('%s')\n", max_node->key, max_node->value);

    // Test 5: Balance check
    printf("\n--- Test 5: Balance Analysis ---\n");
    int height = 0;
    int balanced = is_balanced(root, &height);
    printf("Tree height: %d\n", height);
    printf("Is balanced: %s\n", balanced ? "Yes" : "No");

    // Test 6: Deletion
    printf("\n--- Test 6: Deletion Operations ---\n");
    root = delete_node(root, 20);  // Leaf node
    root = delete_node(root, 30);  // Node with two children
    root = delete_node(root, 90);  // Non-existent node

    printf("After deletions:\n");
    printf("Inorder: ");
    inorder_traversal(root);
    printf("\n");

    // Test 7: File I/O
    printf("\n--- Test 7: File I/O ---\n");
    FILE* save_file = fopen("tree_test.dat", "w");
    if (save_file) {
        save_tree_to_file(root, save_file);
        fclose(save_file);
        printf("Tree saved to 'tree_test.dat'\n");
    }

    show_statistics();

    printf("\nBinary tree tests completed successfully!\n");
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));

    if (argc < 2) {
        printf("Binary Tree Data Structure\n");
        printf("==========================\n\n");
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage();
        return 0;
    }

    if (strcmp(argv[1], "--test") == 0) {
        run_tests();
        goto cleanup;
    }

    // Initialize tree
    if (stats.created == 0) {
        stats.created = time(NULL);
    }

    if (strcmp(argv[1], "--insert") == 0) {
        if (argc < 4) {
            printf("Error: --insert requires key and value\n");
            return 1;
        }
        int key = atoi(argv[2]);
        root = insert_node(root, key, argv[3]);
    }
    else if (strcmp(argv[1], "--search") == 0) {
        if (argc < 3) {
            printf("Error: --search requires key\n");
            return 1;
        }
        int key = atoi(argv[2]);
        search_node(root, key);
    }
    else if (strcmp(argv[1], "--delete") == 0) {
        if (argc < 3) {
            printf("Error: --delete requires key\n");
            return 1;
        }
        int key = atoi(argv[2]);
        root = delete_node(root, key);
    }
    else if (strcmp(argv[1], "--print") == 0) {
        if (root) {
            print_tree_structure(root, 0, "");
        } else {
            printf("Tree is empty\n");
        }
    }
    else if (strcmp(argv[1], "--stats") == 0) {
        show_statistics();
    }
    else if (strcmp(argv[1], "--min") == 0) {
        TreeNode* min_node = find_min(root);
        if (min_node) {
            printf("Minimum: %d ('%s')\n", min_node->key, min_node->value);
        } else {
            printf("Tree is empty\n");
        }
    }
    else if (strcmp(argv[1], "--max") == 0) {
        TreeNode* max_node = find_max(root);
        if (max_node) {
            printf("Maximum: %d ('%s')\n", max_node->key, max_node->value);
        } else {
            printf("Tree is empty\n");
        }
    }
    else if (strcmp(argv[1], "--height") == 0) {
        printf("Tree height: %d\n", calculate_height(root));
    }
    else if (strcmp(argv[1], "--balance") == 0) {
        int height = 0;
        int balanced = is_balanced(root, &height);
        printf("Tree height: %d\n", height);
        printf("Is balanced: %s\n", balanced ? "Yes" : "No");
    }
    else if (strcmp(argv[1], "--traverse") == 0) {
        if (argc < 3) {
            printf("Error: --traverse requires type (inorder, preorder, postorder, level)\n");
            return 1;
        }

        if (root == NULL) {
            printf("Tree is empty\n");
        } else {
            printf("Traversal (%s): ", argv[2]);

            if (strcmp(argv[2], "inorder") == 0) {
                inorder_traversal(root);
            } else if (strcmp(argv[2], "preorder") == 0) {
                preorder_traversal(root);
            } else if (strcmp(argv[2], "postorder") == 0) {
                postorder_traversal(root);
            } else if (strcmp(argv[2], "level") == 0) {
                level_order_traversal(root);
            } else {
                printf("Unknown traversal type: %s\n", argv[2]);
                return 1;
            }
            printf("\n");
        }
    }
    else if (strcmp(argv[1], "--benchmark") == 0) {
        int operations = 1000;
        if (argc > 2) {
            operations = atoi(argv[2]);
            if (operations < 10 || operations > 100000) {
                printf("Error: Operations must be between 10 and 100,000\n");
                return 1;
            }
        }
        run_benchmark(operations);
        goto cleanup;
    }
    else if (strcmp(argv[1], "--load") == 0) {
        if (argc < 3) {
            printf("Error: --load requires filename\n");
            return 1;
        }
        load_tree_from_file(argv[2]);
    }
    else if (strcmp(argv[1], "--save") == 0) {
        if (argc < 3) {
            printf("Error: --save requires filename\n");
            return 1;
        }
        FILE* file = fopen(argv[2], "w");
        if (file) {
            save_tree_to_file(root, file);
            fclose(file);
            printf("Tree saved to '%s'\n", argv[2]);
        } else {
            printf("Error: Cannot create file '%s'\n", argv[2]);
            return 1;
        }
    }
    else {
        printf("Unknown command: %s\n", argv[1]);
        print_usage();
        return 1;
    }

cleanup:
    // Clean up memory
    if (root) {
        free_tree(root);
    }

    return 0;
}