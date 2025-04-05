#include <avr/io.h>
#include "UART.h"

// Prototypes
// Initialization

// Direcciones de los registros de UART
volatile uint8_t *UCSRA_regs[] = { &UCSR0A, &UCSR1A, &UCSR2A, &UCSR3A };
volatile uint8_t *UCSRB_regs[] = { &UCSR0B, &UCSR1B, &UCSR2B, &UCSR3B };
volatile uint8_t *UCSRC_regs[] = { &UCSR0C, &UCSR1C, &UCSR2C, &UCSR3C };
volatile uint8_t *UBRRL_regs[] = { &UBRR0L, &UBRR1L, &UBRR2L, &UBRR3L };
volatile uint8_t *UBRRH_regs[] = { &UBRR0H, &UBRR1H, &UBRR2H, &UBRR3H };
volatile uint8_t *UDR_regs[]   = { &UDR0,   &UDR1,   &UDR2,   &UDR3   };
	
volatile int cnt=0;

UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop)
{

	uint16_t ubrr_value = (16000000 / (8 * baudrate)) - 1;  // Cálculo del valor de UBRR

    // Configurar los registros UBRR0L y UBRR0H
    *UBRRH_regs[com] = (uint8_t)(ubrr_value >> 8);  // Guardar el byte alto de UBRR
    *UBRRL_regs[com] = (uint8_t)ubrr_value;         // Guardar el byte bajo de UBRR
    *UCSRB_regs[com] = (1 << TXEN0) | (1 << RXEN0);  // Habilita TX y RX
    *UCSRC_regs[com] = (size - 5) << UCSZ00; // Configura el tamaño de los datos (5 a 9 bits)
    *UCSRA_regs[com] |= (1 << U2X0);  // Habilitar el bit de doble velocidad en UCSR0A
        
    // Configura la paridad
    if (parity == 1)
        {*UCSRC_regs[com] |= (1 << UPM01);}                // Paridad par even
    else if (parity == 2) 
        {*UCSRC_regs[com] |= (1 << UPM00) | (1 << UPM01);} // Paridad impar odd
        
        
    // Configura los bits de parada
    if (stop == 2) 
    {
        *UCSRC_regs[com] |= (1 << USBS0); // 2 bits de parada
    }
    
}

// Send
void UART_puts(uint8_t com, char *str)
{

    while(*str)
    {
        while(!(*UCSRA_regs[com] & (1<< UDRE0)));
        *UDR_regs[com]=*str++;
		cnt++;
    }
/*
	while (*str) 
	{
        UART_putchar(com, *str);
        str++;
    
    }
    */
}

void UART_putchar(uint8_t com, char data)
{                                       
   
    while (!(*UCSRA_regs[com] & (1 << UDRE0)));  // Compara con el bit UDRE0 en UCSR0
	                                             // (Espera hasta que el registro de datos esté vacío)
    *UDR_regs[com]= data;         
  
}

// Received
uint8_t UART_available(uint8_t com)
{
    return (!(*UCSRA_regs[com] & (1 << RXC0))) ; // Hay dato disponible
}
char UART_getchar(uint8_t com )
{
	char c;
	// Espera hasta que haya un dato disponible
    while (UART_available(com)) { }
    // Retorna el dato recibido
	c= *UDR_regs[com]; 
  
        
        return c;

}

void UART_gets(uint8_t com, char *str)
{
	char c;
    uint8_t i = 0;
	
    // Lee hasta que se reciba un salto de línea o el buffer esté lleno
	
		
		while(1)
		 {
			c = UART_getchar(com);
		
            if(i<128 || c=='\b')
            {
	            if(!(i==0 && c=='\b'))
					UART_putchar(com,c);
            }

            
            if (c == '\b' || c == 127) 
            {

                if (i > 0) 
                {
                    UART_putchar(com, ' ');
                    UART_putchar(com, '\b');
                    str[i--] = '\0';  
                }

            }
            else
            {
                if (c == '\r' )
                {
                    break; // Finaliza
                }

				if(i<128)
					str[i++] = c;
            }
         }
    str[i] = '\0'; // Termina la cadena	
}

// Escape sequences
UART_clrscr( uint8_t com )
{
    UART_puts(com, "\x1b[2J");  // Limpiar la pantalla
    UART_puts(com, "\x1b[H");   // Mover el cursor a la posición inicial (0, 0)
}
UART_setColor(uint8_t com, uint8_t color)
{
    UART_puts(0, "\x1B[");
    UART_putchar(0, '0' + (color / 10));  // Imprime primer dígito
    UART_putchar(0, '0' + (color % 10));  // Imprime segundo dígito
    UART_puts(0, "m");
}

UART_gotoxy(uint8_t com, uint8_t x, uint8_t y)
{
    // Secuencia de escape para mover el cursor a la posición (x, y)
    UART_puts(com, "\x1B[");

    // Imprime la coordenada y
    if (y >= 10) {  // Si y es un número de dos dígitos
        UART_putchar(com, '0' + (y / 10));  // Imprime el primer dígito de y
    }
    UART_putchar(com, '0' + (y % 10));  // Imprime el segundo dígito de y

    UART_putchar(com, ';');  // Separador de las coordenadas

    // Imprime la coordenada x
    if (x >= 10) {  // Si x es un número de dos dígitos
        UART_putchar(com, '0' + (x / 10));  // Imprime el primer dígito de x
    }
    UART_putchar(com, '0' + (x % 10));  // Imprime el segundo dígito de x

    UART_putchar(com, 'H');  // Finaliza la secuencia de movimiento de cursor
}

// Utils
void itoa(uint16_t number, char* str, uint8_t base)
{
    char digits[] = "0123456789ABCDEF";
    int i = 0;
    char temp[17];

    if (number == 0) 
        temp[i++] = '0';
    else 
    {
        while (number != 0) 
        {
            temp[i++] = digits[number % base];  // Obtiene el dígito
            number /= base;                    // Desplaza el número a la derecha
        }
    }
    
    int tarray=i;

     //Invertir la cadena para que los dígitos estén en el orden correcto
    for (int j = 0; j < tarray; j++) 
    {
        str[j]=temp[i-j-1];
    }
	str[tarray]='\0';
    
}


uint16_t atoi(char *str)
{
    uint16_t result = 0;
    while (*str) 
    {
        if (*str >= '0' && *str <= '9') {
            result = result * 10 + (*str - '0');
        }
        str++;
    }
   
    return result;
}
