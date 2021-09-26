/* Tankanzeige fuer VW Kaefer und aehnliche Tankanzeigen
Tankgeber wird von einem ATtiny25/45/85 gemessen und generiert hierfuer einen
Strom fuer das Anzeige-Instrument. 
Die Widerstaende fuer Tank-leer, Tank-reserve und Tank-voll muessen gemessen werden.
Ebenso muss die Stromaufnahme fuer den jeweiligen Zeigerausschlag fuer
Tank leer und Tank voll gemessen werden.
 */

#include "main.h"
#include "pid.h"
#include "FloatingAverage.h"

// globale Variablen, werden in der Mainloop und/oder in verschiedenen 
// Interrupts (ISR()) genutzt

// Messkanal ADC
volatile uint8_t channel = 1;

// Feedback
volatile float actualCurrentFeedback = 0.0;

// Filter Current Feedback
volatile tFloatAvgFilter filterCurrentFeedback; 

// Messwertvariabel fuer den Tankgeber.
volatile float tankGeber = TANKSENSOREMPTY;

// Filter Tankgeber
volatile tFloatAvgFilter filterTankGeber;
   
int main(void)
{
	
    // Filter Tankgeber initialisieren
    InitFloatAvg((tFloatAvgFilter*)&filterTankGeber, 
    			 (tFloatAvgType)(TANKSENSOREMPTY*100.0) );
    // Filter currentFeedback initialisieren
    InitFloatAvg((tFloatAvgFilter*)&filterCurrentFeedback, 
    			  0);
    
	// Mikrocontroller einstellen
    setUpAvr();
    
    // Regler fuer die Anzeige deklarieren
    pid_controller_t tankController;
    
    // Regler initialisieren
    pid_init(KP,					// P-Faktor
    		 KI,					// I-Faktor
    		 KD,					// D-Faktor
    		 &tankController);		// Controller
    for(;;){
    	// Mikrocontroller schlafen legen
    	// wird vom ADC oder Watchdog aufgeweckt
        sleep_mode();
    	// Interrupts deaktivieren damit die aktuellen Messwerte nicht verfaelscht werden
    	// durch eine neue Messung (ADC ist Interrupt-gesteuert)
    	uint8_t sreg = SREG;
    	cli();
    	if(channel == 3){
			// Neuer Messwert liegt vor, Tankgeberwiderstand errechnen anhand des 
			// gleitenden Mittelwerts, Ergebnis in Ohm.
			tankGeber = M_SENSOR 
						* GetOutputValue((tFloatAvgFilter*)&filterTankGeber)/100.0 
						+ B_SENSOR;
    	}
    	// Tankgeber unter Reserve?
    	if(tankGeber > TANKSENSORLOW){
    		// optionale LED einschalten.
    		PORTB |= (1 << PB4);
    	} else if(tankGeber < (TANKSENSORLOW - 2.0)){
    		// tankGeber war groeßer als TANKSENSORLOW und muss nun um 2 Ohm
    		// kleiner sein (entspricht einem volleren Tank). Hysterese um ein 
    		// Flackern der LED zu vermeiden falls man an der Grenze ist.
    		PORTB &= ~(1 << PB4);
    	}
    	// Stromquelle einstellen fuer die Tankanzeige, Vorgabe in mV
    	// da Stromwerte in mA angegeben werden. If-Verschachtelung
    	// damit das Anzeigeinstrument innerhalb der zulaessigen Parameter betrieben wird
    	
    	if(tankGeber >= TANKSENSOREMPTY){
    		// Tank leer oder Kabel zum Tankgeber nicht angeschlossen.
    		OCR0B = (uint8_t)pid_compute((SHUNT * (M_SOURCE * TANKSENSOREMPTY + B_SOURCE)),
    							 		 GetOutputValue((tFloatAvgFilter*)&filterCurrentFeedback)/1000.0,
    							 		 &tankController);
    	}else if(tankGeber <= TANKSENSORFULL){
    		// Tank voll oder Kurzschluss.
    		OCR0B = (uint8_t)pid_compute((SHUNT * (M_SOURCE * TANKSENSORFULL + B_SOURCE)),
    							 		 GetOutputValue((tFloatAvgFilter*)&filterCurrentFeedback)/1000.0,
    							 		 &tankController);
    	}else {
    		// Tankgeber arbeitet im zulaessigen Bereich.
    		OCR0B = (uint8_t)pid_compute((SHUNT * (M_SOURCE * tankGeber + B_SOURCE)),
    							 		  GetOutputValue((tFloatAvgFilter*)&filterCurrentFeedback)/1000.0,
    							 		  &tankController);
    	}
    	// Interrupts wieder aktivieren fuer den naechsten Messzyklus
    	// durch zurueckschreiben von sreg
    	SREG = sreg;
        
        // Mikrocontroller aufgewacht durch ADC oder Watchdog
        // Watchdog Interruptmode wieder einschalten falls er aus ist damit es nicht zum
        // Reset kommt, dieser wird bei jedem Watchdog-Interrupt deaktiviert.
        // Sollte das Programm mal haengen bleiben wird der Watchdog den Mikrocontroller
        // reseten und somit neu starten. Dazu sollte es eigentlich nie kommen.
        if ( !(WDTCR & (1 << WDIE)) ){
        	WDTCR |= (1 << WDIE);
        }
    }
    return 0;   /* wird nie erreicht */
}

