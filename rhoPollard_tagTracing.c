// Pour compiler : gcc -g -Wall TER.c -lgmp

#include <assert.h>
#include <gmp.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "set.h"
#include "tagTracing.h"

#define SIZE_P 1024  // taille de p
#define R 7          // r de r-adding walk
#define L 20         // l de la table ({M_s}U{1})^l
#define SIZE_DISTINGUE 1023
// en moyenne on stock (cst*sqrt(q))/2^x élement dans ta la table ou
//(x= Nb de bit de p -size distingué)
// Car theta, la proprotion de distingué qu'on sotck
// c'est 1/(2^x) car on stock que les élement qui se code sur size_distingué
// On a en moyenne (cst*sqrt(q)) itération (c'est la complexité de la méthode)
// donc on prend le nombre d'itération et en moyenne on stock une proporation
// theta des résultat
#define N_THREADS 1  // nombre de thread
#define I_THETA \
  (1 << (SIZE_P - SIZE_DISTINGUE))  // Nb de bit de p -size distingué

#define INIT(name, value) \
  mpz_init(name);         \
  mpz_set_str(name, value, 10);

#define PRINT2(var)             \
  mpz_out_str(stdout, 10, var); \
  printf("\n");

/* compteur de la taille du l'arbre */

int cpt = 0;
unsigned long int itter = 0;

/* the struct shared by all threads*/
typedef struct {
  node *set;                /* Ensemble ou les thread mettent les éléments*/
  couple collision_1;       /* element 1 de la collision */
  couple collision_2;       /* element 2 de la collision */
  pthread_mutex_t lock_set; /* lock pour la table (set) et collision1/2 */
  pthread_mutex_t lock_signal; /* lock pour la condition variable et
                                  collision_1/collision_2 */
  pthread_cond_t signalFinish; /* indique quand la collision a été trouvé  */
  mpz_t p;
  mpz_t q;
  mpz_t g;
  mpz_t h;
  element listM[R];
  treeM_l *root;
} thread_struct;

