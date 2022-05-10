// Pour compiler : gcc -g -Wall TER.c -lgmp

#include <assert.h>
#include <gmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 20

#define INIT(name, value) \
  mpz_t name;             \
  mpz_init(name);         \
  mpz_set_str(name, value, 10);

#define PRINT(var)              \
  mpz_out_str(stdout, 10, var); \
  printf("\n");

typedef struct
{
  mpz_t m; /*exposant de  g*/
  mpz_t n; /*exposant de  h*/
} couple;

void f(mpz_t *y, mpz_t *a, mpz_t *b, mpz_t *g, mpz_t *h, mpz_t *q, mpz_t *p,
       couple *listExposant, mpz_t *listM)
{
  /* on génère une partition de l'ensemble en N cas */
  unsigned long int s = mpz_fdiv_ui(*y, N);
  mpz_add(*b, *b, listExposant[s].n);
  mpz_mod(*b, *b, *q);
  mpz_add(*a, *a, listExposant[s].m);
  mpz_mod(*a, *a, *q);
  mpz_mul(*y, *y, listM[s]);
  mpz_mod(*y, *y, *p);
}

int main()
{

  char temp_g[1000];
  char temp_h[1000];
  char temp_p[1000];
  char temp_q[1000];

  FILE *input = fopen("input.txt", "r+");

  if (input == NULL)
  {
    printf("file is NULL\n");
    exit(EXIT_FAILURE);
  }

  fscanf(input, "%s %s %s %s", &temp_g, &temp_h, &temp_p, &temp_q);

  /* génerateur g de G */
  /*INIT(g,
       "39446785437698040340461597167197039958900119517854788257122347157448791"
       "324269")
  /* h tel que l'on cherche log_g(h) */
  /*INIT(h,
       "47009043480611367617830675310337741057970676588266669887302467688270735"
       "539171")*/
  /* l'ordre du grand groupe */
  // INIT(p,
  /*    "68526471339746042131712464873602124202388714266226208884158671408493995"
      "491329")
 /* ordre du sous-groupe G */
  // INIT(q, "166578624252013")
  mpz_t g; // génerateur G
  mpz_t h;
  mpz_t p;
  mpz_t q; // ordre groupe G
  mpz_init(g);
  // mpz_set_ui(g, tab[0]);
  mpz_set_str(g, temp_g, 10);

  mpz_init(h); // on initialise les entiers a 0
  // mpz_set_ui(h, 228);
  mpz_set_str(h, temp_h, 10);
  // mpz_set_ui(h, tab[1]);
  mpz_init(p);
  mpz_set_str(p, temp_p, 10);
  // mpz_set_ui(p, tab[2]);
  mpz_init(q);
  mpz_set_str(q, temp_q, 10);
  /* PRINT(h)
   PRINT(p)
   PRINT(q)
 */
  // debut de la declaration des variables
  INIT(y, "1")
  INIT(y_even, "1")
  INIT(a, "0") /* exposant de g */
  INIT(a_even, "0")
  INIT(b, "0") /* exposant de h */
  INIT(b_even, "0")
  INIT(r, "0") /* r for result */

  /* generation des ms et ns de façon aléatoire */

  gmp_randstate_t state;
  gmp_randinit_mt(state);
  gmp_randseed_ui(state, time(NULL));
  couple listExposant[N]; /* from 0 to N-1: c'est un N-adding walk */
  mpz_t listM[N];
  INIT(g_init, "0")
  INIT(h_init, "0")

  for (int i = 0; i < N; i++)
  {
    mpz_init(listExposant[i].m);
    mpz_urandomm(listExposant[i].m, state, q); /* defined mod q */
    mpz_init(listExposant[i].n);
    mpz_urandomm(listExposant[i].n, state, q);
    mpz_powm(g_init, g, listExposant[i].m, p);
    mpz_powm(h_init, h, listExposant[i].n, p);
    mpz_init(listM[i]);
    mpz_mul(listM[i], g_init, h_init);
    mpz_mod(listM[i],listM[i],p);
  }
  mpz_clear(g_init);
  mpz_clear(h_init);
  gmp_randclear(state);

  /* application de la fonction tant qu'il n'y a pas de collision */
  do
  {
    f(&y, &a, &b, &g, &h, &q, &p, listExposant, listM);
    f(&y_even, &a_even, &b_even, &g, &h, &q, &p, listExposant, listM);
    f(&y_even, &a_even, &b_even, &g, &h, &q, &p, listExposant, listM);
  } while (mpz_cmp(y, y_even) != 0);

  /* Extraction du resultat , on veut : (a_even-a)(b-b_even)^(-1) (mod q)  */
  mpz_sub(r, b, b_even); /* r = (b - b_even) (mod q) */
  mpz_mod(r, r, q);      /* r = (b - b_even) (mod q) */

  /* test si (b - b_even) est inversible (mod q) */
  if (mpz_cmp_si(r, 0) == 0)
  {
    printf("ECHEC \n");
    goto end;
  }

  mpz_invert(r, r, q);        /* r= (b-b_even)^(-1) (mod q) */
  mpz_sub(a_even, a_even, a); /* a_even = a_even-a (mod q) */
  mpz_mul(r, r, a_even);
  mpz_mod(r, r, q);

  printf("r = ");
  PRINT(r)

  /* liberation de la mémoire */
end:
  for (int i = 0; i < N; i++)
  {
    mpz_clear(listExposant[i].m);
    mpz_clear(listExposant[i].n);
    mpz_clear(listM[i]);
  }
  mpz_clear(g);
  mpz_clear(h);
  mpz_clear(p);
  mpz_clear(y);
  mpz_clear(y_even);
  mpz_clear(a_even);
  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(b_even);
  mpz_clear(q);
  mpz_clear(r);
  fclose(input);

  return EXIT_SUCCESS;
}