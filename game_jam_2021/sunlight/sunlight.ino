/*  this is more simulation than game
 */

byte sunDim = 0; // sun brightness
byte oceanDim = 0; // ocean brightness
byte sunID = false; // am a sun
byte oceanID = false; // am an ocean


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

  if (!sunID & !oceanID) {
    sunDim = (sunDim + getSunLevels()) / 2;
    oceanDim = (oceanDim + getOceanLevels()) / 2;
  }
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      setValueSentOnFace((sunDim << 4) + (oceanDim), f);
    }
  }

  displayLoop();
}

byte getSunLevels() {
  sunDim = 0;
  byte denom = 0;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      sunDim = sunDim + getSun(getLastValueReceivedOnFace(f));
      denom = denom + 1;
    }
    // sunLevels[f] = getLastValueReceivedOnFace(f);
    // sunDim = sunDim + sunLevels[f];
  }
  sunDim = sunDim / denom;
  // sunDim = sunDim * 32; // convert back to hex
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
    // sunLevels[f] = getLastValueReceivedOnFace(f);
    // sunDim = sunDim + sunLevels[f];
  }
  oceanDim = oceanDim / denom;
  // sunDim = sunDim * 32; // convert back to hex
  return oceanDim;
}

byte getOcean(byte data) {
    return (data & 2);//returns bits C and D
}

byte getSun(byte data) {
    return ((data >> 4) & 2);//returns bits C and D
}

void displayLoop() {
  setColor(dim(BLUE, 0));
  setColorOnFace(dim(BLUE, oceanDim * 64), 0);
  setColorOnFace(dim(YELLOW, sunDim * 16), 3);
}