void *f_dinstingue(thread_struct *s_struct) {
  /* INIT */
  mpz_t y, a, b, n1, m1, tmp1, tmp2, tmp3;
  mpz_init(a);
  mpz_init(b);
  mpz_init(y);
  gmp_randstate_t state;
  gmp_randinit_mt(state);
  node *node_Tmp;
  int borne = 1000000;  // just a definition

  mpz_init(n1);
  mpz_init(m1);
  mpz_init(tmp1);
  mpz_init(tmp2);
  mpz_init(tmp3);
  // new
  linked_list *l;
  treeM_l *node_curr;
  unsigned long int s, tag;
  mpz_t B;
  mpz_init(B);
  mpz_cdiv_q_ui(B, s_struct->p, OMEGA);
  mpz_mul_ui(B, B, OMEGA_p);

  while (true) {
    mpz_urandomm(n1, state, s_struct->q);
    mpz_urandomm(m1, state, s_struct->q);
    mpz_powm(tmp1, s_struct->g, m1, s_struct->p); /* tmp1 = g^m1 (mod p)*/
    mpz_powm(tmp2, s_struct->h, n1, s_struct->p); /* tmp1 = h^n1 (mod p)*/
    mpz_mul(y, tmp1, tmp2);
    mpz_mod(y, y, s_struct->p);
    // mpz_mod(y, y, s_struct->p);
    tag = tau(y, B, s_struct->p, tmp3);
    mpz_set(a, m1);
    mpz_set(b, n1);
    for (int i = 0; i < borne; i++) {
      /* faire un tag tracing */
      l = NULL;
      node_curr = s_struct->root;
      s = tag / B_p;
      for (int j = 0; j < L; j++) {
        // itter++;
        l = add_to_list(s, l);
        node_curr = find_node(s_struct->root, l);
        tag = tau__(y, node_curr->v);
        if (tag % B_p == B_p - 1 || tag % B_p == B_p - 2) break;
        s = tag / B_p;
        // printf("the value of j : %d\n", j);
      }
      delete_list(l);
      mpz_add(b, b, node_curr->value.n);
      mpz_mod(b, b, s_struct->q);
      mpz_add(a, a, node_curr->value.m);
      mpz_mod(a, a, s_struct->q);
      mpz_mul(y, y, node_curr->value.M);
      mpz_mod(y, y, s_struct->p);
      tag = tau(y, B, s_struct->p, tmp3);

      // printf("here is y : ");
      // mpz_out_str(stdout, 10, y);
      // tag = tau(y, B, s_struct->p);
      // printf("tau : %lu  and b-1 : %d \n", tag, B_p - 1);

      /* regarder si c'est un point distingué */
      // printf("SIZE OF THE POINT : %ld \n here is Y :", mpz_sizeinbase(y, 2));
      // PRINT2(y)
      // if (tag % B_p == B_p - 1)
      //   printf("the size of y : %ld , \n ", mpz_sizeinbase(y, 2));
      if (tag % B_p == B_p - 1 && mpz_sizeinbase(y, 2) <= SIZE_DISTINGUE) {
        pthread_mutex_lock(&s_struct->lock_set);
        // if (cpt % 100 == 0) {
        //   printf("number of element in the table so far : %d \n", cpt);
        //   // printf("here is y so far : ");
        //   // PRINT2(y);
        //   // printf("here is m so far : ");
        //   // PRINT2(a);
        //   // printf("here is n so far : ");
        //   // PRINT2(b);
        // }
        node_Tmp = search(s_struct->set, y);
        if (node_Tmp != NULL) {
          if (mpz_cmp(b, node_Tmp->n) == 0) {
            pthread_mutex_unlock(&s_struct->lock_set);
            break;
          }
          mpz_set(s_struct->collision_1.m, a);
          mpz_set(s_struct->collision_1.n, b);
          mpz_set(s_struct->collision_2.n, node_Tmp->n);
          mpz_set(s_struct->collision_2.m, node_Tmp->m);
          pthread_mutex_lock(&s_struct->lock_signal);
          pthread_cond_broadcast(&s_struct->signalFinish);
          pthread_mutex_unlock(&s_struct->lock_signal);
          return NULL;
        }
        s_struct->set = insert(s_struct->set, y, b, a);
        cpt++;
        pthread_mutex_unlock(&s_struct->lock_set);
      }
    }
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  FILE *in = fopen("input.txt", "r");
  if (in == NULL) {
    printf("Error open file IN\n");
    exit(EXIT_FAILURE);
  }

  // while (!feof(in))
  //{
  char temp_g[1000];
  char temp_h[1000];
  char temp_p[1000];
  char temp_q[1000];

  // while (!feof(in))
  //{
  struct timeval start, end;

  fscanf(in, "%s %s %s %s", temp_g, temp_h, temp_p, temp_q);
  thread_struct sharedStruct; /* shared struct */
  sharedStruct.set = NULL;
  /* génerateur g de G */
  INIT(sharedStruct.g, temp_g)
  /* h tel que l'on cherche log_g(h) */
  INIT(sharedStruct.h, temp_h)
  /* l'ordre du grand groupe */
  INIT(sharedStruct.p, temp_p)
  /* ordre du sous-groupe G */
  INIT(sharedStruct.q, temp_q)

  PRINT2(sharedStruct.h)
  // PRINT2(sharedStruct.p)
  // PRINT2(sharedStruct.q)

  /* generation des ms et ns de façon aléatoire */
  gmp_randstate_t state;
  gmp_randinit_mt(state);
  unsigned long int seed = time(NULL);
  gmp_randseed_ui(state, seed);
  mpz_t g_init;
  mpz_t h_init;
  mpz_init(g_init);
  mpz_init(h_init);
  for (int i = 0; i < R; i++) {
    mpz_init(sharedStruct.listM[i].m);
    mpz_urandomm(sharedStruct.listM[i].m, state,
                 sharedStruct.q); /* defined mod q */
    mpz_init(sharedStruct.listM[i].n);
    mpz_urandomm(sharedStruct.listM[i].n, state, sharedStruct.q);
    mpz_powm(g_init, sharedStruct.g, sharedStruct.listM[i].m, sharedStruct.p);
    mpz_powm(h_init, sharedStruct.h, sharedStruct.listM[i].n, sharedStruct.p);
    mpz_init(sharedStruct.listM[i].M);

    mpz_mul(sharedStruct.listM[i].M, g_init, h_init);
    mpz_mod(sharedStruct.listM[i].M, sharedStruct.listM[i].M, sharedStruct.p);
    printf("here is M%d : ", i);
    PRINT2(sharedStruct.listM[i].M)
    printf("\n");
  }

  mpz_clear(g_init);
  mpz_clear(h_init);
  gmp_randclear(state);

  /* INIT DE M_L */
  sharedStruct.root =
      generate_tree(R, L, sharedStruct.listM, sharedStruct.p, sharedStruct.q);
  printf("the table has been generated \n");
  /* START JUNK */

  // printf("the table has been generated \n");

  // double vrai_temps;
  // long int temps;

  // int M = 10000000;
  // mpz_t B;
  // mpz_init(B);
  // mpz_t tmp;
  // mpz_init(tmp);
  // mpz_t tmp2;
  // mpz_init(tmp2);
  // mpz_cdiv_q_ui(B, sharedStruct.p, OMEGA);
  // mpz_mul_ui(B, B, OMEGA_p);
  // gettimeofday(&start, NULL);

  // for (int i = 0; i < M; i++) {
  //   mpz_mul(tmp, sharedStruct.g, root->childrens[0]->childrens[i %
  //   R]->value.M); mpz_mod(tmp, tmp, sharedStruct.p); mpz_fdiv_q(tmp, tmp, B);
  //   // printf("the real value should be : ");
  //   // mpz_out_str(stdout, 10, tmp);
  //   // printf("\n");
  // }
  // gettimeofday(&end, NULL);

  // temps = ((end.tv_sec * 1000000 + end.tv_usec) -
  //          (start.tv_sec * 1000000 + start.tv_usec));

  // printf("temps mult = %lu\n", temps);

  // unsigned long int k;
  // unsigned long int *M_v;

  // gettimeofday(&start, NULL);
  // for (int i = 0; i < M; i++) {
  //   M_v = root->childrens[0]->childrens[i % R]->v;
  //   k = tau__(sharedStruct.g, M_v);
  //   // printf("tau value : %ld\n", k);
  // }
  // gettimeofday(&end, NULL);
  // temps = ((end.tv_sec * 1000000 + end.tv_usec) -
  //          (start.tv_sec * 1000000 + start.tv_usec));

  // printf("temps tau  = %lu\n", temps);

  // return 0;

  // printf("here is the value of M1 : ");

  // mpz_out_str(stdout, 10, sharedStruct.listM[1].M);
  // printf("\n");

  // printf("here is the value of M1**5 : ");

  // mpz_out_str(stdout, 10,
  //             root->childrens[1]
  //                 ->childrens[0]
  //                 ->childrens[0]
  //                 ->childrens[0]
  //                 ->childrens[0]
  //                 ->value.M);
  // printf("\n");

  /* END JUNK */
  /* INIT collision*/
  mpz_init(sharedStruct.collision_1.n);
  mpz_init(sharedStruct.collision_2.n);
  mpz_init(sharedStruct.collision_1.m);
  mpz_init(sharedStruct.collision_2.m);

  /* creation de N_THREADS thread qui calcul des points distingué */
  pthread_t tid[N_THREADS];                         /* thread identifiers */
  pthread_mutex_init(&sharedStruct.lock_set, NULL); /* mutex lock for set */
  /* the main thread waits on this variable to shut down everything
  (has to be linked with a mutex) */
  pthread_mutex_init(&sharedStruct.lock_signal, NULL);
  pthread_cond_init(&sharedStruct.signalFinish, NULL);
  /*locking the threads while creating them*/
  pthread_mutex_lock(&sharedStruct.lock_signal);
  pthread_mutex_lock(&sharedStruct.lock_set);

  for (int i = 0; i < N_THREADS; i++) {
    if (pthread_create(&tid[i], NULL, f_dinstingue, &sharedStruct) != 0) {
      fprintf(stderr, "Unable to create thread number : %d\n", i);
      exit(EXIT_FAILURE);
    }
  }

  gettimeofday(&start, NULL);
  pthread_mutex_unlock(&sharedStruct.lock_set);
  pthread_cond_wait(&sharedStruct.signalFinish, &sharedStruct.lock_signal);
  for (int i = 0; i < N_THREADS; i++) {
    pthread_cancel(tid[i]);
  }

  printf("HEIGHT OF THE TREE : %d | NB OF ELEMENT ADDED : %d\n",
         sharedStruct.set->height, cpt);

  /* Extraction du resultat , on veut : (a_even-a)(b-b_even)^(-1) (mod q)  */
  mpz_t r;
  mpz_init(r);
  mpz_sub(r, sharedStruct.collision_1.n,
          sharedStruct.collision_2.n); /* r = (b - b_even) (mod q) */
  mpz_mod(r, r, sharedStruct.q);       /* r = (b - b_even) (mod q) */

  /* test si (b - b_even) est inversible (mod q) */
  if (mpz_cmp_si(r, 0) == 0) {
    printf("ECHEC \n");
    goto clear;
  }

  mpz_invert(r, r, sharedStruct.q); /* r= (b-b_even)^(-1) (mod q) */
  mpz_sub(sharedStruct.collision_2.m, sharedStruct.collision_2.m,
          sharedStruct.collision_1.m); /* a_even = a_even-a (mod q) */
  mpz_mul(r, r, sharedStruct.collision_2.m);
  mpz_mod(r, r, sharedStruct.q);

  printf("r = ");
  PRINT2(r)

  /* liberation de la mémoire */
clear:
  mpz_clear(sharedStruct.g);
  mpz_clear(sharedStruct.h);
  mpz_clear(sharedStruct.p);
  mpz_clear(sharedStruct.q);
  mpz_clear(r);
  mpz_clear(sharedStruct.collision_1.n);
  mpz_clear(sharedStruct.collision_1.m);
  mpz_clear(sharedStruct.collision_2.n);
  mpz_clear(sharedStruct.collision_2.m);
  deleteTree(sharedStruct.set);
  for (int i = 0; i < R; i++) {
    mpz_clear(sharedStruct.listM[i].m);
    mpz_clear(sharedStruct.listM[i].n);
    mpz_clear(sharedStruct.listM[i].M);
  }
  delete_tree(sharedStruct.root);

  gettimeofday(&end, NULL);
  long int temps = ((end.tv_sec * 1000000 + end.tv_usec) -
                    (start.tv_sec * 1000000 + start.tv_usec));
  printf("temps = %ld\n", temps);
  printf("here is the number of itteration : %ld", itter);

  fclose(in);
  return EXIT_SUCCESS;
}