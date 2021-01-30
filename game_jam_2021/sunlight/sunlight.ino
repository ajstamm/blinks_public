/*  this is more simulation than game
 */

byte sunDim = 0; // sun brightness
byte oceanDim = 0; // ocean brightness
byte sunID = false; // am a sun
byte oceanID = false; // am an ocean
byte plantDim = 0;
byte flowerDim = 0;
byte dayLength = 10000;
// #define dawn 5000;
// #define dusk 5000;
// Timer sunTimer;


// modified from https://github.com/Move38/Simulations/blob/master/ForestFire/ForestFire.ino
// byte sunLevels[6] = {0, 0, 0, 0, 0, 0}; // my sunlight level
// byte sendData = (signalState << 2) + (gameMode);
// << 2 means push data 2 digits before concatenating
// & 3 gives the last 3 digits
// byte sendData = (sunDim << 4) + (oceanDim);
// 

// store dimness level 0 to 7 
// or 16 sun, 4 water

// could add timer for sun and maybe water
// later tie water to be dependent on sun?

void setup() {
}

void loop() {
  // put your main code here, to run repeatedly:
  if (buttonSingleClicked()) {
    sunID = true;
    sunDim = 15;
  }
  if (buttonDoubleClicked()) {
    oceanID = true;
    oceanDim = 3;
  }

  if (!sunID) {
    sunDim = (sunDim + getSunLevels()) / 2;
  } else {
    int pulseProgress = millis() % dayLength;
    byte pulseMapped = map(pulseProgress, 0, dayLength, 0, 15);
    sunDim = sin8_C(pulseMapped);
  }
  if (!oceanID) {
    oceanDim = (oceanDim + getOceanLevels()) / 2;
  }
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      setValueSentOnFace((sunDim << 4) + (oceanDim), f);
    }
  }
  
  if (sunDim > 7 & oceanDim > 1 & plantDim < 255) {
    plantDim = plantDim + 5;
  }
  if (sunDim > 7 & oceanDim > 1 & plantDim > 250 & flowerDim < 250) {
    flowerDim = flowerDim + 5;
  }
  if (sunDim < 8 & oceanDim > 1 & plantDim > 50 & flowerDim == 0) {
    plantDim = plantDim - 5;
  }
  if (sunDim < 8 & oceanDim > 1 & flowerDim > 0) {
    flowerDim = flowerDim - 5;
  }
  displayLoop();
  
}

byte sunSet() {
  
}



byte getSunLevels() {
  sunDim = 0;
  byte denom = 0;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      sunDim = sunDim + getSun(getLastValueReceivedOnFace(f));
      denom = denom + 1;
    }
  }
  sunDim = sunDim / denom;
  return sunDim;
}

byte getOceanLevels() {
  oceanDim = 0;
  byte denom = 0;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      oceanDim = oceanDim + getOcean(getLastValueReceivedOnFace(f));
      denom = denom + 1;
    }
  }
  oceanDim = oceanDim / denom;
  return oceanDim;
}

byte getOcean(byte data) {
    return (data & 3);//returns bits C and D
}

byte getSun(byte data) {
    return ((data >> 4) & 15);//returns bits C and D
}

void displayLoop() {
  setColor(dim(ORANGE, 0));
  setColorOnFace(dim(BLUE, (oceanDim + 1) * 64 - 1), 0);
  setColorOnFace(dim(YELLOW, (sunDim + 1) * 16 - 1), 3);
  setColorOnFace(dim(GREEN, plantDim), 1);
  setColorOnFace(dim(RED, flowerDim), 2);
}
