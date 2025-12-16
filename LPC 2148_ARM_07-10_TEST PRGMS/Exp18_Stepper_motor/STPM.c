// STEPPER MOTOR INTERFACING
//--------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: December - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------
//-------------------------------------------------------------------
// A stepper motor direction is controlled by shifting the voltage across 
// the coils. Port lines : P0.28 to P0.31
//-------------------------------------------------------------------

#include <LPC21xx.h>

void clock_wise(void) ;
void anti_clock_wise(void) ;
 
unsigned long int var1 , var2 ;
unsigned int i = 0 , j = 0 , k = 0 ;

int main(void)
{
	PINSEL1 = 0x00FFFFFF ;		//P0.28 to P0.31 GPIO
	IO0DIR |= 0xF0000000 ;		//P0.28 to P0.31 made as output
	
	while(1)
	{
		for( j = 0 ; j < 50 ; j++ )       // 20 times in Clock wise Rotation
			clock_wise() ;
		
		for( k = 0 ; k < 65000 ; k++ ) ;   // Delay to show  anti_clock Rotation 
		
		for( j=0 ; j < 50 ; j++ )       // 20 times in  Anti Clock wise Rotation
			anti_clock_wise() ;

		for( k = 0 ; k < 65000 ; k++ ) ;   // Delay to show clock Rotation 

	} 							

}// End of main 								

void clock_wise(void)
{
	var1 = 0x08000000;         		//For Clockwise
    for( i = 0 ; i <= 3 ; i++ )     // for A B C D Stepping
	{
		var1 <<= 1 ;        	//For Clockwise

		IO0CLR =0xF0000000 ;	   //clearing all 4 bits
		
		IO0SET = var1 ;			   // setting perticular bit

        for( k = 0 ; k < 3000 ; k++ ); 		//for step speed variation         
	}

}

void anti_clock_wise(void)
{
	var1 = 0x80000000 ;      				//For Anticlockwise

	IO0CLR =0xF0000000 ;					//clearing all 4 bits

	IO0SET = var1 ;							// setting perticular bit

    for( k = 0 ; k < 3000 ; k++ ) ; 		//for step speed variation 

    for( i = 0 ; i < 3 ; i++ )      		// for A B C D Stepping
   	{
		var1 >>= 1 ;     					//For Anticlockwise
		
		IO0CLR =0xF0000000 ;

		IO0SET = var1 ;
		
        for( k = 0 ; k < 3000 ; k++ ) ; 		//for step speed variation 
	}
}
