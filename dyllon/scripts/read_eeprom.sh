avrdude -p attiny1616 -c serialupdi -P $1 -U eeprom:r:eeprom-backup.hex:i