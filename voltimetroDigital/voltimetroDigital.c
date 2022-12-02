#include <p18f4520.h>


#pragma config	OSC = INTIO67
#pragma config	WDT = OFF
#pragma config	MCLRE = OFF
#pragma config	PBADEN = OFF
#pragma config	BORV = 3
#pragma config 	BOREN = OFF
	

void delay_ms (int);
void delay_s (int);
void delay_us (int);


// Funções do display
void envia_comando (char);
void envia_dado (char);
void inicializar_display(void);
void mostrar_tacometro(void);
void ch_especial(void);
void mostrar_volt1(void);
void mostrar_volt2(void);


#define comando_dado LATCbits.LATC0
#define pulso LATCbits.LATC1
#define volt2 PORTAbits.RA0
#define volt1 PORTAbits.RA1

unsigned int adc_AN0 = 0;


void delay_s (int s)
{
	unsigned long int i;
	for ( ; s > 0; s--)
	{
		for (i=0;i<83333;i++) {};
	}
}


void delay_ms(int ms)

{
	unsigned int i;
	for ( ;ms > 0; ms--)
	{
		for (i=0;i<130;i++) {}		//tempo de loop = 1ms
	}
}


void delay_us(int us)	//funciona a partir de 10us
{
	unsigned char i;
	for ( ;us > 0; us--){
		for (i=0;i<28;i++) {};
	};
	i=0;
}


void envia_comando (char comando)
{
		comando_dado = 0;
		pulso = 1;
		LATD = comando;
		pulso = 0;
		delay_ms(10);
}


void envia_dado (char dado)
{
		comando_dado = 1;
		pulso = 1;
		LATD = dado;
		pulso = 0;
		delay_ms(10);
}


void inicializar_display(){
		envia_comando(0b00111000);
		envia_comando(0b00001100);
		envia_comando(0b00000001);	
}


void mostrar_volt1(){

	envia_comando(0x80);
	envia_dado('V');
	envia_dado('1');
	envia_dado(':');
	envia_dado(' ');
	envia_dado('0');
	envia_dado('0');
	envia_dado(',');
	envia_dado('0');
	envia_dado('0');
	envia_dado(' ');
	envia_dado('V');
	envia_comando(0xC0);
	envia_dado('V');
	envia_dado('2');
	envia_dado(':');
	envia_dado(' ');
	envia_dado('0');
	envia_dado('0');
	envia_dado(',');
	envia_dado('0');
	envia_dado('0');
	envia_dado(' ');
	envia_dado('m');
	envia_dado('V');


}

void mostrar_volt2(){
	envia_comando(0x80);
	envia_dado('V');
	envia_dado(' ');
	envia_dado('1');
	envia_dado(' ');
	envia_dado('2');
	envia_dado(' ');
	envia_dado('3');
	envia_dado(' ');
	envia_dado('4');
	envia_dado(' ');
	envia_dado('5');
	envia_dado(' ');
	envia_dado('6');
	envia_dado(' ');
	envia_comando(0xCD);
	envia_dado('R');
	envia_dado('P');
	envia_dado('M');
}


void configuracaoDoPic(){

	INTCON2bits.INTEDG0 = 1; // Inicia como borda de subida
	INTCONbits.INT0IE = 1;	// Interrupção
	//INTCONbits.INT0IP = 1;	// Prioridade
	INTCONbits.INT0IF = 0;	// Flag
	INTCONbits.GIE = 1;		// Habilita o sistema de 
							// interrupção de alta prioridade

	ADCON0 = 0B00000011;	//Modulo conversor habilitado para A0 e A1
	ADCON1 = 0B00001101;    //AN0 e AN1 habilitados
	ADCON2 = 0B10100001;

// Configuração da direção dos pinos de I/O
	
	TRISAbits.RA0 = 1; 		// v2 como entrada
	TRISAbits.RA1 = 1; 		// v1 como entrada
	ADCON1 = 0B00000000; 	// Configura os pinos 0 a 3
							// como portas analogicas 

//	TRISC = 0B01111111; 	// Configura LED como saida (RC7 = 0)
							// Interruptor como entrada (RC0 = 1)

	OSCCONbits.IRCF2 = 1;
	OSCCONbits.IRCF1 = 1;
	OSCCONbits.IRCF0 = 1;	//OSC = 8MHz (interno)

//BIT=1 -> PORTA DE ENTRADA, BIT=0 -> PORTA DE SAÍDA
	TRISA=0B11111111;
//	TRISB=0B00000000;
	TRISC=0B00000000;	 
	TRISD=0B00000000;
}


main ()

{	
	configuracaoDoPic();
	inicializar_display();
	delay_ms(10);
	while(1){
		mostrar_volt1();
	}
}