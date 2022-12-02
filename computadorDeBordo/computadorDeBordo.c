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
void mostrar_odometro(void);
void mostrar_tacometro(void);

char convert(int);


#define comando_dado LATCbits.LATC0
#define pulso LATCbits.LATC1
#define odometro PORTBbits.RB0
#define tacometro PORTBbits.RB2

int count = 0;
int km_un = 0;
int km_dez = 0;
int km_cen = 0;
int km_mil = 0;
int km_dez_mil = 0;
int km_cen_mil = 0;


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

void ch_especial(){

	envia_comando(0x40);
	envia_dado(0b11111);
	envia_comando(0x41);
	envia_dado(0b11111);
	envia_comando(0x42);
	envia_dado(0b11111);
	envia_comando(0x43);
	envia_dado(0b11111);
	envia_comando(0x44);
	envia_dado(0b11111);
	envia_comando(0x45);
	envia_dado(0b11111);
	envia_comando(0x46);
	envia_dado(0b11111);
	envia_comando(0x47);
	envia_dado(0b11111);


}

void mostrar_odometro(){

	envia_comando(0x80);
	envia_dado(convert(km_cen_mil));
	envia_dado(convert(km_dez_mil));
	envia_dado(convert(km_mil));
	envia_dado(convert(km_cen));
	envia_dado(convert(km_dez));
	envia_dado(convert(km_un));
	envia_dado(' ');
	envia_comando(0x87);
	envia_dado('K');
	envia_dado('M');

}

void mostrar_tacometro(){
	envia_comando(0x80);
	envia_dado('0');
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
	envia_comando(0xC0);
	envia_dado(0);
	envia_dado(0);
	envia_dado(0);
	envia_comando(0xCD);
	envia_dado('R');
	envia_dado('P');
	envia_dado('M');
}


void tratamentoIntAlta(void);

#pragma code vertorAlta = 0x0008 	// Pragma code para alocar
									// o código no endereço de 
									// memória 0x0008
void intAltaPrioridade (void){
	_asm GOTO tratamentoIntAlta _endasm // Assembly
}

#pragma code						// 0x0008


#pragma interrupt tratamentoIntAlta 
void tratamentoIntAlta (void){
	if(INTCONbits.INT0IF){
		count++;
		INTCONbits.INT0IF = 0;
		if(count >= 893){
			km_un++;
			if(km_un > 9){
				km_un = 0;
				km_dez++;
				if(km_dez > 9){
					km_dez = 0;
					km_cen++;
					if(km_cen > 9){
						km_cen = 0;
						km_mil++;
						if(km_mil > 9){
							km_mil = 0;
							km_dez_mil++;
							if(km_dez_mil > 9){
								km_dez_mil = 0;
								km_cen_mil++;
								if(km_cen_mil > 9){
									km_un = 0;
									km_dez = 0;
									km_cen = 0;
									km_mil = 0;
									km_dez_mil = 0;
									km_cen_mil = 0;
								}
							}
						}
					}
				}
			}
			count = 0;
		}
	}
}

char convert(int inteiro){
	char character = inteiro + '0';
	return character;
}

void configuracaoDoPic(){

	INTCON2bits.INTEDG0 = 1; // Inicia como borda de subida
	INTCONbits.INT0IE = 1;	// Interrupção
	//INTCONbits.INT0IP = 1;	// Prioridade
	INTCONbits.INT0IF = 0;	// Flag
	INTCONbits.GIE = 1;		// Habilita o sistema de 
							// interrupção de alta prioridade

// Configuração da direção dos pinos de I/O
	
	TRISBbits.RB0 = 1; 		// odometro como entrada
	TRISBbits.RB2 = 1; 		// tacometro como entrada
	ADCON1 = 0B00001111; 	// Configura todos os pinos
							// das portas como I/O 

//	TRISC = 0B01111111; 	// Configura LED como saida (RC7 = 0)
							// Interruptor como entrada (RC0 = 1)

	OSCCONbits.IRCF2 = 1;
	OSCCONbits.IRCF1 = 1;
	OSCCONbits.IRCF0 = 1;	//OSC = 8MHz (interno)

//BIT=1 -> PORTA DE ENTRADA, BIT=0 -> PORTA DE SAÍDA
	TRISB=0B00000101;
	TRISC=0B00000000;	 
	TRISD=0B00000000;
}


main ()

{	
	configuracaoDoPic();
	inicializar_display();
	ch_especial();
	delay_ms(10);
	while(1){
		mostrar_odometro();
	}
}