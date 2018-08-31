
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

int analogPin = 0;
int filterPin = 1;
int strobePin = 2;
int buttonPin = 4;
int pixelDataPin = 6;
int spectrumValue[7];
int filterValue = 60;
int buttonState; 
int lastButtonState = LOW;   // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 100;    // the debounce time; increase if the output flickers


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, pixelDataPin, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


void setup()
{
  Serial.begin(9600);
  
  pinMode(analogPin, INPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(buttonPin, INPUT);

  analogReference(DEFAULT);

  digitalWrite(strobePin, HIGH);  
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop()
{
  buttonState = getButtonState();
  
  filterValue = analogRead(filterPin);
  filterValue = map(filterValue, 0, 1023, 0, 255);
  //Serial.println("----" + (String)filterValue);

  for (int i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(45);

    spectrumValue[i] = analogRead(analogPin);
    spectrumValue[i] = constrain(spectrumValue[i], filterValue, 1023);
    spectrumValue[i] = map(spectrumValue[i], filterValue, 1023, 0, 255);

    Serial.print(spectrumValue[i]);
    Serial.print(" ");
    
    digitalWrite(strobePin, HIGH);
  }

  Serial.println();
  
  // TODO Freitag - random wechseln von modi einbauen
  cycleRandomModes();
}

int getButtonState() {
  int reading = digitalRead(buttonPin);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
    }
  }
  
  lastButtonState = reading;
  
  return buttonState;
}


int mode = random(0, 9);
long startTime = millis(); // Divide by 1000 for seconds
long timePerMode = 360000.0;
void cycleRandomModes() {
  if(millis() > startTime+timePerMode) {
    startTime += timePerMode;
    mode = random(0, 9);
  }
  
  if (buttonState == HIGH) {
    mode = (mode+1) % 9; 
  } 

  switch(mode) {
    case 0:
      blinkStrip();
      break;
    case 1:
      blinkStrip_Split_HighMediumLow();
      break;
    case 2:
      blinkStrip_Pulse_OutToIn();
      break;
    case 3:
      blinkStrip_Pulse_InToOut();
      break;
    case 4:
      blinkFromLeftToRight();
      break;
    case 5:
      blinkFromLeftToRightBassAndHeightsFromLeft();
      break;
    case 6:
      cycleTill();
      break;
    case 7:
      blinkStrip_Right_Blue();
      break;
    case 8:
      tryChristian();
      break;
    default:
      blinkStrip();
      break;  
  }

}



void blinkStrip_Split_HighMediumLow(){
  //links
 uint32_t color = 0;
 for(int i=0;i<30;i++){
   if(i<11){
     //hoehen
     int value = spectrumValue[1];
     color = strip.Color(value,0,0);
   }
   else if(i<21){
     //mitten
     int value = spectrumValue[3];
     color = strip.Color(0,value,0);
   }
   else if(i<31){
    //tiefen
    int value = spectrumValue[6];
    color = strip.Color(0,0,value);
   }
  strip.setPixelColor(i,color); 
 }
 
  
  
  //rechts
  for(int i=31;i<60;i++){
   if(i<41){
     //tiefen
     int value = spectrumValue[6];
     color = strip.Color(0,0,value);
   }
   else if(i<51){
     //mitten
     int value = spectrumValue[3];
     color = strip.Color(0,value,0);
   }
   else if(i<61){
    //hoehen
    int value = spectrumValue[1];
    color = strip.Color(value,0,0);
   }
  strip.setPixelColor(i,color); 
 }
 //strip.setPixelColor(10,strip.Color(255,255,255));
 
 strip.show();
}

//pegel zu bässen links und rechts; blau aus; gruen an
void blinkStrip_Pulse_OutToIn(){
  
  //links 1 -31
 uint32_t green = strip.Color(0,255,0);
  uint32_t blue = strip.Color(0,0,255);
  int value = spectrumValue[4];
  
 for(int i=1;i<31;i++){
  int treshold = (int)(256/30)*i;
  if(value>=treshold){
   strip.setPixelColor(i,green);
  }
  else{
    strip.setPixelColor(i,blue);
  }
 }
 
  
  
  //rechts 31 - 61
  for(int i=31;i<61;i++){
  int treshold = (int)(256/30)*(i-30);
  if(value>=treshold){
   strip.setPixelColor(90-i,green);
  }
  else{
    strip.setPixelColor(90-i,blue);
  }
 }
 
 
  strip.show();
 }
 
 void blinkStrip_Pulse_InToOut(){
  
  //links 1 -31
  uint32_t green = strip.Color(0,255,0);
  uint32_t blue = strip.Color(0,0,255);
  int value = spectrumValue[4];
  
 for(int i=1;i<31;i++){
  
  int treshold = (int)(256/30)*i;
  if(value<=treshold){
   strip.setPixelColor(31-i,green);
  }
  else{
    strip.setPixelColor(31-i,blue);
  }
 }
 
  
  
  //rechts 31 - 61
  for(int i=31;i<61;i++){
    
  int treshold = (int)(256/30)*(i-30);
  if(value<=treshold){
   strip.setPixelColor(i,green);
  }
  else{
    strip.setPixelColor(i,blue);
  }
 }
 
 
  strip.show();
 }
 //strip.setPixelColor(10,strip.Color(255,255,255));


