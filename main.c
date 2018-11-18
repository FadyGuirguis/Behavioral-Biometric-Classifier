#include <REG52.h>
#include <stdio.h>
#include <math.h>

unsigned char character;
unsigned int overflow = 0;
unsigned long flight = 0;
unsigned long flightAverageA[9] = {0,0,0,0,0,0,0,0,0};
unsigned long flightAverageB[9] = {0,0,0,0,0,0,0,0,0};

unsigned long dA;
unsigned long dB;

unsigned int trials;

unsigned char characters[10] = {'.', 't', 'i', 'e', '5', 'R', 'o', 'n', 'a', 'l'};
unsigned char currentCharacter = 0;

sbit phase = P0^0; // 0 for training and 1 for testing
sbit user = P0^1; // 0 for user a and 1 for user b
sbit led = P0^2; 

void timer0_isr() interrupt 1 {
	overflow++;
	TH0 = 0X00;
	TL0 = 0X00;
}
void delay(int value){
	int i;
	int j;
	for(i = 0; i < value; i++){
		for(j = 0; j < 100; j++);
	}
}

void check(int feature){
	if (phase == 0){
		if(user == 0){
			flightAverageA[feature] += flight;
		} else {
			flightAverageB[feature] += flight;
		}
	} else {
		dA += labs(flight - flightAverageA[feature]);
		dB += labs(flight - flightAverageB[feature]);
	}
}

void readCharacter(){
	character = _getkey();
	
	if (character != characters[currentCharacter])
		return;
	
	currentCharacter = (currentCharacter + 1) % 10;
	
	flight = (overflow * 65536) + ((TH0 << 8) | TL0);
	
	switch (character){
		
		case '.': break;
		case 't': check(0); break;
		case 'i': check(1); break;
		case 'e': check(2); break;
		case '5': check(3); break;
		case 'R': check(4); break;
		case 'o': check(5); break;
		case 'n': check(6); break;
		case 'a': check(7); break;
		case 'l': check(8); trials ++; break;
	}
	
	if (phase == 0 && trials == 5){
		int i;
		for (i = 0; i < 9; i++) {
			if (user == 0)
				flightAverageA[i] = flightAverageA[i]/5;
			else
				flightAverageB[i] = flightAverageB[i]/5;
		}
		trials = 0;
	}
	
	if (phase == 1 && character == 'l'){
	
		if(dA < dB){
			led = 1;
			delay(10000);
			led = 0;
		} else {
			led = 1;
			delay(10000);
			led = 0;
			delay(10000);
			led = 1;
			delay(10000);
			led = 0;
		}
		
		dA = 0;
		dB = 0;
	}
	overflow = 0;
	TH0 = 0;
	TL0 = 0;	
}
 void main (void){
	 
	 SCON = 0X50;
	 PCON = 0X80;
	 TMOD = 0X21;
	 TH1 = 0XF3;
	 TR1 = 1;
	 TH0 = 0X00;
	 TL0 = 0X00;
	 TR0 = 1;
	 ET0 = 1;
	 EA = 1;
	 TI = 1;
	 
	 led = 0;
	 phase = 0;
	 
	 while(1){
		 readCharacter();
	 }
	 
 }
