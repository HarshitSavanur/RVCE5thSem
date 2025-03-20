#include <stdio.h>
#include<stdlib.h>
void function()
{
printf("Entered function !!");
}


void main(int argv,char **argc)
{

int a =1;
int b=atoi(argc[1]);
int c=a+b;
if(c>0)
{
printf("The sum is : %d",c);
exit(0);
}
function();
}

