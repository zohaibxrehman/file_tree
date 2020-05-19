#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "ftree.h"
#include <string.h>

struct TreeNode *generate_ftree_helper(const char *fname, char *path) {
    // collect file statistics at full_path into stat_buf
    struct stat stat_buf;
    char full_path[strlen(path) + strlen(fname) + 2]; 
    strcpy(full_path, path);
    strcat(full_path, "/");
    strcat(full_path, fname);
    if(lstat(full_path, &stat_buf) == -1) {
        exit(1);
    }

    // initialize a node for file/dir at full_path
    struct TreeNode *node = malloc(sizeof(struct TreeNode));
    if (node == NULL) {
        exit(1);
    }
    node->fname = malloc(strlen(fname) + 1);
    if (node->fname == NULL) {
        exit(1);
    }
    strcpy(node->fname, fname);
    node->permissions = stat_buf.st_mode & 0777;
    node->next = NULL;
    node->contents = NULL;
    
    if(S_ISREG(stat_buf.st_mode)) { 
        // Regular files
        node->type = '-';
    } else if(S_ISLNK(stat_buf.st_mode)) { 
        // Links
        node->type = 'l';
    } else if (S_ISDIR(stat_buf.st_mode)) { 
        // Directories
        node->type = 'd';
       
        DIR *d_ptr = opendir(full_path);
        if (d_ptr == NULL) {
           exit(1);
        }

        struct dirent *entry_ptr;
        entry_ptr = readdir(d_ptr);

        struct TreeNode *curr;
        struct TreeNode *prev_curr;
        int first_element = 1;
        while(entry_ptr != NULL) {
            if((entry_ptr->d_name)[0] != '.') {
                curr = generate_ftree_helper(entry_ptr->d_name, full_path);
                if(first_element) {
                    node->contents = curr;
                    first_element = 0;
                } else {
                    prev_curr->next = curr;
                }
                prev_curr = curr;
            } 

            entry_ptr = readdir(d_ptr);
        }
        closedir(d_ptr);
    }
    return node;
}


/*
 * Returns the FTree rooted at the path fname.
 *
 * Use the following if the file fname doesn't exist and return NULL:
 * fprintf(stderr, "The path (%s) does not point to an existing entry!\n", fname);
 *
 */
struct TreeNode *generate_ftree(const char *fname) {
    struct stat stat_buf;
    if(lstat(fname, &stat_buf) == -1) {
        fprintf(stderr, "The path (%s) does not point to an existing entry!\n", fname);
        return NULL;
    }
    // helper function used to recursively build path-names as traversing through directories
    return generate_ftree_helper(fname, ".");
}


/*
 * Prints the TreeNodes encountered on a preorder traversal of an FTree.
 *
 * The only print statements that you may use in this function are:
 * printf("===== %s (%c%o) =====\n", root->fname, root->type, root->permissions)
 * printf("%s (%c%o)\n", root->fname, root->type, root->permissions)
 *
 */
void print_ftree(struct TreeNode *root) {
    static int depth = 0;
    printf("%*s", depth * 2, "");

    if(root->type == '-' || root->type == 'l') { 
        // Printing file info
        printf("%s (%c%o)\n", root->fname, root->type, root->permissions);
    } else if(root->type == 'd') { 
        // Printing Directory info and its contents
        printf("===== %s (%c%o) =====\n", root->fname, root->type, root->permissions);
        struct TreeNode *curr = root->contents;
        depth += 1;
        while(curr != NULL) {
            print_ftree(curr);
            curr = curr->next;
        }
        depth -= 1;
    }
}


/* 
 * Deallocate all dynamically-allocated memory in the FTree rooted at node.
 * 
 */
void deallocate_ftree (struct TreeNode *node) {
    if(node->type == '-' || node->type == 'l') { 
        // deallocating file node
        free(node->fname);
        free(node);
    } else if(node->type == 'd') { 
        // deallocating directory node and its content
        struct TreeNode *curr = node->contents;
        struct TreeNode *curr_next = curr;
        // deallocating done using bottom up approach
        while(curr != NULL) {
            curr_next = curr->next;
            deallocate_ftree(curr);
            curr = curr_next;
        }
        free(node->fname);
        free(node);
    }
}
