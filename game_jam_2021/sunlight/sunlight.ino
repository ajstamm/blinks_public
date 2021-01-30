/*  this is more simulation than game
 */

byte sunDim = 0; // my brightness
// modified from https://github.com/Move38/Simulations/blob/master/ForestFire/ForestFire.ino
byte sunLevels[6] = {0, 0, 0, 0, 0, 0}; // my sunlight level
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
    sunLevels[f] = sunDim / 10;
    setValueSentOnFace(sunLevels[f], f);
  }
}

byte getSunLevels() {
  sunDim = 0;
  FOREACH_FACE(f) {
    sunLevels[f] = getLastValueReceivedOnFace(f);
    sunDim = sunDim + sunLevels[f];
  }
  sunDim = sunDim * 4; // convert back to hex
  return sunDim;
}
