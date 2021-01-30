/*  this is more simulation than game
 */

byte sunDim = 0; // sun brightness
byte oceanDim = 0; // ocean brightness
byte sunID = false; // am a sun
byte oceanID = false; // am an ocean
byte plantDim = 0;
byte flowerDim = 0;
#define DAY_LENGTH 2000

// store dimness level 16 sun, 4 water
// byte sendData = (sunDim << 4) + (oceanDim);
// << 2 means push data 2 digits before concatenating
// & 3 gives the last 2 digits


// could add timer for sun
// tie water to be dependent on sun?

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
    int pulseProgress = millis() % DAY_LENGTH;
    byte pulseMapped = map(pulseProgress, 0, DAY_LENGTH, 0, 255);
    sunDim = map(sin8_C(pulseMapped), 0, 255, 0, 15);
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
  byte sunNeighbor = 0;
  byte denom = 0;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      sunNeighbor = sunNeighbor + getSun(getLastValueReceivedOnFace(f));
      denom = denom + 1;
    }
  }
  sunNeighbor = sunNeighbor / denom;
  return sunNeighbor;
}

byte getOceanLevels() {
  byte oceanNeighbor = 0;
  byte denom = 0;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      oceanNeighbor = oceanNeighbor + getOcean(getLastValueReceivedOnFace(f));
      denom = denom + 1;
    }
  }
  oceanNeighbor = oceanNeighbor / denom;
  return oceanNeighbor;
}

byte getOcean(byte data) {
    return (data & 3);//returns bits 5 and 6
}

byte getSun(byte data) {
    return ((data >> 4) & 15);//returns bits 1 to 4
}

void displayLoop() {
  setColor(dim(ORANGE, 0));
  setColorOnFace(dim(BLUE, (oceanDim + 1) * 64 - 1), 0);
  setColorOnFace(dim(YELLOW, (sunDim + 1) * 16 - 1), 3);
  setColorOnFace(dim(GREEN, plantDim), 1);
  setColorOnFace(dim(RED, flowerDim), 2);
}
