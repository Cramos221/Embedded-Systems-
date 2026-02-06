#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// --- PINOUT ---
#define DHT_PIN    PB0
#define PIN_EMERG  PB2 
#define PIN_LUZ    PD5
#define PIN_FAN    PD6
#define PIN_BOMBA  PD7

// --- PARAMETROS ---
#define TEMP_MAXIMA    300  // 30.0C
#define UMIDADE_MINIMA 400  // 40.0%
#define TEMPO_REGA_MAX 5    
#define DURACAO_DIA    10   
#define DURACAO_NOITE  10   

// --- VARIAVEIS ---
volatile uint8_t  segundos_sistema = 0;
volatile uint8_t  contador_rega = 0;
volatile uint8_t  modo_emergencia = 0; 

uint16_t umidade = 0;
uint16_t temperatura = 0;

// --- DRIVER DHT22 ---
void dht_start() {
    DDRB |= (1 << DHT_PIN); PORTB &= ~(1 << DHT_PIN);
    _delay_ms(18); PORTB |= (1 << DHT_PIN); _delay_us(40);
    DDRB &= ~(1 << DHT_PIN);
}
uint8_t dht_read_byte() {
    uint8_t res = 0;
    for (int i=0; i<8; i++) {
        while(!(PINB & (1 << DHT_PIN))); _delay_us(30);
        if (PINB & (1 << DHT_PIN)) res |= (1 << (7-i));
        while(PINB & (1 << DHT_PIN));
    }
    return res;
}
uint8_t ler_dht() {
    uint8_t d[5];
    dht_start();
    if (PINB & (1 << DHT_PIN)) return 0; _delay_us(80);
    if (!(PINB & (1 << DHT_PIN))) return 0; _delay_us(80);
    for (int i=0; i<5; i++) d[i] = dht_read_byte();
    if ((uint8_t)(d[0]+d[1]+d[2]+d[3]) != d[4]) return 0;
    umidade = (d[0] << 8) | d[1];
    temperatura = (d[2] << 8) | d[3];
    return 1;
}



// --- INTERRUPECOES ---

// 1. BOTaO DE EMERGENCIA (INT0 - PD2)
ISR(INT0_vect) {
    modo_emergencia = !modo_emergencia; // Alterna o estado 
    if (modo_emergencia) {
        // --- ENTROU EM EMERGENCIA ---
        // 1. Desliga todas as cargas (PD5, PD6, PD7)
        PORTD &= ~((1 << PIN_LUZ) | (1 << PIN_FAN) | (1 << PIN_BOMBA));
        
        // 2. LIGA o LED de Emerg�ncia (PB2)
        PORTB |= (1 << PIN_EMERG);
        
    } else {
        // --- SAIU DA EMERGENCIA ---
        // 1. Apaga o LED de Emergencia
        PORTB &= ~(1 << PIN_EMERG);
        
        // As cargas voltarao a funcionar sozinhas no loop principal
    }
   
    _delay_ms(200);
}

// 2. RELOGIO (TIMER1)
ISR(TIMER1_COMPA_vect) {
    if (modo_emergencia) return; // Se estiver em emergencia, o tempo para.

    segundos_sistema++;

    // Logica da Bomba (Seguranca)
    if (PORTD & (1 << PIN_BOMBA)) {
        contador_rega++;
        if (contador_rega >= TEMPO_REGA_MAX) {
            PORTD &= ~(1 << PIN_BOMBA); 
        }
    } else {
        contador_rega = 0;
    }

    // Logica da Luz (Ciclo)
    if (segundos_sistema < DURACAO_DIA) {
        PORTD |= (1 << PIN_LUZ);
    } else {
        PORTD &= ~(1 << PIN_LUZ);
    }

    if (segundos_sistema >= (DURACAO_DIA + DURACAO_NOITE)) {
        segundos_sistema = 0;
    }
}

int main(void) {
    // --- CONFIG GPIO ---
    DDRD |= (1 << PIN_LUZ) | (1 << PIN_FAN) | (1 << PIN_BOMBA); // SaIdas Cargas
    DDRB |= (1 << PIN_EMERG); // Saida LED Emergencia (PB2)
    
    // Inicializa tudo desligado
    PORTD &= ~((1 << PIN_LUZ) | (1 << PIN_FAN) | (1 << PIN_BOMBA)); 
    PORTB &= ~(1 << PIN_EMERG); 

    // Botao INT0 (PD2)
    DDRD &= ~(1 << PD2); 
    PORTD |= (1 << PD2); 

    // --- CONFIG TIMER1 ---
    TCCR1B |= (1 << WGM12); // CTC
    OCR1A = 15625;          // 1 segundo
    TIMSK1 |= (1 << OCIE1A);
    TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024

    // --- CONFIG INT0 ---
    EICRA |= (1 << ISC01); // Borda de descida
    EIMSK |= (1 << INT0);

    sei(); 

    while(1) {
        // Se NaO estiver em emergencia, roda a automacaoo
        if (!modo_emergencia) {
            _delay_ms(2000); 
            
            if (ler_dht()) {
                // Fan
                if (temperatura > TEMP_MAXIMA) PORTD |= (1 << PIN_FAN);
                else PORTD &= ~(1 << PIN_FAN);

                // Bomba
                if (umidade < UMIDADE_MINIMA && contador_rega == 0) PORTD |= (1 << PIN_BOMBA);
                if (umidade > (UMIDADE_MINIMA + 50)) PORTD &= ~(1 << PIN_BOMBA);
            }
        } 
        else {
				PORTD &= ~(1 << PIN_BOMBA);
				PORTD &= ~(1 << PIN_FAN);
				PORTD &= ~(1 << PIN_LUZ);
          
        }
    }
}
