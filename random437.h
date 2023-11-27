#ifndef random437_h
#define random437_h

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

double U_Random();
int poissonRandom();

int poissonRandom(int meanArrival)
{
int k=0;
long double p=1.0;
long double l=exp(-meanArrival);
double u=U_Random();
double F=l;
while(u>=F)
{   
    k++;
    l*=(double)meanArrival/k;
    F+=l;
}
  return k;
}

double U_Random()
{
return (double)rand()/RAND_MAX;
}


#endif /* random437_h */