#include <p18f4520.h>
#include <math.h>

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
void mostrar_painel(void);

char convert(int);

#define comando_dado LATCbits.LATC0
#define pulso LATCbits.LATC1
#define odometro PORTBbits.RB2
#define tacometro PORTBbits.RB1


int count = 0;
int count2 = 1000;
int count3 = 0;
int km_un = 0;
int km_dez = 0;
int km_cen = 0;
int km_mil = 0;
int km_dez_mil = 0;
int km_cen_mil = 0;
int rot_mot = 0;
int rpm = 0;
int vel = 0;
int vel_un = 0;
int vel_dez = 0;
int vel_cen = 0;
int vamola = 0;
char character[3];


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


void envia_comando (char comando){
		comando_dado = 0;
		pulso = 1;
		LATD = comando;
		pulso = 0;
		delay_ms(10);
}

void envia_dado (char dado){
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


void mostrar_painel(){
	character[0] = (vel/100)+ '0';
	character[1] = (vel/10) % 10 + '0';
	character[2] = vel%10 + '0';

	character[3] = (rpm/1000)+ '0';
	character[4] = (rpm/100) % 10 + '0';
	character[5] = (rpm/100) % 10 + '0';
	character[6] = rpm%10 + '0';
	envia_comando(0x80);
	envia_dado(character[0]);
	envia_dado(character[1]);
	envia_dado(character[2]);
	envia_dado('k');
	envia_dado('m');
	envia_dado('/');
	envia_dado('h');
	envia_comando(0x88);
	envia_dado(convert(km_cen_mil));
	envia_dado(convert(km_dez_mil));
	envia_dado(convert(km_mil));
	envia_dado(convert(km_cen));
	envia_dado(convert(km_dez));
	envia_dado(convert(km_un));
	envia_dado('K');
	envia_dado('M');
	envia_comando(0xC0);
	envia_dado(character[3]);
	envia_dado('.');
	envia_dado(character[4]);
	envia_dado('(');
	envia_dado('x');
	envia_dado('1');
	envia_dado('0');
	envia_dado('0');
	envia_dado('0');
	envia_dado(')');
	envia_dado('R');
	envia_dado('P');
	envia_dado('M');

}

/*********************************************************/


char convert(int inteiro){
	char character = inteiro + '0';
	return character;
}

/**********************************************************/

void tratamentoIntAlta (void);

//vetor de interrupção de alta prioridade
#pragma code high_vector = 0x0008

void intAltaPrioridade (void){
	_asm GOTO tratamentoIntAlta _endasm // Assembly
}

#pragma code 

//Rotina de tratamento de interrupção (ISR)
#pragma interrupt tratamentoIntAlta 
void tratamentoIntAlta (void){
	if(INTCON3bits.INT2IF){
		count++;
		count3++;
		INTCON3bits.INT2IF = 0;
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

	if(INTCON3bits.INT1IF){
		INTCON3bits.INT1IF = 0;
		rot_mot++;
	}

	if(INTCONbits.TMR0IF){
		INTCONbits.TMR0IF = 0;
		count2--;
		if(count2 == 0){
			rpm = ceil(rot_mot * 60);
			vel = 1.12 * count3 * 3.6;
			rot_mot = 0;
			count2 = 1000;
			count3 = 0;
		}
	}

}

void configuracaoDoPic(){

	INTCON2bits.INTEDG0 = 1; // Inicia como borda de subida
	INTCON3bits.INT2IE = 1;	// Interrupção
	INTCON3bits.INT2IP = 1;	// Prioridade
	INTCON3bits.INT2IF = 0;	// Flag
	INTCON3bits.INT1IE = 1;	// Interrupção
	INTCON3bits.INT1IP = 1;	// Prioridade
	INTCON3bits.INT1IF = 0;	// Flag
	INTCONbits.GIE = 1;		// Habilita o sistema de 
							// interrupção de alta prioridade

	INTCONbits.TMR0IE = 1;	//liga chave individual de interrupção externa 0
	INTCON2bits.TMR0IP = 1;	//interrupção externa 0 ocorrerá na borda de descida



//configura TMR0 para operar como temporizador e estourar a cada 1ms
	T0CON = 0b11000010;				//Timer0 configurado como temporizador<5>
									//prescaler ativado<3>
									//fator de prescaler de 1:8<2:0>
									//liga TMR0
	TMR0L = 5;						//inicializa TMR0



// Configuração da direção dos pinos de I/O
	
	TRISBbits.RB1 = 1; 		// Tacometro como entrada
	TRISBbits.RB2 = 1; 		// Odometro como entrada
	ADCON1 = 0B00001111; 	// Configura todos os pinos
							// das portas como I/O 

//	TRISC = 0B01111111; 	// Configura LED como saida (RC7 = 0)
							// Interruptor como entrada (RC0 = 1)

	OSCCONbits.IRCF2 = 1;
	OSCCONbits.IRCF1 = 1;
	OSCCONbits.IRCF0 = 1;	//OSC = 8MHz (interno)

//BIT=1 -> PORTA DE ENTRADA, BIT=0 -> PORTA DE SAÍDA
	TRISB=0B00000110;
	TRISC=0B00000000;	 
	TRISD=0B00000000;
}


main ()

{	
	configuracaoDoPic();
	inicializar_display();
	delay_ms(10);
	while(1){
		mostrar_painel();
	}
}