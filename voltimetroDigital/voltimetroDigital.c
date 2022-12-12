#include <p18f4520.h>

#define barramento LATD

#define RS LATCbits.LATC0
#define EN1 LATCbits.LATC1

int tensao;
int aux1=-2, aux2=-2, aux3=-2, aux4=-2;

void delay_ms(void) {
	
	int i;
	for(i = 0; i < 500; i++){} 

}

void delay_inicial(void) {
	
	int i;
	for(i = 0; i < 5000; i++){}
}

void envia_dado(unsigned char dado) {
	
	RS = 1;
	EN1 = 1;
	barramento = dado;
	EN1 = 0;
	delay_ms();
}

void envia_comando(unsigned char comando) {
	
	RS = 0;
	EN1 = 1;
	barramento = comando;
	EN1 = 0;
	delay_ms();
}

void inicializar_display(void){
		envia_comando(0b00111000);
		envia_comando(0b00001100);
		envia_comando(0b00000001);	
}


void configura_pic(void) {

	ADCON0bits.ADON=1;  // LIGA O CONVERSOR
	ADCON1bits.PCFG3=1;
	ADCON1bits.PCFG2=1;
	ADCON1bits.PCFG1=0;
	ADCON1bits.PCFG0=1; // USA AN0 e AN1 como analogicas <3:0>

	ADCON1bits.VCFG1=0; // VREF- (GND)
	ADCON1bits.VCFG0=0; // VREF+ (5V)

	ADCON2=0b10111100; 	// JUSTIFICA PELA DIREITA <7> 
						// 20TAD <5:3>
						// FOSC/4 <2:0>

	TRISC = 0b00000000; //ConfiguraçãoLCD
	TRISD = 0b00000000;	// RS e EN como SAIDA
	
	delay_inicial(); // Barramento SAIDA

	inicializar_display();

}

void mostrar_lcd(void) {

	envia_comando(0x80);
	envia_dado('C');
	envia_dado('a');
	envia_dado('n');
	envia_dado('a');
	envia_dado('l');
	envia_dado('1');
	envia_dado(':');
	envia_dado(' ');
	envia_dado('0');
	envia_dado(',');
	envia_dado('0');
	envia_dado('0');
	envia_dado('V');
	envia_comando(0xC0);
	envia_dado('C');
	envia_dado('a');
	envia_dado('n');
	envia_dado('a');
	envia_dado('l');
	envia_dado('2');
	envia_dado(':');
	envia_dado(' ');
	envia_dado('0');
	envia_dado(',');
	envia_dado('0');
	envia_dado('0');
	envia_dado('V');
}

void calculo(void) {

	unsigned float conversao = 0.0048828125;
	float valor = tensao * conversao;
	int organiza = valor * 100;

	aux1=organiza/100;
	organiza %= 100;
	aux2=organiza/10;
	organiza %= 10;
	aux3=organiza;	

	if(ADCON0bits.CHS0) { //Se Canal 1 (AN0)
		envia_comando(0x88);
	} 

	else{
		envia_comando(0xC8);
	}

	envia_dado(aux1+48);
	envia_dado(',');
	envia_dado(aux2+48);
	envia_dado(aux3+48);
}

void main() {

	configura_pic();
	mostrar_lcd();

	while(1){  

		// CANAL 1 Selecao de canal analogico<5:2>
		ADCON0bits.CHS3=0;
		ADCON0bits.CHS2=0;
		ADCON0bits.CHS1=0;
		ADCON0bits.CHS0=0; 	// SELECIONA AN0	
		ADCON0bits.GO=1; 	// INICIA UMA CONVERSÃO (Bit de estatus de conversão)

		while(ADCON0bits.GO==1){}; 	// ESPERA TERMINAR CONVERSAO
		tensao=(ADRESH*256)+ADRESL;
		calculo();

		// CANAL 2 Selecao de canal analogico<5:2>
		ADCON0bits.CHS3=0;
		ADCON0bits.CHS2=0;
		ADCON0bits.CHS1=0;
		ADCON0bits.CHS0=1; 	// SELECIONA AN1
		ADCON0bits.GO=1; 	// INICIA UMA CONVERSÃO (Bit de estatus de conversão

		while(ADCON0bits.GO==1){}; 	// ESPERA TERMINAR CONVERSAO
		tensao=(ADRESH*256)+ADRESL;
		calculo();
	}
}