void blinkStrip_Pulse_InToOut2(){
  
  //links 1 -31
  uint32_t out = strip.Color(0,10,0);
  uint32_t in = strip.Color(250,10,0);
  int value = spectrumValue[4];
  
 for(int i=1;i<31;i++){
  
  int treshold = (int)(256/30)*i;
  if(value<=treshold){
   strip.setPixelColor(31-i,out);
  }
  else{
    strip.setPixelColor(31-i,in);
  }
 }
 
  
  
  //rechts 31 - 61
  for(int i=31;i<61;i++){
    
  int treshold = (int)(256/30)*(i-30);
  if(value<=treshold){
   strip.setPixelColor(i,out);
  }
  else{
    strip.setPixelColor(i,in);
  }
 }
 
 
  strip.show();
}

void blinkBinaer(){
  int value = spectrumValue[1];
  
  for(int i =1;i<17;i++){
    
    if(i%2 == 1){
      
    }
    
  }
  
  
  
  
}

void blinkFromLeftToRight(){
  
  int value = spectrumValue[5];//hoehen
  uint32_t in = strip.Color(50, 255, 0);
  uint32_t out = strip.Color(200, 80, 100);

  for(int i=1;i<61;i++){
     int threshold = (255/60)*i;
     
     if(value>=threshold){
       strip.setPixelColor(i,in);
     }
      else
      {
       strip.setPixelColor(i,out);
      }
   
  } 
  
  strip.show();
  
  
}


void blinkFromLeftToRightBassAndHeightsFromLeft(){
  
  int valueBass = spectrumValue[5];//bass
  int valueHeights = spectrumValue[1];
  uint32_t inBass = strip.Color(50, 255, 0);
  uint32_t outBass = strip.Color(200, 0, 0);
  uint32_t inHeights = strip.Color(50, 255, 240);
  uint32_t outHeights = strip.Color(255, 255, 255);
  //bass
  for(int i=1;i<61;i++){
     int threshold = (255/60)*i;
     
     if(valueBass>=threshold){
       strip.setPixelColor(i,inBass);
     }
      else
      {
       strip.setPixelColor(i,outBass);
      }
   
  }
  strip.show();
  
  //heights
  for(int i =61;i>=0;i--){
    int threshold = (255/60)*i;
     
     if(valueHeights>=threshold){
       strip.setPixelColor(i,inHeights);
     }
      else
      {
       strip.setPixelColor(i,outHeights);
      }
    
    
  }
  
  strip.show();
  
  
}
void blinkStrip() {
  int value = spectrumValue[4];
  
  
  
  uint32_t c1 = strip.Color(value, value, value);
  uint32_t c2 = strip.Color(0, value, 0);
  uint32_t c3 = strip.Color(0, 0, value);

  //int numPixel = strip.numPixels();=60
  
    for (int i=0; i < 30; i++) {
      strip.setPixelColor(i, c2);
    }
  
  
  for (int i=31; i < 60; i++) {
    strip.setPixelColor(i, c3);
  }
  
  
  /*for (int i=40; i < 60; i++) {
    strip.setPixelColor(i, c3);
  }*/
  
  
  
  strip.show();
}

void blinkStrip_Right_Blue(){
  //links blau, snare
  int value=spectrumValue[2];
  for(int i=0;i<30;i++){
    strip.setPixelColor(i,strip.Color(0,0,value));
  }
  value=spectrumValue[6];
  for(int i=31;i<60;i++){
    strip.setPixelColor(i,strip.Color(100,value,200));
  }
  strip.show();
  
 
  
}
void cycleTill() {
  for(int i = 0; i<60; i++) {
    int value;
    uint32_t color;
    if(i%2 == 0) {
      color = strip.Color(spectrumValue[5], 0, 0);
    } else {
      color = strip.Color(0, 0, spectrumValue[0]);
    }
    strip.setPixelColor(i, color);
  }
  
  strip.show();
}


void tryChristian() {
  for(int i = 0; i<60; i++) {
    int value;
    uint32_t color;
    if(i%3 == 0) {
      value = spectrumValue[1];
      color = strip.Color(value, 0, 0);
    } else if(i%3 == 1){
      value = spectrumValue[3];
      color = strip.Color(0, value, 0);
    }
      else{
      value = spectrumValue[6];
      color = strip.Color(0, 0, value);
    }
      strip.setPixelColor(i, color);
  }
  
  strip.show();
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
