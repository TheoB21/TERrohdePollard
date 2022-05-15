#include "tagTracing.h"

treeM_l* build_aux(mpz_t q, mpz_t p, element M_prev, int r_curr, int l_curr,
                   int r, element* listM, mpz_t B) {
  if (l_curr == 0) {
    return NULL;
  }
  /* allocation du noeud */
  treeM_l* node = malloc(sizeof(treeM_l));

  /* Construction du noeud */

  /* first init value */
  mpz_init(node->value.M);
  mpz_init(node->value.m);
  mpz_init(node->value.n);
  mpz_mul(node->value.M, listM[r_curr].M, M_prev.M);
  mpz_mod(node->value.M, node->value.M, p);

  mpz_add(node->value.m, listM[r_curr].m, M_prev.m);
  mpz_mod(node->value.m, node->value.m, q);

  mpz_add(node->value.n, listM[r_curr].n, M_prev.n);
  mpz_mod(node->value.n, node->value.n, q);
  /* second init V as defined in the article */

  mpz_t tmp;  // tmp vaut epsilon^i dans la boucle for
  mpz_init(tmp);
  mpz_set_ui(tmp, 1UL);
  mpz_t tmp2;
  mpz_init(tmp2);

  for (int i = 0; i < D; i++) {
    /* compute floor((eplsion^i  * M (mod p) ) /B) */
    mpz_mul(tmp2, node->value.M, tmp);
    mpz_mod(tmp2, tmp2, p);
    mpz_fdiv_q(tmp2, tmp2, B);
    node->v[i] = mpz_get_ui(tmp2);
    /* update tmp */
    mpz_mul_si(tmp, tmp, EPSILON);
    mpz_mod(tmp, tmp, p);
  }
  mpz_clear(tmp);
  mpz_clear(tmp2);

  /* third the childrens nodes */
  int size_c = r - r_curr;
  node->size_childrens = size_c;
  /* allocate the array of reference to the childrens */
  node->childrens = malloc(size_c * sizeof(treeM_l*));
  for (int i = r_curr; i < r; i++)
    node->childrens[i - r_curr] =
        build_aux(q, p, node->value, i, l_curr - 1, r, listM, B);

  return node;
}

treeM_l* generate_tree(int r, int l, element* listM, mpz_t p, mpz_t q) {
  treeM_l* root = malloc(sizeof(treeM_l));
  mpz_init(root->value.M);
  mpz_init(root->value.n);
  mpz_init(root->value.m);
  mpz_set_ui(root->value.M, 1UL);
  /* init B */
  mpz_t B;
  mpz_init(B);
  mpz_cdiv_q_ui(B, p, OMEGA);
  // printf("B : ");
  // mpz_out_str(stdout, 10, B);
  // printf("\n");
  /* the array */
  root->childrens = malloc(r * sizeof(treeM_l*));
  root->size_childrens = r;

  for (int i = 0; i < r; i++)
    root->childrens[i] = build_aux(q, p, root->value, i, l, r, listM, B);
  mpz_clear(B);
  return root;
}

void delete_tree(treeM_l* node) {
  if (node == NULL) return;
  for (int i = 0; i < node->size_childrens; i++)
    delete_tree(node->childrens[i]);
  mpz_clear(node->value.M);
  mpz_clear(node->value.m);
  mpz_clear(node->value.n);

  free(node->childrens);
}

unsigned long int tau__(mpz_t g, unsigned long int* M_v) {
  unsigned long long int k = 0;
  unsigned long int t;
  unsigned long int x;

  for (int j = 0; j < g->_mp_size; j++) {
    t = g->_mp_d[j];
    for (int w = 0; w < 4; w++) {
      x = t % EPSILON;
      k = (k + (x * M_v[4 * j + w]) % OMEGA) % OMEGA;
      t = t >> 16;
    }
  }
  return k / OMEGA_p;
}

unsigned long int tau_(mpz_t g, unsigned long int* M_v) {
  unsigned long int k = tau__(g, M_v);
  return (k % B_p == B_p - 1 || k % B_p == B_p - 2) ? B_p : k;
}

linked_list* add_to_list(unsigned long int s, linked_list* head) {
  linked_list* new_node = malloc(sizeof(linked_list));
  new_node->value = (s);
  if (head == NULL || s <= head->value) {
    new_node->next = head;
    return new_node;
  }
  linked_list* current_node = head;
  while (current_node->next != NULL &&
         current_node->next->value < new_node->value) {
    current_node = current_node->next;
  }
  new_node->next = current_node->next;
  current_node->next = new_node;
  return head;
}

void delete_list(linked_list* head) {
  linked_list* tmp;
  while (head != NULL) {
    tmp = head->next;
    free(head);
    head = tmp;
  }
}

treeM_l* find_node(treeM_l* root, linked_list* head) {
  int curr_r = 0;
  int index;
  while (head != NULL) {
    index = head->value - curr_r;
    root = root->childrens[index];
    curr_r = head->value;
    head = head->next;
  }
  return root;
}

unsigned long int tau(mpz_t g, mpz_t B, mpz_t p, mpz_t tmp) {
  mpz_mod(tmp, g, p);
  mpz_fdiv_q(tmp, tmp, B);
  unsigned long int k = mpz_get_ui(tmp);
  return k;
}
