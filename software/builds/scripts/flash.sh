#//fuse
avrdude -c usbtiny  -p attiny85 -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m


#//flash
avrdude -v -v -v -c usbtiny  -p attiny85 -U flash:w:flash.hex:i -B 10 


#//lock
avrdude -c usbtiny  -p attiny85 -U lock:w:0x2f:m
