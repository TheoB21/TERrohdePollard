#ifndef TAGTRACING_H
#define TAGTRACING_H

#include <gmp.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define D 64
#define B_p 256
#define EPSILON 65536
#define OMEGA_p 4194304
#define OMEGA 4294967296
#define T 1024

typedef struct {
  mpz_t m; /*exposant de  g  m <-> g */
  mpz_t n; /*exposant de  h  n <-> h */
  mpz_t M; /*  Ms=g^m*h^n */
} element;

typedef struct {
  mpz_t m; /*exposant de  g  m <-> g */
  mpz_t n; /*exposant de  h  m <-> h */
} couple;

typedef struct tree_M {
  element value;
  unsigned long int v[D];  // tableau
  struct tree_M**
      childrens;      /*  ces enfant sous la forme de liste de référerence ces
                         enfants (donc ** pour dire list de référence) */
  int size_childrens; /* nombre d'noeuds*/
} treeM_l;

typedef struct s_linked_list {
  unsigned int value;
  struct s_linked_list* next;
} linked_list;

treeM_l* generate_tree(int r, int l, element* listM, mpz_t q, mpz_t p);

void delete_tree(treeM_l* Tree);

unsigned long int tau__(mpz_t g, unsigned long int* M_v);
unsigned long int tau_(mpz_t g, unsigned long int* M_v);
unsigned long int tau(mpz_t g, mpz_t B, mpz_t p, mpz_t tmp);

linked_list* add_to_list(unsigned long int s, linked_list* head);

void delete_list(linked_list* head);

treeM_l* find_node(treeM_l* root, linked_list* head);

#endif /* MODULE_H */

/*** end of file ***/
