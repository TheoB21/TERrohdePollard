#include "set.h"

#include <stdio.h>
#include <stdlib.h>

// An AVL tree node

// A utility function to get maximum of two integers
int max(int a, int b);

// A utility function to get height of the tree
int height(node *N) {
  if (N == NULL) return 0;
  return N->height;
}

// A utility function to get maximum of two integers
int max(int a, int b) { return (a > b) ? a : b; }

/* Helper function that allocates a new node with the given key and
    NULL left and right pointers. */
node *newNode(const mpz_t key, const mpz_t n, const mpz_t m) {
  node *node_ = (node *)malloc(sizeof(node));
  mpz_init(node_->key);
  mpz_set(node_->key, key);
  mpz_init(node_->n);
  mpz_set(node_->n, n);
  mpz_init(node_->m);
  mpz_set(node_->m, m);
  node_->left = NULL;
  node_->right = NULL;
  node_->height = 1;  // new node is initially added at leaf
  return (node_);
}

// A utility function to right rotate subtree rooted with y
// See the diagram given above.
node *rightRotate(node *y) {
  node *x = y->left;
  node *T2 = x->right;

  // Perform rotation
  x->right = y;
  y->left = T2;

  // Update heights
  y->height = max(height(y->left), height(y->right)) + 1;
  x->height = max(height(x->left), height(x->right)) + 1;

  // Return new root
  return x;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
node *leftRotate(node *x) {
  node *y = x->right;
  node *T2 = y->left;

  // Perform rotation
  y->left = x;
  x->right = T2;

  //  Update heights
  x->height = max(height(x->left), height(x->right)) + 1;
  y->height = max(height(y->left), height(y->right)) + 1;

  // Return new root
  return y;
}

// Get Balance factor of node N
int getBalance(node *N) {
  if (N == NULL) return 0;
  return height(N->left) - height(N->right);
}

node *insert(node *node, const mpz_t key, const mpz_t n, const mpz_t m) {
  /* 1.  Perform the normal BST rotation */
  if (node == NULL) return (newNode(key, n, m));

  if (mpz_cmp(key, node->key) < 0)
    node->left = insert(node->left, key, n, m);
  else if (mpz_cmp(key, node->key) > 0)
    node->right = insert(node->right, key, n, m);
  else  // Equal keys not allowed
    return node;

  /* 2. Update height of this ancestor node */
  node->height = 1 + max(height(node->left), height(node->right));

  /* 3. Get the balance factor of this ancestor
        node to check whether this node became
        unbalanced */
  int balance = getBalance(node);

  // If this node becomes unbalanced, then there are 4 cases

  // Left Left Case
  if (balance > 1 && mpz_cmp(key, node->left->key) < 0)
    return rightRotate(node);

  // Right Right Case
  if (balance < -1 && mpz_cmp(key, node->right->key) > 0)
    return leftRotate(node);

  // Left Right Case
  if (balance > 1 && mpz_cmp(key, node->left->key) > 0) {
    node->left = leftRotate(node->left);
    return rightRotate(node);
  }

  // Right Left Case
  if (balance < -1 && mpz_cmp(key, node->right->key) < 0) {
    node->right = rightRotate(node->right);
    return leftRotate(node);
  }

  /* return the (unchanged) node pointer */
  return node;
}

node *search(node *root, mpz_t val) {
  if (root == NULL) {
    return NULL;
  }
  if (mpz_cmp(root->key, val) == 0) {
    return root;
  }
  if (mpz_cmp(root->key, val) < 0) {
    return search(root->right, val);
  }
  return search(root->left, val);
}

/* Given a non-empty binary search tree, return the
   node with minimum key value found in that tree.
   Note that the entire tree does not need to be
   searched. */
node *minValueNode(node *node_) {
  node *current = node_;

  /* loop down to find the leftmost leaf */
  while (current->left != NULL) current = current->left;

  return current;
}

// A utility function to print preorder traversal of
// the tree.
// The function also prints height of every node
void preOrder(node *root) {
  if (root != NULL) {
    printf("(");
    PRINT(root->key);
    printf("::");
    PRINT(root->m);
    printf("::");
    PRINT(root->n);
    printf(")");

    preOrder(root->left);
    preOrder(root->right);
  }
}

void deleteTree(node *root) {
  if (root == NULL) return;
  deleteTree(root->right);
  deleteTree(root->left);
  mpz_clear(root->key);
  mpz_clear(root->m);
  mpz_clear(root->n);
  free(root);
}
