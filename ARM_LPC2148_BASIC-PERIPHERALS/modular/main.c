		#include<stdio.h>
		#include<lpc21xx.h>
		#include"add.h"
extern unsigned int add_2 (unsigned int,unsigned int);
unsigned int a,b,c;
int main()
{
 
 a=10;
 b=20;
 c=add_2(a,b);
 while(1);
 return(0);
}
