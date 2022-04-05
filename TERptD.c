
#include <gmp.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

bool is_generated = false;
#define INIT_STR(x, value) \
    mpz_t x;               \
    mpz_init(x);           \
    mpz_set_str(x, value, 10);

void f_r_adding_walk(mpz_t *Y, mpz_t *A, mpz_t *B, mpz_t *g, mpz_t *h, mpz_t *n, mpz_t *p, long int *l_e, mpz_t *l_m)
{

    unsigned long int s;

    int r = 0;
    s = mpz_fdiv_ui(*Y, 9); // on génère une partition de l'ensemble en 9 cas
    srand(time(NULL));
    mpz_t g_init; //génerateur G
    mpz_t h_init;
    mpz_init(g_init);
    mpz_set_ui(g_init, 0);
    mpz_init(h_init);
    mpz_set_ui(h_init, 0);
    long int power = pow(2, 31);
    //A et B c'est les exposant à suivre
    //Y c'est la varaible a itérer
    //les M_s sont généré aléatoirement au début et on garde les mêmes après

    if (!is_generated)
    {
        for (int i = 0; i < 18; i++)
        {

            r = rand() % power; //on génère exposant des M_s, les 2i c'est ième exposant de g, les 2i+1 c'est ième exposant de h
            l_e[i] = r;
        }

        for (int k = 0; k < 9; k++)
        {

            mpz_powm_ui(g_init, *g, l_e[2 * k], *p); //on calcul les Ms

            mpz_powm_ui(h_init, *h, l_e[(2 * k) + 1], *p);

            mpz_mul(l_m[k], g_init, h_init);
            mpz_mod(l_m[k], l_m[k], *p);
        }
        //RQ la taille des M_s des assez petite, normalement c'est des nombre aléatoire dans Z/qZ, mais en pratique si on prend des nombres bien plus petit
        //ca devrait marcher aussi et ca réduit calcul !
        //Ex dans rho initial il utiliser les couples (1,0), (0,1)
        is_generated = true;
    }

    //B c'est exposant de h
    //A c'est exposant de g

    mpz_add_ui(*B, *B, l_e[2 * s + 1]); //exposant tracing //On additione l'exposant de M_s en h dans l'exposant Y en h
    mpz_mod(*B, *B, *n);                //optimiser ?? Vriament bresoin de faire mod n ? ca depasse quasi jamais n (ordre du groupe) je pense !!
    mpz_add_ui(*A, *A, l_e[2 * s]);
    mpz_mod(*A, *A, *n);

    mpz_mul(*Y, *Y, l_m[s]); //on fait itéré Y

    mpz_mod(*Y, *Y, *p); //p nb prmeier qui def le groupe, codé en dur ?
}

typedef struct
{

    int table[100000]; // tableau remplie par les thread

    bool finded; // mis a vrai si on trouve une colision

    mpz_t q; // l'ordre du petit groupe

    mpz_t g; // l'ordre generateur du petit groupe

    mpz_t p; // le modulo du grand groupe

    mpz_t h;

    long int * list_exp;  //liste ds exposants       

    mpz_t * list_M;    //liste des M (coefficients multiplicatifs)

} param_thread;

void *iteration(param_thread *p_t)
{
    INIT_STR(seed, "0")
    INIT_STR(x, "0")                        //iterateur
    INIT_STR(bound, "1152921504606846975"); //2**60 -1
    INIT_STR(A, "0")                        //exposants sur g
    INIT_STR(B, "0")                        //exposant sur h
    bool distingue = false;                 // si on trouve un point distingue
    gmp_randstate_t state;
    gmp_randinit_default(state);

    while (!p_t->finded)
    {
        mpz_urandomm(seed, state, p_t->q); // permet de tirer un nombre aleatoire entre 0 et q-1
        mpz_powm(x, x, seed, p_t−>p);     // on met x a la puissance seed mod p
        mpz_set(A, seed);
        while (!distingue)
        {
            if (mpz_cmp(bound, x) >= 0)
            {
                distingue = true;
            }
            f_r_adding_walk(&x, &A, &B, p_t->g, p_t->h, p_t->q, p_t->p, list_exp,list_M);
        }

        //CHERCHE COLLISION DANS LA TABLE SI OUI STOP TOUT THREAD (p_t->finded==true) SINON STOCK X DANS TABLE  et retrire nouvelle griaane ie repasse boucle
    }
}

int main(int argc, char *argv[])
{

    pthread_t tid1, tid2;

    if (pthread_create(&tid1, NULL, iteration, NULL) != 0)
    {

        fprintf(stderr, "Bug création thread\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&tid2, NULL, iteration, NULL) != 0)
    {

        fprintf(stderr, "Bug création thread\n");
        exit(EXIT_FAILURE);
    }
}