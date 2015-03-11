#!/bin/bash



file=$1; #bootloader+usblinky.raw;

function flash {
sleep 0.1;

say "go"

#//fuse
avrdude -c usbtiny  -p attiny85 -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m

fail=false;

#//flash
(avrdude -v  -c usbtiny  -p attiny85 -U flash:w:$file:r -B 3 -s -u ) && say "OK" || fail=true;


if $fail;
then
say "FLASH FAILED";
exit;

fi;

#//lock
avrdude -c usbtiny  -p attiny85 -U lock:w:0x2f:m

say "next";
sleep 1;
}


while true; do (avrdude -c usbtiny -B 10 -p attiny85 2>&1 | grep "Device signature") && flash; sleep 1; done
