/*  this is more simulation than game
 */

byte sunDim = 0; // my brightness
byte sunLevel[6] = {0, 0, 0, 0, 0, 0}; // my sunlight level
byte sunID = false; // am a sun

void setup() {
  setColor(dim(WHITE, 0)); 
}

void loop() {
  // put your main code here, to run repeatedly:
  if (buttonSingleClicked()) {
    sunID = true;
    sunDim = 255;
  }
  if (!sunID) {
    sunDim = (sunDim + getSunLevels()) / 2;
  }
  setColor(dim(YELLOW, sunDim));
  FOREACH_FACE(f) {
    // byte sendData = (blinkRole << 4) + (tradingSignals[f] << 2) + (celebrationState);
    sunLevel[f] = sunDim / 4;
    setValueSentOnFace(sunLevel[f], f);
  }
}

byte getSunLevels() {
  sunDim = 0;
  FOREACH_FACE(f) {
    byte neighbor = sunLevel[f];
    sunDim = sunDim + neighbor;
  }
  sunDim = sunDim * 4; // convert back to hex
  return sunDim;
}
