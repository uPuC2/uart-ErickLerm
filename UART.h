
#ifndef UART_H
#define UART_H

// Estructura que representa los registros del UART
// Definir los registros de UART como punteros a direcciones de memoria.
extern volatile uint8_t *UCSRA_regs[]; // Para UCSR0A, UCSR1A, UCSR2A, UCSR3A
extern volatile uint8_t *UCSRB_regs[]; // Para UCSR0B, UCSR1B, UCSR2B, UCSR3B
extern volatile uint8_t *UCSRC_regs[]; // Para UCSR0C, UCSR1C, UCSR2C, UCSR3C
extern volatile uint8_t *UBRRL_regs[]; // Para UBRR0L, UBRR1L, UBRR2L, UBRR3L
extern volatile uint8_t *UBRRH_regs[]; // Para UBRR0H, UBRR1H, UBRR2H, UBRR3H
extern volatile uint8_t *UDR_regs[];   // Para UDR0, UDR1, UDR2, UDR3

// Prototypes
// Initialization
UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop);

// Send
void UART_puts(uint8_t com, char *str);
void UART_putchar(uint8_t com, char data);

// Received
uint8_t UART_available(uint8_t com);
char UART_getchar(uint8_t com );
void UART_gets(uint8_t com, char *str);

// Escape sequences
UART_clrscr( uint8_t com );
UART_setColor(uint8_t com, uint8_t color);
UART_gotoxy(uint8_t com, uint8_t x, uint8_t y);

#define YELLOW  33 // Fixme 
#define GREEN   32 // Fixme 
#define BLUE    34 // Fixme 

// Utils
void itoa(uint16_t number, char* str, uint8_t base) ;
uint16_t atoi(char *str);

#endif
