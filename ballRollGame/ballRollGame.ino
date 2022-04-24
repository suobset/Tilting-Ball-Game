#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Digital IO pin connected to the button. This will be driven with a
// pull-up resistor so the switch pulls the pin to ground momentarily.
// On a high -> low transition the button press logic will execute.
#define BUTTON0   0
#define BUTTON1   35

#define PIXEL_PIN    2  // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 256  // Number of NeoPixels

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

boolean oldState[] = {HIGH,HIGH};
int boardX = 7;
int boardY = 7;
int prevBoardX = boardX;
int prevBoardY = boardY;
int points = 0;
int coinX = 0;
int coinY = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.show();  // Initialize all pixels to 'off'
  reset();
}

void loop() {
  //BUTTON MOVEMENT
  
  boolean newState = digitalRead(BUTTON0); // Get current button state
  if((newState == LOW) && (oldState[0] == HIGH)) { // Check if state changed from high to low (button press)
    delay(20); // Short delay to debounce button.
    newState = digitalRead(BUTTON0); // Check if button is still low after debounce.
    if(newState == LOW) { // Yes, still low
      boardX = boardX+1;
      if (boardX > 15 || boardX < 0) {
        reset();
      }
    }
  }

  oldState[0] = newState; // Set the last-read button state to the old state.
  
  newState = digitalRead(BUTTON1);
  if((newState == LOW) && (oldState[1] == HIGH)) {
    delay(20);
    newState = digitalRead(BUTTON1);
    if(newState == LOW) {
      boardY = boardY+1;
      if (boardY > 15 || boardY < 0) {
        reset();
      }
    }
  }

  oldState[1] = newState;

  //POINTS
  if (boardX == coinX && boardY == coinY) {
    strip.setPixelColor(points, strip.Color(1,1,1));
    points++;
    strip.show();
    calcAndShowCoin();
  }

  //SHOW BALL
  if (prevBoardX != boardX || prevBoardY != boardY) {
    calcAndShowBall();
  }

  prevBoardX = boardX;
  prevBoardY = boardY;
}

void reset() {
  boardX = 7;
  boardY = 7;
  strip.clear();
  points = 0;
  calcAndShowBall();
  calcAndShowCoin();
}

void calcAndShowBall() {
  int pos = calcPosOfCoords(prevBoardX, prevBoardY);
  strip.setPixelColor(pos, strip.Color(0,0,0));
    
  pos = calcPosOfCoords(boardX, boardY);
  strip.setPixelColor(pos, strip.Color(0,0,35));
  strip.show();
}

void calcAndShowCoin() {
  coinX = random(0, 15);
  coinY = random(0, 15);
  int pos = calcPosOfCoords(coinX, coinY);
  strip.setPixelColor(pos, strip.Color(20,20,0));
  strip.show();
}

int calcPosOfCoords(int x, int y) {
  int pos = (y*16)+x;
  if (pos % 32 <= 15) pos += 15 - (x*2);
  return pos;
}
