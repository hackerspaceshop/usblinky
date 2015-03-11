/*

This is an arduino sketch for the usblinky project.

Get one here: http://www.hackerspaceshop.com/blinky/usblinky.html



usblinky is a small usbstick that contains a ATTINY85 MCU and is used to drive WS2811/12/12B pixels
Once programmed usblinky can be connected to a standard 5V USB powersupply and run on its own without a connected computer
Its used in many different illumination projects like moodlamps, 21st century lavalamps, art installations, illuminated pictureframes, and much more.
  
It makes use of the miconucleus bootloader that comes with the Digistump from Disgispark.
Please download their Arduino Environment to program the usblinky.
  
  
  
To upload the sketch plug in the stick and hit upload before it starts any program the bootloader might run.
If you have this sketch running on your usbtiny already simply hold the button down till the leds turn off and keep it pressed a little longer
The leds will turn to a static blue color. You are now in programmer mode and can upload your code.





This code was written by Florian 'overflo' Bittner for the hackerspaceshop.com and uses libraries from adafruit.
Please support this project and its continued development by buying something from hackerspaceshop.com and adafruit.com




-------------------------------------------------------------------------
  usblinky is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  usblinky is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with usblinky.  If not, see
  <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------
 
*/


// save/read runtime state
#include <EEPROM.h>

// for reset in software
#include <avr/wdt.h>
// blinky goodness 
#include <Adafruit_NeoPixel.h>



// how many pixels are on the whole strand?
#define PIXELS 20


// button is connected to this pin (2nd from right)
#define BUTTON_PIN 2 // SCK

// strip is connected to this pin (2nd from left) 
#define LEDSTRIP_PIN 0 // mosi on attiny 85




// globals
byte brightness=255;

//uint8_t offcounter=0;


// set from ISR and handle_button()
uint8_t button_pressed=0;
uint8_t longpress=0;



// millis() time values are big.
uint32_t button_down =0;
uint32_t button_up =0;








// all the programs
// the numbering is not important as long as it is unique
// RUNTIME_STATE is looked up in loop() and is changed in button handler ISR on short press

const uint8_t STATE_OFF =0;
const uint8_t STATE_WHITE =1;
const uint8_t STATE_EVENING_WHITE =2;
const uint8_t STATE_MOODLIGHT =3;
const uint8_t STATE_RAINBOW_UP =4;
const uint8_t STATE_RAINBOW_DOWN =5;
const uint8_t STATE_RAINBOW_AROUND=6;
const uint8_t STATE_POLICE=7;
const uint8_t STATE_SINGLEPOINT=8;
const uint8_t STATE_STARTUP=100;


// current program runnig start_up() changes this to the last known state from eeprom
uint8_t RUNTIME_STATE = STATE_RAINBOW;




/*


THE PARTY STARTS BELOW ..

\o/

*/







// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, LEDSTRIP_PIN,NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {

  // initialize the ledstrip
  strip.begin();
  
  //full on
  strip.setBrightness(brightness);

  // no pixels set, sp everything if off here
  strip.show(); 


  // the button is pulled to ground when pressed
  pinMode(BUTTON_PIN,INPUT);
  digitalWrite(BUTTON_PIN,1); // pullup  
  // used to handle the button events asynchronous
  attachInterrupt(0,button_pressed_ISR,CHANGE);

}




// called from interrupt, keep this function as small as possible, no delay() in here!
void button_pressed_ISR()
{

  button_pressed=1;
  if(!digitalRead(BUTTON_PIN)) 
  { 
    button_pressed=1;
    button_down=millis();
  }  

  if(digitalRead(BUTTON_PIN)) 
  {
    button_pressed=0;
    button_up=millis();    
  }
}








// this is called all the time over and over and over again..
void loop() {

  // always handle the button and register short and long presses
  // button_pressed is set from the interrupt service routine (see attachInterrupt() in setup())
  if(button_pressed)    handle_button(); 



  // this is a statemachine. do what the current state says.
  // effects are declared below in this file, look at the funtions for a detailed explaination whats going on there
  switch(RUNTIME_STATE)
  {
  case  STATE_WHITE:
    effect_singlecolor(strip.Color(255,255,255));
  break; 


  case STATE_EVENING_WHITE:
    effect_singlecolor(strip.Color(255,255,60));
  break;       

  case STATE_MOODLIGHT:
    effect_moodlight(700);
  break;     

  case STATE_RAINBOW_DOWN:
    effect_rainbow_down(10);
  break;  

  case STATE_RAINBOW_UP:
    effect_rainbow_up(100);
  break;  

  case STATE_RAINBOW_AROUND:
    effect_rainbow_around(20); 
  break;  

  case STATE_POLICE:
    effect_police(); 
  break;  


  case STATE_SINGLEPOINT:
    effect_singlepoint(120); 
  break; 



  case STATE_STARTUP:
    start_up();
  break;  

  case STATE_OFF:  
  default:
    do_nothing();
  break;  

  }
}













