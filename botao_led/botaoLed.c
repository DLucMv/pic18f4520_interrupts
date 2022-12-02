#include <p18f4520.h>

#define interruptor PORTBbits.RB1 //INTO
#define led PORTCbits.RC7

void configuracaoDoPic(){

	INTCON2bits.INTEDG0 = 1; // Inicia como borda de subida
	INTCON3bits.INT1IE = 1;	// Interrup��o
	INTCON3bits.INT1IP = 1;	// Prioridade
	INTCON3bits.INT1IF = 0;	// Flag
	INTCONbits.GIE = 1;		// Habilita o sistema de 
							// interrup��o de alta prioridade

// Configura��o da dire��o dos pinos de I/O
	
	TRISBbits.RB1 = 1; 		// Interruptor como entrada
	TRISCbits.RC7 = 0; 		// Lampada como saida
	ADCON1 = 0B00001111; 	// Configura todos os pinos
							// das portas como I/O 

//	TRISC = 0B01111111; 	// Configura LED como saida (RC7 = 0)
							// Interruptor como entrada (RC0 = 1)

}

void tratamentoIntAlta(void);

#pragma code vertorAlta = 0x0008 	// Pragma code para alocar
									// o c�digo no endere�o de 
									// mem�ria 0x0008
void intAltaPrioridade (void){
	_asm GOTO tratamentoIntAlta _endasm // Assembly
}

#pragma code						// 0x0008


#pragma interrupt tratamentoIntAlta 
void tratamentoIntAlta (void){
	if(INTCON3bits.INT1IF){
		led = ~led;
		INTCON3bits.INT1IF = 0;
	}
}

void main (void) {
	configuracaoDoPic();
	while(1){}

}