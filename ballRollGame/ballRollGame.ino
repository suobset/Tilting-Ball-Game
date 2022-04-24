//Currently for 16*16
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

//accelerometer
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Digital IO pin connected to the button. This will be driven with a
// pull-up resistor so the switch pulls the pin to ground momentarily.
// On a high -> low transition the button press logic will execute.
#define BUTTON0   0
#define BUTTON1   35

#define PIXEL_PIN    2  // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 256  // Number of NeoPixels

Adafruit_MPU6050 mpu;

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

//set board and true resolution
#define BOARD_RES 16
#define TRUE_RES 512

//general game variables
boolean oldState[] = {HIGH,HIGH};
int boardX = 0;
int boardY = 0;
int trueX = 0;
int trueY = 0;
int prevBoardX = boardX;
int prevBoardY = boardY;
int points = 0;
int coinX = 0;
int coinY = 0;

//accelerometer variables
int mpuTimeout = 0;
double accelX = 0.0;
double accelY = 0.0;
double velX = 0.0;
double velY = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.show();  // Initialize all pixels to 'off'
  reset();

  // Try to initialize mpu
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(100);
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  //Physics
  if (millis() > mpuTimeout) {
    //get accelerometer input
    accelX = a.acceleration.x-0.35; //account for miscalibration
    if (accelX > -0.15 && accelX < 0.15) accelX = 0; //rotational deadzone
    accelY = a.acceleration.y;
    if (accelY > -0.15 && accelY < 0.15) accelY = 0; //rotational deadzone
    
    //add acceleration to velocity
    velX-=accelX/4;
    velY+=accelY/4;
    
    //add velocity to position
    trueX+=int(velX);
    trueY+=int(velY);
    
    //print acceleration and velocity for debugging
    Serial.print("Acceleration X: ");
    Serial.print(accelX);
    Serial.print(", Y: ");
    Serial.print(accelY);
    Serial.print(" Velocity X: ");
    Serial.print(velX);
    Serial.print(", Y: ");
    Serial.print(velY);
    Serial.println("");
    
    //reset timer
    mpuTimeout = 50+millis();
  }
  
  //BUTTON MOVEMENT
  boolean newState = digitalRead(BUTTON0); // Get current button state
  if((newState == LOW) && (oldState[0] == HIGH)) { // Check if state changed from high to low (button press)
    delay(20); // Short delay to debounce button.
    newState = digitalRead(BUTTON0); // Check if button is still low after debounce.
    if(newState == LOW) { // Yes, still low
      trueX++;
    }
  }

  oldState[0] = newState; // Set the last-read button state to the old state.
  
  newState = digitalRead(BUTTON1);
  if((newState == LOW) && (oldState[1] == HIGH)) {
    delay(20);
    newState = digitalRead(BUTTON1);
    if(newState == LOW) {
      trueY++;
    }
  }

  oldState[1] = newState;

  //setting board coords
  boardX = trueX/(TRUE_RES/BOARD_RES);
  boardY = trueY/(TRUE_RES/BOARD_RES);
  //checking for out of bounds death
  if ((boardX >= BOARD_RES) || boardX < 0) {reset();}
  if ((boardY >= BOARD_RES) || boardY < 0) {reset();}

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
  accelX = 0;
  accelY = 0;
  velX = 0;
  velY = 0;
  trueX = TRUE_RES/2;
  trueY = TRUE_RES/2;
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
  coinX = random(1, BOARD_RES-2);
  coinY = random(1, BOARD_RES-2);
  int pos = calcPosOfCoords(coinX, coinY);
  strip.setPixelColor(pos, strip.Color(20,20,0));
  strip.show();
}

int calcPosOfCoords(int x, int y) {
  int pos = (y*BOARD_RES)+x;
  if (pos % (BOARD_RES*2) <= BOARD_RES-1) pos += (BOARD_RES-1) - (x*2);
  return pos;
}
