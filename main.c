/*
 * Pre_lab_06.c
 *
 * Created: 4/20/2024 6:03:55 PM
 * Author : samue
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void setup(void);
void initUART9600(void);  // significa que el UART comenzara a 9600
void writeUART(char Caracter); //  funcion para mandar caracter por caracter
void WriteTextUART(char * Texto); // funcion para enviar un texto completo de un solo 


// declaracion de variables volatiles
volatile uint8_t bufferTX;
volatile uint8_t bufferRX;
uint8_t valorADC = 0x00;
uint8_t centenas;
uint8_t decenas;
uint8_t	unidades;
uint8_t centenas1;
uint8_t decenas1;
uint8_t unidades1;
uint8_t bandera;



void setup(){
	// declaro todo el puerto B como salida 
	DDRB |= (1<<DDB0) | (1<<DDB1) | (1<<DDB2) | (1<<DDB3) | (1<<DDB4) | (1<<DDB5);
	// Configuración de los pines del puerto C como salidas
	DDRC |= (1 << PC0) | (1 << PC1);
	initUART9600();
	initADC();
	sei();
}	


void UART_lab() {
	writeUART(10);
	writeUART(13);
	WriteTextUART("ESTE ES EL LABORATORIO Ingrese un caracter: ");
	writeUART(10);
	writeUART(13);
}

void UART_PostLab(){
	writeUART(10);
	writeUART(13);
	WriteTextUART("Escoger que desea ver, presionar 1 o 2:");
	writeUART(10);
	writeUART(13);
	WriteTextUART("1. Leer el potenciometro");
	writeUART(10);
	writeUART(13);
	WriteTextUART("2. Enviar ASCII");
	writeUART(10);
	writeUART(13);
}

void RespuestaDeMenu(){
	//if ()
	
	
}

int main(void)
{
    setup();
	//UART_lab();
	UART_PostLab();
	char opcion;
    while (1) 
    {
		ADCSRA |= (1<<ADSC);
		
    }
}

void initUART9600(void){
	// paso 1: establecer pines como entrada y salida 
	//RX COMO ENTRADA Y TX COMO SALIDA 
	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD1);
	
	// PASO 2: configurar UCSR0A
	UCSR0A = 0;
	
	// PASO 3: configurar UCSR0B  DONDE SE HABILITA LA ISR DE RECEPCION 
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);

	//PASO 4: CONFIGURAR UCSR0C
	UCSR0C = 0;
	//CONFIGURAR ASINCORONO Y SIN PARIDAD, 1 BIT DE STOP, 8 CARACTERES
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
	//PASO 5: SE CALCULA EL VALOR DE BAUD Y VELOCIDAD: 9600
	UBRR0 = 103;
		
}


void writeUART(char Caracter){
	// si no es 1 quedate esperando POR LO TANTO HASTA QUE SEA 1 VA CONTINUAR 
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = Caracter;
	
}


void WriteTextUART(char * Texto){
	uint8_t i;
	for (i=0; Texto[i]!= '\0'; i++){
		while (!(UCSR0A & (1<<UDRE0)) );
		UDR0 = Texto[i];
	}
}



void LEDS(){
	// funcion para cargar a los leds el valor del contador
	PORTB = (PORTB & 0xC0) | (bufferTX & 0x3F);
	PORTC = (PORTC & 0xFC) | ((bufferTX >> 6) & 0x03);
}
ISR(USART_RX_vect){
	
	bufferTX = UDR0;
	while (!(UCSR0A & (1<<UDRE0)));
	//UDR0 = bufferTX;
	//LEDS();
	if (bandera == 1){
		LEDS();
		bandera = 0;
		UART_PostLab();
		writeUART(10);
		writeUART(13);
	}
	else {
		if ((bufferTX == 49)){
			PORTB = valorADC;
			PORTC = (valorADC >> 6);
			centenas = valorADC/100;
			centenas1 = centenas + 48;
			decenas = (valorADC-(centenas*100))/10;
			decenas1 = decenas + 48;
			unidades = (valorADC-(centenas*100)-(decenas*10));
			unidades1 = unidades + 48;
			writeUART(centenas1);
			writeUART(decenas1);
			writeUART(unidades1);
			writeUART(10);
			writeUART(13);
			UART_PostLab();
			
		}
	else if (bufferTX == 50){
		WriteTextUART("Ingrese un caracter: ");
		writeUART(10);
		writeUART(13);
		bandera = 1;
	}
	else {
		UART_PostLab();
		writeUART(10);
		writeUART(13);
	}
	}
	
	//UART_lab();
	//UART_PostLab();
	
	
}

ISR(ADC_vect) {
	valorADC = ADCH;
	ADCSRA |= (1<<ADIF); // Resetear la bandera de ADC
}

void initADC() {
	ADMUX = 0; // INICIO EN 0
	// REFERENCIA AVCC = 5V
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	// SELECCIONO EL ADC[6] = 0110
	ADMUX |= (1 << MUX2) | (1 << MUX1);
	
	// JUSTIFICACION A LA IZQUIERDA
	ADMUX |= (1 << ADLAR);
	
	ADCSRA = 0;
	
	
	// HABILITAR LA INTERRUPCION DEL ADC
	ADCSRA |= (1 << ADIE);
	
	// HABILITAMOS EL PRESCALER 128  FADC = 125 KHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	// HABILITAMOS EL ADC
	ADCSRA |= (1 << ADEN);
}