// triggered if the variable button_pressed is set ..
void handle_button()
{ 

  // button not long pressed by default
  byte longpress=0;

  // stay in here as long as the button is down
  while(button_pressed)
  {
    // button_down  contains millis() set in ISR on button down event
    if(millis()-button_down >500)
    {
      // button down for half a second or more? its a long press.
      longpress=1; 

      // decrement the brightness
      brightness =brightness - 5;




      // we are off?
      if(brightness<=0)
      {
        shut_down(); 
        // after shut_down switch the state to OFF
        RUNTIME_STATE=STATE_OFF;
      }

      // if the button is still pressed after we did a shutdown we want to reset and enter programmer mode   
      // if button was pressed for longer than 10 secs .. reset
      if(millis()-button_down >5000)
      {
        reset_mcu();
      }

      // if we got here we are still running .. so decreent that brightness we set earlier
      strip.setBrightness(brightness);
      strip.show();

      delay(50);
    } // longpress - close
  } // while button pressed - close


  // button released!
  if(longpress)
  {
    longpress=0;
  }
  else
  {
    // short press?
    if(RUNTIME_STATE==STATE_OFF)
    {
      RUNTIME_STATE=STATE_STARTUP;
      return;
    }

 

    // switch proram if button pressed shortly
    switch(RUNTIME_STATE)
    {
    case  STATE_WHITE:
      RUNTIME_STATE= STATE_EVENING_WHITE;
    break;    
    case STATE_EVENING_WHITE:
      RUNTIME_STATE= STATE_MOODLIGHT;    
    break; 
    case STATE_MOODLIGHT:
      RUNTIME_STATE= STATE_RAINBOW_DOWN;
    break;     
    case STATE_RAINBOW_DOWN:
      RUNTIME_STATE= STATE_RAINBOW_UP;
    break;   
    case STATE_RAINBOW_UP:
      RUNTIME_STATE= STATE_RAINBOW_AROUND;
    break;  
    case STATE_RAINBOW_AROUND:
      RUNTIME_STATE= STATE_POLICE;
    break;   
    case STATE_POLICE:
      RUNTIME_STATE= STATE_SINGLEPOINT;
    break;   
    case STATE_SINGLEPOINT:
      RUNTIME_STATE= STATE_WHITE;
    break;    
    default:
      RUNTIME_STATE= STATE_STARTUP;
    break;   
    }


  // debounce button 
  delay(100);

  } // not long pressed






}














// called if button is pressed till lights go off and then still pressed for some time..
void reset_mcu()
{


  strip.setBrightness(255);


  // fade to blue
  for(uint8_t j=0;j<255;j++)
  {
    for( uint8_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0,0,j));
    }
    strip.show();
  }



  // this is a super neat feature for software resetting an AVR MCU.
  // enable the wtchdog timer and do nothing till it resets the microcontroller.
  // after that the bootloader triggers and the usblinky is in programmable mode
  wdt_enable(WDTO_15MS);
  while(1) { }


}




















/*  PATTERNS AND EFFECTS BELOW HERE */





// this turns on all the leds bright white
void effect_singlecolor(uint32_t color)
{

  for( uint8_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
  if(breakable_delay(1000)){    return; }

}




// turn all to the same color, cycle colors slowly.. 
void effect_moodlight(int wait)
{
  uint8_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((j) & 255));  
    }  
    strip.show();
    if(breakable_delay(wait/2)){
      return;
    }
    
   for(i=0; i<strip.numPixels(); i+=2) {
      strip.setPixelColor(i, Wheel((j+1) & 255));  
    }  
    strip.show();
    if(breakable_delay(wait/2)){
      return;
    }
    
    
    
    
  }
}



// draw a rainbow going from top to bottom
void effect_rainbow_down(int wait)
{
  uint8_t i, j;
  uint32_t color = 0;

  for(j=0; j<256; j++) 
  { 
    for(i=0; i< (strip.numPixels()/2); i++) 
    {
      color =  Wheel(((i * 256 / strip.numPixels()) + j) & 255);

      strip.setPixelColor(i, color);
      strip.setPixelColor(strip.numPixels()-i-1, color);      
    }

    strip.show();

    if(breakable_delay(wait)){
      return;
    }

  }  
}



