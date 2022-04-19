#!/bin/bash

> output.txt # clear the file 
i=0
while [ $i -lt $1 ] ; do 
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
exit