void setUpAvr(void){
	// watchdog reset 
	asm("wdr");
	// und ausschalten
	WDTCR = (1 << WDCE)|(1 << WDE);
	WDTCR &= ~(1 << WDE);
	
	// PLL einschalten für 16 MHz Mode
	// CKSEL wird auf 0001 eingestellt via fuses (Clocksource: PLL)
	PLLCSR |= (1 << PLLE);
	
	// Einstellen des ADC:
	// ADC-Prescaler auf 16 einstellen
	// Mikrocontroller laeuft auf 1 MHz, damit laeuft der ADC mit 62.5 kHz, Abtastrate
	// ergibt sich so zu ca. 2.5 kHz da abwechselnd zwei Kanaele gemessen werden.
	ADCSRA |= (1 << ADPS2);
	// und Referenzspannungquelle (hier interne 2.56V) waehlen, 
	// zugleich wird ADC-Messkanal 1 ausgewaehlt.
	ADMUX |= (1 << REFS2)|(1 << REFS1)|(1 << REFS0)|(1 << MUX0);
	// ADC starten
    ADCSRA |= (1 << ADEN)|(1 << ADSC);
    // und auf Abschluss der Konvertierung warten. 
    // Dummy-Read wegen Wahl/Aenderung der Referenzspannungsquelle!
    while (ADCSRA & (1 << ADSC)) {
        
    }
    // ADC starten und Interrupt einschalten
    ADCSRA |= (1 << ADIE)|(1 << ADSC);
    
    // PWM einstellen:
    // Fast-PWM Mode und OC0B wird gewaehlt.
    TCCR0A |= (1 << WGM00)|(1 << WGM01)|(1 << COM0B1);
    TCCR0B |= (1 << CS00);
    // PWM Ausgangspin einstellen auf Ausgang fuer OC0B.
    DDRB |= (1 << PB1);
    PORTB &= ~(1 << PB1);
    
    // LED/Schaltkontakt fuer Tankreserve.
    // Anschluss standardmaeßig nicht bestueckt,
    // Funktionalitaet aber vorhanden.
    // Der Kontakt schaltet die Versorgung des Konstanters durch
    // und kann mit bis zu 0.5A maximal belastet werden.
    DDRB |=(1 << PB4);
    PORTB &= ~(1 << PB4);
    
    // nicht benoetigte Module abschalten, spart Strom wenngleich das hier
    // verschwindet gering sein wird.
    // Deaktivieren von Timer1 und USI-Modul
    PRR |= (1 << PRTIM1)|(1 << PRUSI);
    // Deaktivieren des Analog Comparators
    ACSR |= (1 << ACD);
    
    // Schlafmodus des Mikrocontrollers einstellen.
    // Da die Timer fuer die PWM benoetigt werden ist nur IDLE moeglich,
    // schon bei SLEEP_MODE_ADC werden die Clocks fuer die PWM deaktiviert.
    // Folge waere: PWM arbeitet nicht mehr, Tankanzeige bekommt den falschen
    // Strom eingepraegt und somit ist die Anzeige ansich falsch.
    set_sleep_mode(SLEEP_MODE_IDLE);
    
    // Watchdog einstellen auf 1s,
    WDTCR |= (1 << WDP2)|(1 << WDP1);
    // Interrupt konfigurieren
    WDTCR |= (1 << WDIE);
    // und einschalten
    WDTCR |= (1 << WDE);

    // enable interrupts
    sei();
}
ISR(ADC_vect){
	switch(channel){
		case 1:
			// tankGeber, aktuelle Spannung ueber dem Tankgeber,
			// der Widerstand wird in der Mainloop berechnet.
			AddToFloatAvg((tFloatAvgFilter*)&filterTankGeber, 
						  (tFloatAvgType)(ADC * (VREF / ADC_RESOLUTION) * 100.0));
			// Kanalwechsel, beim naechsten Aufruf wird die Spannung ueber dem 
			// Shunt zur Strommessung gemessen.
			channel = 3;
			break;
		case 3:
			// Feedback, aktuelle Spannung ueber dem Shunt.
			AddToFloatAvg((tFloatAvgFilter*)&filterCurrentFeedback, 
						  (tFloatAvgType)(ADC * (VREF / ADC_RESOLUTION) * 1000.0));
			// Kanalwechsel, beim naechsten Aufruf wird die Spannung ueber dem
			// Tankgeber gemessen.
			channel = 1;
			break;
		default:
			channel = 1;
			break;
	}
	ADMUX = (ADMUX & ~(0x0f)) | (channel & 0x0f);
	// ADC wieder starten fuer den naechsten Durchlauf.
	ADCSRA |= (1 << ADSC);
}

ISR(WDT_vect){
	// Watchdog Interrupt wird jede Sekunde aufgerufen um den Mikrocontroller aufzuwecken.
	// Watchdog Interrupt ist somit erstmal deaktiviert, wird in der Mainloop wieder
	// aktiviert damit es nicht zu einem Reset durch den Watchdog kommt.
}