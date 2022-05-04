
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

//accelerometer
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

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

//set board res, true res, max vel
#define BOARD_RES 16
#define TRUE_RES 512
#define RATIO 32
#define MAX_VEL 13

//general game variables
int playerCol[] = {0,0,35};
int boardX = 0;
int boardY = 0;
int trueX = 0;
int trueY = 0;
int prevBoardX = boardX;
int prevBoardY = boardY;
int points = 0;
int coinX = 0;
int coinY = 0;

//POINT DIGIT POSITIONS
//ones
// 2, 1, 0
//29,30,31
//34,33,32
//61,62,63
//66,65,64
int onesPlace[10][13] = {
  { 1,29,31,34,32,61,63,65,-1,-1,-1,-1,-1}, //0
  { 1,29,30,33,62,66,65,64,-1,-1,-1,-1,-1}, //1
  { 2, 1,31,33,61,66,65,64,-1,-1,-1,-1,-1}, //2
  { 2, 1,31,33,32,63,66,65,-1,-1,-1,-1,-1},
  { 2, 0,29,31,33,32,63,64,-1,-1,-1,-1,-1},
  { 2, 1, 0,29,33,32,63,66,65,-1,-1,-1,-1},
  { 1, 0,29,34,33,32,61,63,66,65,-1,-1,-1},
  { 2, 1, 0,31,32,63,64,-1,-1,-1,-1,-1,-1},
  { 2, 1, 0,29,31,34,33,32,61,63,66,65,64},
  { 2, 1, 0,29,31,33,32,63,66,65,-1,-1,-1} 
};
//tens
// 6, 5, 4
//25,26,27
//38,37,36
//57,58,59
//70,69,68
int tensPlace[10][13] = {
  { 5,25,27,38,36,57,59,69,-1,-1,-1,-1,-1},
  { 5,25,26,37,58,70,69,68,-1,-1,-1,-1,-1},
  { 6, 5,27,37,57,70,69,68,-1,-1,-1,-1,-1},
  { 6, 5,27,37,36,59,70,69,-1,-1,-1,-1,-1},
  { 6, 4,25,27,37,36,59,68,-1,-1,-1,-1,-1},
  { 6, 5, 4,25,37,36,59,70,69,-1,-1,-1,-1},
  { 5, 4,25,38,37,36,57,59,70,69,-1,-1,-1},
  { 6, 5, 4,27,36,59,68,-1,-1,-1,-1,-1,-1},
  { 6, 5, 4,25,27,38,37,36,57,59,70,69,64},
  { 6, 5, 4,25,27,37,36,59,70,69,-1,-1,-1} 
};
//hundreds
//10, 9, 8
//21,22,23
//42,41,40
//53,54,55
//74,73,72
int hundredsPlace[10][13] = {
  { 9,21,23,42,40,53,55,73,-1,-1,-1,-1,-1},
  { 9,21,22,41,54,74,73,72,-1,-1,-1,-1,-1},
  {10, 9,23,41,53,74,73,72,-1,-1,-1,-1,-1},
  {10, 9,23,41,40,55,74,73,-1,-1,-1,-1,-1},
  {10, 8,21,23,41,40,55,72,-1,-1,-1,-1,-1},
  {10, 9, 8,21,41,40,55,74,73,-1,-1,-1,-1},
  { 9, 8,21,42,41,40,53,55,74,73,-1,-1,-1},
  {10, 9, 8,23,40,55,72,-1,-1,-1,-1,-1,-1},
  {10, 9, 8,21,23,42,41,40,53,55,74,73,64},
  {10, 9, 8,21,23,41,40,55,74,73,-1,-1,-1} 
};

//accelerometer variables
int mpuTimeout = 0;
double accelX = 0.0;
double accelY = 0.0;
double velX = 0.0;
double velY = 0.0;

void setup() {
  Serial.begin(9600);
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
    if (accelX > -0.5 && accelX < 0.5) accelX = 0; //rotational deadzone
    accelY = a.acceleration.y;
    if (accelY > -0.5 && accelY < 0.5) accelY = 0; //rotational deadzone
    
    //add acceleration to velocity
    velX-=accelX*1.1;
    velY+=accelY*1.1;

    if (velX < (MAX_VEL*-1)) velX = (MAX_VEL*-1);
    if (velX > MAX_VEL) velX = MAX_VEL;
    if (velY < (MAX_VEL*-1)) velY = (MAX_VEL*-1);
    if (velY > MAX_VEL) velY = MAX_VEL;
    
    //add velocity to position
    trueX+=int(velX);
    trueY+=int(velY);
    
    //reset timer
    mpuTimeout = 50+millis();
  }

  //setting board coords
  boardX = trueX/RATIO;
  boardY = trueY/RATIO;
  //checking for out of bounds death
  if ((trueX >= TRUE_RES) || trueX < RATIO*-1) {reset();}
  if ((trueY >= TRUE_RES) || trueY < RATIO*-1) {reset();}

  //POINTS
  if ((boardX == coinX || boardX == coinX+1 || boardX+1 == coinX || boardX+1 == coinX+1) &&
      (boardY == coinY || boardY == coinY+1 || boardY+1 == coinY || boardY+1 == coinY+1)) {
    if (points < 999) points++;
    if (points == 10) {playerCol[0] = 12; playerCol[1] = 0; playerCol[2] = 25;} //Green
    if (points == 20) {playerCol[0] = 0; playerCol[1] = 28; playerCol[2] = 0;} //Purple
    if (points == 30) {playerCol[0] = 20; playerCol[1] = 3; playerCol[2] = 12;} //Pink
    if (points == 40) {playerCol[0] = 27; playerCol[1] = 10; playerCol[2] = 0;} //Orange
    if (points == 50) {playerCol[0] = 35; playerCol[1] = 0; playerCol[2] = 0;} //Red
    newCoin();
  }
  
  //SHOW ALL LAYERS
  if (prevBoardX != boardX || prevBoardY != boardY) {
    calcAndShow();
  }

  prevBoardX = boardX;
  prevBoardY = boardY;
}

