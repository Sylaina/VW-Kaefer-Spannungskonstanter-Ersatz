# Quellecode für den Mikrocontroller

Im makefile stehen alles nötigen Einstellungen um das Mikrocontrollerprogramm mit Hilfe des avr-gcc, avrdude 
und einem USBTiny auf einen ATtiny85 zu flashen.

# main.h

Das ist das einzige File, dass angefasst werden muss. In der Headerdatei werden die Widerstände des Potis
für die unterschiedlichen Tankfüllstände sowie die Ströme des Anzeigeninstruments festgelegt. Genaueres 
steht in den Kommentaren des Quelltextes.
