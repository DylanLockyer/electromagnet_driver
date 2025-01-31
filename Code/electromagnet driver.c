#define F_CPU 8000000UL //Set clock frequency for delay function

#include <avr/io.h>
#include <util/delay.h>

void analog_begin(){
	ADMUX = (1<<MUX1)|(1<<MUX0); //Set pin A3
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1); //Set prescaler of 64

}	
void digital_begin(){
	DDRB|= (1<<PB0)|(1<<PB1); //Initialise driver output
	PORTB=(1<<PB1); //Set initial values for driver
}
unsigned long analog_read(){
	ADCSRA |= (1<<ADSC);//Start ADC conversion
	while (ADCSRA & (1<<ADSC)); //Wait untill read

	return ADC; //set analog_read to ADC
}

//Delay function cause avr-gcc is dumb and a delay can't be set to a variable
void ms_delay(unsigned long milliseconds){
	while (milliseconds--){ //Subtract from milliseconds untill values is true (greater than 0)
		_delay_ms(1); //Wait one millisecond for every value of variable milliseconds
	}
}

//Create map function for changing pot sensor range
long map(unsigned long a, unsigned long in_min, unsigned long in_max, unsigned long out_min, unsigned long out_max){
	return (long)(a-in_min)*(long)(out_max-out_min)/(long)(in_max-in_min)+out_min;
}

int main(void){
	analog_begin(); //Initialise analog read
	digital_begin(); //Initialise digital output		
		
	//Pot read variables
	unsigned long pot = 0;
	unsigned long read = 0;
	unsigned long wait = 500;	
	while(1){
	
		read=analog_read(); //Read potentiometer and set to variable read
		pot = map(read,0,1024,500,50); //Map value of pot read to delay interval

		//Increase speed in increments for smoother operation
		if ((long)(pot-wait)>20){
			wait += map(wait,50,500,1,20); //Scale value added to delay to make rate of change consistent
		} else if ((long)(pot-wait)<-20){
			wait -= map(wait,50,500,1,20);;//Scale value added to delay to make rate of change consistent
		} else{
			wait = pot;
		}
		
		//if pot is less or equal to 20, turn off outputs and do nothing
		if (pot > 490){
			_delay_ms(10); //Wait
			PORTB = 0; //Set both outputs to zero
		} else{

			if (PORTB & (1<<PB0)){ //If PB0 is on
				PORTB = (1<<PB1); //PB1 on, PB0 off
			} else{ //If PB1 is on
				PORTB = (1<<PB0); //PB1 off, PB0 on
			}

			ms_delay(wait); //Wait for predetermined time
		}		
	}
}
