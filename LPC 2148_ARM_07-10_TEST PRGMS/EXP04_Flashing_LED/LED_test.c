// LED's INTERFACE
//----------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: December - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------

#include <LPC21xx.h>

unsigned int delay=0 , i = 0 , temp = 0x00 ;

int main ()
{
	PINSEL1 = 0x00000000 ;

	IO0DIR = 0x0000F0000;	// Configure P0.16 to P0.19  as output
	IO0SET = 0x000F0000 ; 	// all LED's turned OFF.

	while(1)
	{
		temp =0x00008000 ; 	
		for( i = 0 ; i < 4 ; i++ )
		{
			temp <<= 1 ;			 	// LED's ON
			IO0CLR = temp ; 	
					
			for(delay = 0 ; delay < 100000 ; delay++ );
		}
		for(delay = 0 ; delay < 100000 ; delay++ );
		for(delay = 0 ; delay < 100000 ; delay++ );

		temp =0x00100000 ; 	
		for( i = 0 ; i < 4 ; i++ )
		{								 // LED's OFF
			temp >>= 1 ;
			IO0SET = temp ; 	
			for(delay = 0 ; delay < 100000 ; delay++ );
		}

		for(delay = 0 ; delay < 100000 ; delay++ );	  
       	for(delay = 0 ; delay < 100000 ; delay++ );
	}  
}	

