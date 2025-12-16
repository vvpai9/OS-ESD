#include<stdio.h>
#include<lpc21xx.h>



extern int f( int ,  int);
 int a=5, b=5, c;
int main()
{
	c=f(a,b);
	return(0);
}