#ifdef __cplusplus
extern "C" {
#endif

#ifndef _FLOATING_AVERAGE_H_
#define _FLOATING_AVERAGE_H_
/* geklaut aus dem Wiki von RN-Wissen.de zum Thema "Gleitender Mittelwert in C" */
#include <inttypes.h>

// Ueber wieviele Werte soll der gleitende Mittelwert berechnet werden?
// Zulaessige Werte 1..255
#define SIZE_OF_AVG  32

// Datentyp, ueber den der gleitende Mittelwert berechnet werden soll.
// typedef uint16_t tFloatAvgType;
typedef float tFloatAvgType;

// Wird nur intern fuer die Durchschnittsberechnung benutzt.
// Muss Zahlen fassen koennen, die SIZE_OF_AVG mal groesser als tFloatAvgType sind.
// typedef uint16_t tTempSumType;
typedef float tTempSumType;

// Die Struktur, in der die Daten zwischengespeichert werden
typedef struct
 {
	tFloatAvgType aData[SIZE_OF_AVG];
	uint8_t IndexNextValue;		// deswegen darf SIZE_OF_AVR nur zwischen 1 und 255 liegen
 } tFloatAvgFilter;				// kann man aendern bei Bedarf


// Initialisiert das Filter mit einem Startwert.
void InitFloatAvg(tFloatAvgFilter * io_pFloatAvgFilter,
		  tFloatAvgType f_DefaultValue);

// Schreibt einen neuen Wert in das Filter.
void AddToFloatAvg(tFloatAvgFilter * io_pFloatAvgFilter,
		   tFloatAvgType f_NewValue);

// Berechnet den Durchschnitt aus den letzten SIZE_OF_AVG eingetragenen Werten.
tFloatAvgType GetOutputValue(tFloatAvgFilter * io_pFloatAvgFilter);

#endif

#ifdef __cplusplus
}
#endif