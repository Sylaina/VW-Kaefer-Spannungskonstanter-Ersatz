# VW-Käfer Spannungskonstanter-Ersatz für die Tankanzeige

Die hier gezeigte Schaltung dient dem Ersatz des im VW Käfer eingesetzten Spannungskonstanter für die Tankanzeige.

Der Füllstand des Tanks im Käfer ist mehr eine Schätzung als eine gute Anzeige. Ich habe in der Vergangenheit verschiedene Tankgeber und Tankanzeigen ausprobiert aber nie war die Tankanzeige zufriedenstellend einzustellen. Nach genauerer Analyse der verwendeten Technik ist, meiner Meinung nach, das Anzeigeinstrument das Problem. Der Tankgeber wird so im Grunde sogar heute noch in PKWs eingesetzt, hier betätigt ein Schwimmer ein Potentiometer (kurz Poti) welches seinen Widerstandswert dadurch ändert. Das Poti ist in Reihe geschaltet mit der Tankanzeige. Über Poti und Tankanzeige liegt eine konstante Spannung, durch die Widerstandsänderung des Potis ändert sich somit der Strom durch Poti und Tankanzeige. Dieser Strom erwärmt die Tankanzeige was letzten Endes die Tanknadel im Instrument bewegt.
Bei mir war das Problem, dass ich entweder die Leerstellung oder die Vollstellung richtig einstellen konnte. Ich habe es aber nie geschafft, die Leer- und Voll-Stellung gleichzeitig richtig einzustellen.

# Was macht die Schaltung?

Die hier vorgestellte Schaltung ersetzt den originalen Spannungskonstanter und prägt dem Anzeigeinstrument, abhängig von der Potistellung des Tankgebers, einen konstanten Strom ein. Lediglich die Widerstandswerte des Potis für Leer, Reserve (~5l Tankfüllung) und Voll müssen ermittelt werden sowie der Strom, den das Anzeigeinstrument für den Zeigerausschlag auf Tank Voll und Tank Leer aufnimmt.

Die Schaltung misst mit einem Mikrocontroller (AVR ATtiny85) den Widerstand des Potis vom Tankgeber und wandelt diesen Widerstand in einen proportionalen Strom für die Tankanzeige um.

# Wie installiert man die Schaltung

Man entfernt den alten Spannungskonstanter und baut die Schaltung an der Stelle ein, in der auch der Spannungskonstanter eingebaut war. Dann muss nur noch das Instrument, die Schaltung und der Tankgeber angeschlossen werden und schon sollte alles funktionieren. Das Programm für den Mikrocontroller sowie die Gerberfiles für die Platine findet ihr in diesem github.

Viel Spass beim Nachbauen