void reset() {
  playerCol[0] = 0;
  playerCol[1] = 0;
  playerCol[2] = 35;
  accelX = 0;
  accelY = 0;
  velX = 0;
  velY = 0;
  trueX = (TRUE_RES/2)-RATIO;
  trueY = (TRUE_RES/2)-RATIO;
  strip.clear();
  points = 0;
  newCoin();
  calcAndShow();
}

void calcAndShow() {
  int pos[] = {0,0,0,0};
  //delete prev ball
  pos[0] = calcPosOfCoords(prevBoardX, prevBoardY);
  pos[1] = calcPosOfCoords(prevBoardX+1, prevBoardY);
  pos[2] = calcPosOfCoords(prevBoardX, prevBoardY+1);
  pos[3] = calcPosOfCoords(prevBoardX+1, prevBoardY+1);
  strip.setPixelColor(pos[0], strip.Color(0,0,0));
  strip.setPixelColor(pos[1], strip.Color(0,0,0));
  strip.setPixelColor(pos[2], strip.Color(0,0,0));
  strip.setPixelColor(pos[3], strip.Color(0,0,0));
  
  //delete old points
  int deletePos[] = {0,1,2,29,30,31,32,33,34,61,62,63,64,65,66, 4,5,6,25,26,27,36,37,38,57,58,59,68,69,70, 8,9,10,21,22,23,40,41,42,53,54,55,72,73,74};
  for (int i=0; i<(sizeof(deletePos) / sizeof(deletePos[1])); ++i) {
    strip.setPixelColor(deletePos[i], strip.Color(0,0,0));
  }
  //show points
  int pointPos[13];
  memcpy(pointPos, onesPlace[(points%10)], sizeof(onesPlace[(points%10)]));
  for (int i=0; i<13; ++i) {
    strip.setPixelColor(pointPos[i], strip.Color(2,2,2));
  }
  
  if (points >= 10) {
    memcpy(pointPos, tensPlace[((points/10)%10)], sizeof(tensPlace[((points/10)%10)]));
    for (int i=0; i<13; ++i) {
      strip.setPixelColor(pointPos[i], strip.Color(2,2,2));
    }

    if (points >= 100) {
      memcpy(pointPos, hundredsPlace[(points/100)], sizeof(hundredsPlace[(points/100)]));
      for (int i=0; i<13; ++i) {
        strip.setPixelColor(pointPos[i], strip.Color(2,2,2));
      }
    }
  }
  
  //show coin
  strip.setPixelColor(calcPosOfCoords(coinX, coinY), strip.Color(20,20,0));
  strip.setPixelColor(calcPosOfCoords(coinX+1, coinY), strip.Color(20,20,0));
  strip.setPixelColor(calcPosOfCoords(coinX, coinY+1), strip.Color(20,20,0));
  strip.setPixelColor(calcPosOfCoords(coinX+1, coinY+1), strip.Color(20,20,0));

  //show ball
  pos[0] = calcPosOfCoords(boardX, boardY);
  pos[1] = calcPosOfCoords(boardX+1, boardY);
  pos[2] = calcPosOfCoords(boardX, boardY+1);
  pos[3] = calcPosOfCoords(boardX+1, boardY+1);
  if (trueX < 0) {
    Serial.println("sda");
    pos[1] = -1;
    pos[3] = -1;
  }
  if (trueX >= TRUE_RES-RATIO) {
    pos[1] = -1;
    pos[3] = -1;
  }
  if (trueY < 0) {
    Serial.println("sda");
    pos[2] = -1;
    pos[3] = -1;
  }
  strip.setPixelColor(pos[0], strip.Color(playerCol[0],playerCol[1],playerCol[2]));
  strip.setPixelColor(pos[1], strip.Color(playerCol[0],playerCol[1],playerCol[2]));
  strip.setPixelColor(pos[2], strip.Color(playerCol[0],playerCol[1],playerCol[2]));
  strip.setPixelColor(pos[3], strip.Color(playerCol[0],playerCol[1],playerCol[2]));
  strip.show();
}

void newCoin() {
  strip.setPixelColor(calcPosOfCoords(coinX, coinY), strip.Color(0,0,0));
  strip.setPixelColor(calcPosOfCoords(coinX+1, coinY), strip.Color(0,0,0));
  strip.setPixelColor(calcPosOfCoords(coinX, coinY+1), strip.Color(0,0,0));
  strip.setPixelColor(calcPosOfCoords(coinX+1, coinY+1), strip.Color(0,0,0));
  coinX = random(1, BOARD_RES-3);
  coinY = random(1, BOARD_RES-3);
}

int calcPosOfCoords(int x, int y) {
  int pos = (y*BOARD_RES)+x;
  if (pos % (BOARD_RES*2) <= BOARD_RES-1) pos += (BOARD_RES-1) - (x*2);
  return pos;
}
