// Pour compiler : gcc -g -Wall TER.c -lgmp

#include <gmp.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

void f_roh (mpz_t * Y,int *A,int *B, mpz_t g,mpz_t h, int n, mpz_t p){
        
        unsigned long int s;
        s = mpz_fdiv_ui(*Y,3);


        if(s == 1){
            *B = (*B+1)%n;
            mpz_mul(*Y,*Y,h);
            mpz_mod(*Y,*Y,p); //p nb prmeier qui def le groupe, dé en dur ? 
        }
        if(s == 0){
            *A=(2 * (*A)) % n;
            *B=(2 * (*B)) % n;
            mpz_mul(*Y,*Y,*Y);
            mpz_mod(*Y,*Y,p);
        }
        if(s == 2){
            *A=(*A+1) % n;
            mpz_mul(*Y,*Y,g);
            mpz_mod(*Y,*Y,p);
        }
}
int modInverse(int a, int m)
{
    int m0 = m;
    int y = 0, x = 1;
 
    if (m == 1)
        return 0;
 
    while (a > 1) {
        // q is quotient
        int q = a / m;
        int t = m;
 
        // m is remainder now, process same as
        // Euclid's algo
        m = a % m, a = t;
        t = y;
 
        // Update y and x
        y = x - q * y;
        x = t;
    }
 
    // Make x positive
    if (x < 0)
        x += m0;
 
    return x;
}

int main()
{
    // initialisation des entiers
   /* char gs[1024]; // generateur en entree en string
    char ns[1024]; // ordre en string
    char hs[1024]; // point de depart de l'iteration en string*/
    mpz_t g;
    mpz_t h;
    mpz_t p;
    int n = 191;
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
    mpz_t Y_EVEN, Y;              // variables iteratives
    int A, A_EVEN, B, B_EVEN, i,r = 0; // puissances

    mpz_init(Y);
    mpz_set_ui(Y, 1);
    mpz_init(Y_EVEN);
    mpz_set_ui(Y_EVEN, 1);          //initialisation des entiers a 0


    while (1)
    {
        i++;
        f_roh(&Y,&A,&B,g,h,n,p);
        f_roh(&Y_EVEN,&A_EVEN,&B_EVEN,g,h,n,p);       //application de la fonction 
        f_roh(&Y_EVEN,&A_EVEN,&B_EVEN,g,h,n,p);
        printf("%d,%d,%d,%d,%d,%d\n",i,A,B,Y_EVEN,A_EVEN,B_EVEN);
        if( mpz_cmp(Y,Y_EVEN) == 0){
            r = (B-B_EVEN)% n;
            if(r == 0){
                return -1;
            }
             int resultat = modInverse(r,n)*(A_EVEN-A) % n;
            printf("resultat = %d \n",resultat);
            return resultat;                                //inversion modulaire
        }




    }

    mpz_clear(g);
    mpz_clear(h);
    mpz_clear(p);
    mpz_clear(Y);
    mpz_clear(Y_EVEN);
    return 0;
}