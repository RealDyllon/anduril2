avrdude -p attiny1616 -c serialupdi -P $1 -U flash:r:old-firmware.hex:i