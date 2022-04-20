#!/bin/bash

#parametre de la fonction : 1- nombre de jeu de test 2- nombre de fois que l'on va tester un parametre

> output.txt # clear the file
> input.txt

j=1
while [ $j -lt $1 ] ; do
    
    ./changeInput $j
    j=$[$j+1]
    i=0
    while [ $i -lt $2 ] ; do
        i=$[$i+1]
        echo Time to Rho $i times: >> output.txt
        echo -n >> output.txt
        { time ./TER2 ; } 2>> output.txt
        echo -n >> output.txt
        
        echo Time to rAddingWalk  $i times: >> output.txt
        echo -n >> output.txt
        { time ./rhoPollard_rAddingWalk ; } 2>> output.txt
        echo -n >> output.txt
        
        echo Time to Threads $i times: >> output.txt
        echo -n >> output.txt
        { time ./rhoPollard_Threads ; } 2>> output.txt
        echo -n >> output.txt
    done
done
exit