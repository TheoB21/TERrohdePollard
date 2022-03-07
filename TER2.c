// Pour compiler : gcc -g -Wall TER.c -lgmp

#include <gmp.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

bool is_generated = false;

void f_roh(mpz_t *Y, mpz_t *A, mpz_t *B, mpz_t *g, mpz_t *h, mpz_t *n, mpz_t *p)
{

    unsigned long int s;
    s = mpz_fdiv_ui(*Y, 3);

    if (s == 1)
    {
        //*B = (*B + 1) % n;
        mpz_add_ui(*B, *B, 1);
        mpz_mod(*B, *B, *n); //exposant tracing

        mpz_mul(*Y, *Y, *h); //on fait itéré Y

        mpz_mod(*Y, *Y, *p); //p nb prmeier qui def le groupe, codé en dur ?
    }
    if (s == 0)
    {
        mpz_mul_ui(*A, *A, 2);
        mpz_mod(*A, *A, *n);
        //*A = (2 * (*A)) % n;

        mpz_mul_ui(*B, *B, 2);
        mpz_mod(*B, *B, *n);
        //*B = (2 * (*B)) % n;

        mpz_mul(*Y, *Y, *Y);
        mpz_mod(*Y, *Y, *p);
    }
    if (s == 2)
    {
        //*A = (*A + 1) % n;
        mpz_add_ui(*A, *A, 1);
        mpz_mod(*A, *A, *n);

        mpz_mul(*Y, *Y, *g);
        mpz_mod(*Y, *Y, *p);
    }
}

void f_r_adding_walk(mpz_t *Y, mpz_t *A, mpz_t *B, mpz_t *g, mpz_t *h, mpz_t *n, mpz_t *p)
{

    unsigned long int s;
    long int list_exp[20 * sizeof(long int)];
    mpz_t list_M[10 * sizeof(mpz_t)];
    int r = 0;
    srand(time(NULL));
    s = mpz_fdiv_ui(*Y, 9); // on génère une partition de l'ensemble en 9 cas

    mpz_t g_init; //génerateur G
    mpz_t h_init;
    mpz_init(g_init);
    mpz_set_ui(g_init, 0);
    mpz_init(h_init);
    mpz_set_ui(h_init, 0);

    //A et B c'est les exposant à suivre
    //Y c'est la varaible a itérer
    //les M_s sont généré aléatoirement au début et on garde les mêmes après

    if (!is_generated)
    {

        for (int i = 0; i < 18; i++)
        {

            r = rand() % 2 ^ 31; //on génère exposant des M_s, les 2i c'est ième exposant de g, les 2i+1 c'est ième exposant de h
            list_exp[i] = r;
        }
        for (int i = 0; i < 10; i++)
        {

            mpz_powm_ui(g_init, *g, list_exp[2 * i], *p); //on calcul les Ms
            mpz_powm_ui(h_init, *h, list_exp[(2 * i) + 1], *p);
            mpz_mul(list_M[i], g_init, h_init);
            mpz_mod(list_M[i], list_M[i], *p);
        }
        //RQ la taille des M_s des assez petite, normalement c'est des nombre aléatoire dans Z/qZ, mais en pratique si on prend des nombres bien plus petit
        //ca devrait marcher aussi et ca réduit calcul !
        //Ex dans rho initial il utiliser les couples (1,0), (0,1)
        is_generated = true;
    }

    //B c'est exposant de h
    //A c'est exposant de g

    mpz_add_ui(*B, *B, list_exp[2 * s]); //exposant tracing //On additione l'exposant de M_s en h dans l'exposant Y en h
    mpz_mod(*B, *B, *n);                 //optimiser ?? Vriament bresoin de faire mod n ? ca depasse quasi jamais n (ordre du groupe) je pense !!
    mpz_add_ui(*A, *A, list_exp[2 * s + 1]);
    mpz_mod(*A, *A, *n);

    mpz_mul(*Y, *Y, list_M[s]); //on fait itéré Y

    mpz_mod(*Y, *Y, *p); //p nb prmeier qui def le groupe, codé en dur ?
}

