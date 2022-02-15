// Pour compiler : gcc -g -Wall TER.c -lgmp

#include <gmp.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

void f_roh(mpz_t *Y, mpz_t *A, mpz_t *B, mpz_t *g, mpz_t *h, mpz_t *n, mpz_t *p)
{

    unsigned long int s;
    s = mpz_fdiv_ui(*Y, 3);

    if (s == 1)
    {
        //*B = (*B + 1) % n;
        mpz_add_ui(*B, *B, 1);
        mpz_mod(*B, *B, *n);

        mpz_mul(*Y, *Y, *h);

        mpz_mod(*Y, *Y, *p); //p nb prmeier qui def le groupe, dé en dur ?
    }
    if (s == 0)
    {
        mpz_add(*A, *A, *A);
        mpz_mod(*A, *A, *n);
        //*A = (2 * (*A)) % n;

        mpz_add(*B, *B, *B);
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


int main()
{
    // initialisation des entiers
    /*char gs[1024]; // generateur en entree en string
    char ns[1024]; // ordre en string
    char hs[1024]; // point de depart de l'iteration en string*/
    mpz_t g;
    mpz_t h;
    mpz_t p;
    //int flag;

    /*  printf("Entrez votre le generateur g :\n");
    scanf("%1023s", gs);
    printf("Entrez votre l'ordre n :\n"); //on passe les entrees
    scanf("%d", ns);
    printf("Entrez votre l'element h :\n");
    scanf("%1023s", hs);*/

    mpz_init(g);
    mpz_set_ui(g, 2);
    mpz_init(h); // on initialise les entiers a 0
    mpz_set_ui(h, 228);
    mpz_init(p);
    mpz_set_ui(p, 383);

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
    mpz_t n;
    mpz_t A;
    mpz_t A_EVEN;
    mpz_t B;
    mpz_t B_EVEN;
    mpz_t r;


    mpz_init(Y);
    mpz_set_ui(Y, 1);
    mpz_init(Y_EVEN);
    mpz_set_ui(Y_EVEN, 1); //initialisation des entiers a 0
    mpz_init(A);
    mpz_set_ui(A, 0);
    mpz_init(A_EVEN);
    mpz_set_ui(A_EVEN, 0);
    mpz_init(B);
    mpz_set_ui(B, 0);
    mpz_init(B_EVEN);
    mpz_set_ui(B_EVEN, 0);
    mpz_init(r);
    mpz_set_ui(r, 0);
    mpz_init(n);
    mpz_set_ui(n, 191);

    while (1)
    {
        printf ("B = ");
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

        printf("FIN \n");




        f_roh(&Y, &A, &B, &g, &h, &n, &p);
        f_roh(&Y_EVEN, &A_EVEN, &B_EVEN, &g, &h, &n, &p); //application de la fonction
        f_roh(&Y_EVEN, &A_EVEN, &B_EVEN, &g, &h, &n, &p);

        if (mpz_cmp(Y, Y_EVEN) == 0)
        {
            printf ("r = ");
            mpz_out_str(stdout,10,r);
            printf("\n");
            mpz_sub(r, B, B_EVEN);
            //r = (B - B_EVEN);
            printf ("B = ");
            mpz_out_str(stdout,10,B);
            printf("\n");
            printf ("B_EVEN = ");
            mpz_out_str(stdout,10,B_EVEN);
            printf("\n");


            mpz_mod(r, r, n);
            //r = r % n;

            if (mpz_cmp_si(r, 0) == 0)
            {
                return -1;
            }

            //int resultat = modInverse(r, n) * (A_EVEN - A); //inversion modulaire
            mpz_sub(A_EVEN, A_EVEN, A);
            printf ("r = ");
            mpz_out_str(stdout,10,r);
            printf("\n");
            mpz_invert(r, r, n);
            printf ("r = ");
            mpz_out_str(stdout,10,r);
            printf("\n");
            mpz_mul(r,r, A_EVEN);
            printf ("r = ");
            mpz_out_str(stdout,10,r);
            printf("\n");
            mpz_mod(r,r,n);
            
            printf ("r = ");
            mpz_out_str(stdout,10,r);
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