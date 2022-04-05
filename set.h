#ifndef SETMPZ_H_INCLUDED
#define SETMPZ_H_INCLUDED

#include <gmp.h>

typedef struct Node {
  mpz_t key;
  mpz_t n;
  mpz_t m;
  struct Node* left;
  struct Node* right;
  int height;
} node;

#define PRINT(var) mpz_out_str(stdout, 10, var)

node* search(node* root, mpz_t val);
node* insert(node* node, const mpz_t key, const mpz_t n, const mpz_t m);
void preOrder(node* root);
void deleteTree(node* root);

#endif  // SETMPZ_H_INCLUDED
