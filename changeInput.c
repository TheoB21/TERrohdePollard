#include <gmp.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define TAILLE_MAX 1000

int main(int argc, char *argv[])
{
    char temp_g[TAILLE_MAX];
    char temp_h[TAILLE_MAX];
    char temp_p[TAILLE_MAX];
    char temp_n[TAILLE_MAX];

    FILE *input = fopen("input.txt", "w+");
    if (input == NULL)
    {
        printf("input is NULL\n");
        exit(EXIT_FAILURE);
    }
    FILE *allinput = fopen("allInput.txt", "r+");
    if (allinput == NULL)
    {
        printf("allinput is NULL\n");
        exit(EXIT_FAILURE);
    }
    char c;
    int cpt = 0;
    int i = 0;
    while (cpt < (atoi(argv[1]) - 1) * 4)
    {
        c = fgetc(allinput);

        fseek(allinput, i, SEEK_SET);

        if (c == '\n')
        {
            cpt++;
        }
        if (c == EOF)
        {
            printf("fin de fichier\n");
            return 1;
        }
        i++;
    }

    fgets(temp_g, TAILLE_MAX, allinput);
    fgets(temp_h, TAILLE_MAX, allinput);
    fgets(temp_p, TAILLE_MAX, allinput);
    fgets(temp_n, TAILLE_MAX, allinput);

    fprintf(input, "%s%s%s%s", temp_g, temp_h, temp_p, temp_n);

    fclose(input);
    fclose(allinput);
    return 0;
}