// rainbow from bottom to top
void effect_rainbow_up(int wait)
{
  uint8_t i, j;
  uint32_t color = 0;

  for(j=0; j<256; j++) 
  {   
    for(i=0; i<= (strip.numPixels()/2); i++) 
    {
      
      color =  Wheel(((i * 256 / strip.numPixels()) + j) & 255);

      strip.setPixelColor((strip.numPixels()/2) - i, color);
      strip.setPixelColor((strip.numPixels()/2) + i -1, color);      
    }

    strip.show();

    if(breakable_delay(wait)){
      return;
    }

  }  
}


// each pixel gets its own rainbow color .. cycle trough
void effect_rainbow_around(int wait)
{
  uint8_t i, j;

  for(j=0; j<256; j++) { 
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));

    }
    strip.show();
    if(breakable_delay(wait)){
      return;
    }
  }

}










// whoop whoop its da sound of the police
void effect_police()
{

  //short blue
  for( uint8_t i=0; i<strip.numPixels(); i++) { strip.setPixelColor(i, strip.Color(0,0,255)); }
  strip.show();
  if(breakable_delay(80)){ return; }
  
  //short off
  for( uint8_t i=0; i<strip.numPixels(); i++) { strip.setPixelColor(i, strip.Color(0,0,0)); }
  strip.show();
  if(breakable_delay(40)){ return; }
  
  //short blue
  for( uint8_t i=0; i<strip.numPixels(); i++) { strip.setPixelColor(i, strip.Color(0,0,255)); }
  strip.show();
  if(breakable_delay(80)){ return; }
  
  //long off
  for( uint8_t i=0; i<strip.numPixels(); i++) { strip.setPixelColor(i, strip.Color(0,0,0)); }
  strip.show();
  if(breakable_delay(300)){ return; }


}






// one pixel going around changing color
void effect_singlepoint(int wait)
{
  uint8_t i, j, halfstrip_len;

  //all off
  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();



  halfstrip_len = strip.numPixels()/2;


  // set pixel to one color, display, wait, set current pixel off.. next
  for(j=0; j<256; j++) { 


    // move down

    for(i=0; i< halfstrip_len; i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / halfstrip_len) + j) & 255));
      strip.setPixelColor(strip.numPixels()-i-1, Wheel(((i * 256 / halfstrip_len) + j) & 255)); 
      strip.show();

      if(breakable_delay(wait)){return;}
      strip.setPixelColor(i, 0);
      strip.setPixelColor(strip.numPixels()-i-1, 0); 
    }



    // move up

    for(i=0; i< (strip.numPixels()/2); i++) {
      strip.setPixelColor(halfstrip_len+i, Wheel(((i * 256 / halfstrip_len) + j) & 255));
      strip.setPixelColor(halfstrip_len-i-1, Wheel(((i * 256 / halfstrip_len) + j) & 255)); 
      strip.show();
      if(breakable_delay(wait)){return;}
      strip.setPixelColor(halfstrip_len +i, 0);
      strip.setPixelColor(halfstrip_len-i-1, 0); 
    }



  }


}




















void do_nothing()
{
  // nothing to see here.. move on ;) 
}











// runs on start up, sets maximum brightness, reads eeprom, sets runtme state
void start_up()
{
  brightness=255;
  strip.setBrightness(brightness);


  RUNTIME_STATE= EEPROM.read(300);
  // if this runs for the first time eeprom is empty
  if (RUNTIME_STATE < STATE_WHITE || RUNTIME_STATE > STATE_SINGLEPOINT) {
      RUNTIME_STATE=STATE_WHITE;
  }

}





// turn OFF all leds
void shut_down()
{
  // turn off the leds
  for(uint8_t i=0; i<strip.numPixels(); i++) strip.setPixelColor(i,  0);
  strip.show();

  // save current runtime state in eeprom
 
  EEPROM.write(300, RUNTIME_STATE); 



}













// HELPER funtions below

// a delay() that can be interrupted by a button press
uint8_t breakable_delay(int timeout)
{
  // this is not 100% the timeout specified, but for our application that does not matter at all
  for(int i=0;i<timeout;i++)
  {
    // button_pressed is set from ISR
    if(button_pressed) return 1;
    delay(1);   
  }
  return 0;
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}