int main()
{
    // initialisation des entiers
    /*char gs[1024]; // generateur en entree en string
    char ns[1024]; // ordre en string
    char hs[1024]; // point de depart de l'iteration en string*/
    mpz_t g; //génerateur G
    mpz_t h;
    mpz_t p;
    mpz_t n; //ordre groupe G

    //int flag;

    /*  printf("Entrez votre le generateur g :\n");
    scanf("%1023s", gs);
    printf("Entrez votre l'ordre n :\n"); //on passe les entrees
    scanf("%d", ns);
    printf("Entrez votre l'element h :\n");
    scanf("%1023s", hs);*/

    mpz_init(g);
    mpz_set_ui(g, 3);

    mpz_init(h); // on initialise les entiers a 0
    //mpz_set_ui(h, 228);
    mpz_set_str(h, "835242181", 10);

    mpz_init(p);
    mpz_set_str(p, "17179869209", 10);

    mpz_init(n);
    mpz_set_str(n, "17179869208", 10);

    mpz_out_str(stdout, 10, h);
    printf("\n");
    mpz_out_str(stdout, 10, p);
    printf("\n");
    mpz_out_str(stdout, 10, n);
    printf("\n");

    /*flag = mpz_set_str(g, gs, 10);
    assert(flag == 0);
    flag = mpz_set_str(h, hs, 10); // on transforme les string en int
    assert(flag == 0);
    flag = mpz_set_str(n, ns, 10);
    assert(flag == 0);*/

    /*printf("g = ");
    mpz_out_str(stdout,10,g); //to print
    printf ("\n");*/

    //fin de l'initialisation des entiers

    //debut de la declaration des variables
    mpz_t Y_EVEN, Y; // variables iteratives
    mpz_t A;
    mpz_t A_EVEN;
    mpz_t B;
    mpz_t B_EVEN;
    mpz_t r;

    mpz_init(Y); //la variable qu'on itère
    mpz_set_ui(Y, 1);
    mpz_init(Y_EVEN);      //pour les itérations 2i
    mpz_set_ui(Y_EVEN, 1); //initialisation des entiers a 0
    mpz_init(A);           //exposant
    mpz_set_ui(A, 0);
    mpz_init(A_EVEN);
    mpz_set_ui(A_EVEN, 0);
    mpz_init(B); //exposant
    mpz_set_ui(B, 0);
    mpz_init(B_EVEN);
    mpz_set_ui(B_EVEN, 0);
    mpz_init(r);
    mpz_set_ui(r, 0);

    while (1)
    {
        /*printf ("B = ");
        mpz_out_str(stdout,10,B);
        printf("\n");
        printf ("B_EVEN = ");
        mpz_out_str(stdout,10,B_EVEN);
        printf("\n");
        printf ("A = ");
        mpz_out_str(stdout,10,A);
        printf("\n");
        printf ("A_EVEN = ");
        mpz_out_str(stdout,10,A_EVEN);
        printf("\n");printf ("Y = ");
        mpz_out_str(stdout,10,Y);
        printf("\n");
        printf ("Y_EVEN = ");
        mpz_out_str(stdout,10,Y_EVEN);
        printf("\n");

        printf("FIN \n");*/

        f_roh(&Y, &A, &B, &g, &h, &n, &p);
        f_roh(&Y_EVEN, &A_EVEN, &B_EVEN, &g, &h, &n, &p); //application de la fonction
        f_roh(&Y_EVEN, &A_EVEN, &B_EVEN, &g, &h, &n, &p);

        if (mpz_cmp(Y, Y_EVEN) == 0)
        {

            mpz_sub(r, B, B_EVEN);
            //r = (B - B_EVEN);

            mpz_mod(r, r, n);
            //r = r % n;

            if (mpz_cmp_si(r, 0) == 0)
            {
                return -1;
            }

            //int resultat = modInverse(r, n) * (A_EVEN - A); //inversion modulaire
            mpz_sub(A_EVEN, A_EVEN, A);
            mpz_invert(r, r, n);
            mpz_mul(r, r, A_EVEN);
            mpz_mod(r, r, n);

            printf("r = ");
            mpz_out_str(stdout, 10, r);
            printf("\n");
            return 1;
        }
    }

    mpz_clear(g);
    mpz_clear(h);
    mpz_clear(p); //liberation de l'espace
    mpz_clear(Y);
    mpz_clear(Y_EVEN);
    mpz_clear(A_EVEN);
    mpz_clear(A);
    mpz_clear(B);
    mpz_clear(B_EVEN);
    mpz_clear(n);
    return 0;
}