//
//  main.h

/* TODO: Tankgeber und Tankanzeige vermessen, Ergebnisse hier eintragen */

// Widerstandswerte des Tankgebers bei entsprechenden Fuellstaenden in Ohm
#define TANKSENSOREMPTY	47.0	// Tank leer
#define TANKSENSORLOW	40.0	// Tank Reserve (5l-10l im Tank)
								// wird nur fuer die optionale LED benoetigt.
#define TANKSENSORFULL	 9.0	// Tank voll

// Stromaufnahme des Instruments, 
// Zeiger auf den entsprechenden Positionen, Strom in Ampere
#define TANKEMPTY		0.020	// Tankanzeige leer
#define TANKFULL		0.160	// Tankanzeige voll, sollte deutlich weniger als 500mA 
    							// sein ansonsten ist wahrscheinlich das Instrument defekt

/* TODO: Wird die Schaltung nicht veraendert dann ab hier keine Aenderungen vornehmen */

// Parameter fuer den Regler
// Instrument ist recht träge, daher hier keine Optimierung noetig.
#define KP				2.0
#define KI				1.0
#define KD				0.0

// Die folgenden Werte nur an den Schaltplan anpassen
// Aenderungen sind hier idR nicht noetig.
#define RV				100.0
#define SHUNT			10.0 
#define VCC				5.000
#define VREF			2.56
#define ADC_RESOLUTION	1024.0

// Hilfsvariablen fuer die jeweiligen Kennlinien, Kennlinien wurden als Graden
// approximiert. Das stimmt zwar nicht exakt aber es genuegt fuer die Tankanzeige.
#define M_SENSOR	((TANKSENSOREMPTY - TANKSENSORFULL) / (VCC * ((TANKSENSOREMPTY / (TANKSENSOREMPTY + RV)) - (TANKSENSORFULL / (TANKSENSORFULL + RV)))))
#define B_SENSOR	(TANKSENSOREMPTY - (VCC * TANKSENSOREMPTY / (TANKSENSOREMPTY + RV) * M_SENSOR))
#define M_SOURCE	((TANKFULL - TANKEMPTY) / (TANKSENSORFULL - TANKSENSOREMPTY))
#define B_SOURCE	(TANKEMPTY - (M_SOURCE * TANKSENSOREMPTY))

#ifndef main_h
#define main_h
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

void setUpAvr(void);

#endif /* main_h */
