/*   Abigail Stamm
 *   GGJ 2021, 31 January 2021
 *   this is more simulation than game
 */

byte sunDim = 2; // actual sun brightness 2*10+5
byte oceanDim = 0; // ocean brightness
byte oceanID = false; // am an ocean
byte oceanHue = 50;
byte plantHue = 50;
byte flowerDim = 0;
byte flowerHue1 = 50;
byte flowerHue3 = 50;
byte flowerHue2 = 50;
byte snowSat = 0;

// synchronization values, from puzzle 101
Timer syncTimer;
#define PERIOD_DURATION 5000
#define BUFFER_DURATION 100
byte neighborState[6];
byte syncVal = 0;
int dayLength = 5000; // (random(2) + 4) * 1000;
int monthLength = dayLength * (random(1) + 2) * (random(3) + 7) / 10;
int seasonLength = monthLength  * 3 * (random(3) + 7) / 10;

// store dimness level 32 water
// byte sendData = (sunDim << 5) + (oceanDim);
// << 5 means push data 5 digits left before concatenating
// & 31 gives the last 5 digits

void setup() {
}

void loop() {
  if (buttonSingleClicked()) {
    oceanID = true;
    oceanDim = 31;
  }
  // sun rising, then setting, then rising again ...
  int pulseProgress = millis() % dayLength;
  byte pulseMapped = map(pulseProgress, 0, dayLength, 0, 255);
  // force to range 10 to 25
  sunDim = map(sin8_C(pulseMapped), 0, 255, 100, 255);

  // summer, then winter, then summer again ...
  pulseProgress = millis() % seasonLength;
  pulseMapped = map(pulseProgress, 0, seasonLength, 0, 255);
  snowSat = sin8_C(pulseMapped);
  // force range to 0-25
  snowSat = (snowSat - 5) / 25;

  if (!oceanID) {
    oceanDim = (oceanDim + getOceanLevels()) / 2.1;
  } else {
      pulseProgress = millis() % monthLength;
      pulseMapped = map(pulseProgress, 0, monthLength, 0, 31);
    // force into 0-31 range
      oceanDim = sin8_C(pulseMapped);
  }
  // force ranges to one tenth of value
  oceanHue = map(oceanDim, 0, 31, 100, 160);
  plantHue = map(oceanDim, 0, 31, 50, 100);
   
  // run syncLoop to determine sun brightness synced across blinks
  syncLoop();
  
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      setValueSentOnFace((syncVal << 5) + oceanDim, f);
    }
  }
  
  if (sunDim > 200 & plantHue > 75 & flowerDim < 25) {
    flowerDim = flowerDim + 1;
  }
  if (sunDim < 150 & flowerDim > 0) {
    flowerDim = flowerDim - 1;
  }
  // tie flowers to plants, will need to multiply by 10
  flowerHue1  = plantHue - map(flowerDim, 0, 25, 0, plantHue);
  flowerHue2  = plantHue - map(flowerDim, 0, 25, 0, plantHue - 15);
  flowerHue3  = plantHue - map(flowerDim, 0, 25, 0, plantHue - 30);
  displayLoop();
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
    return (data & 31); // returns bits 2 to 6
}

void displayLoop() {
//  setColor(makeColorHSB(plantHue * 10, 255 - (snowSat * 25 + 5), sunDim));
  if (oceanID) {
    setColorOnFace(dim(makeColorHSB(oceanHue, 255 - (255 - (snowSat * 25 + 5)) / 2, 255), sunDim), 0);    
  } else {
    setColorOnFace(dim(makeColorHSB(flowerHue3, 255 - (snowSat * 25 + 5), 255), sunDim), 0);
  }
  setColorOnFace(dim(makeColorHSB(plantHue, 255 - (snowSat * 25 + 5), 255), sunDim), 1);
  if (snowSat > 4 & flowerHue1 < 4) {
    setColorOnFace(dim(makeColorHSB(flowerHue2, 155, 255), sunDim), 2);
  } else {
    setColorOnFace(dim(makeColorHSB(flowerHue1, 255 - (snowSat * 25 + 5), 255), sunDim), 2);
  }  
  setColorOnFace(dim(makeColorHSB(plantHue, 255 - (snowSat * 25 + 5), 255), sunDim), 3);
  setColorOnFace(dim(makeColorHSB(flowerHue2, 255 - (snowSat * 25 + 5), 255), sunDim), 4);
  setColorOnFace(dim(makeColorHSB(plantHue, 255 - (snowSat * 25 + 5), 255), sunDim), 5);
//  setColorOnFace(MAGENTA, 5); // doesn't flicker
}

// below copied from puzzle 101
byte getSyncVal(byte data) {
  return (data >> 5) & 1;
}

void syncLoop() {
  bool didNeighborChange = false;
  // look at our neighbors to determine if one of them passed go (changed value)
  // note: absent neighbors changing to not absent don't count
  FOREACH_FACE(f) {
    if (isValueReceivedOnFaceExpired(f)) {
      neighborState[f] = 2; // this is an absent neighbor
    }
    else {
      byte data = getLastValueReceivedOnFace(f);
      if (neighborState[f] != 2) {  // wasn't absent
        if (getSyncVal(data) != neighborState[f]) { // passed go (changed value)
          didNeighborChange = true;
        }
      }
      neighborState[f] = getSyncVal(data);  // update our record of state now that we've check it
    }
  }

  // if our neighbor passed go and we haven't done so within the buffer period, catch up and pass go as well
  if (didNeighborChange && syncTimer.getRemaining() < PERIOD_DURATION - BUFFER_DURATION) {
    syncTimer.set(PERIOD_DURATION); // aim to pass go in the defined duration
    syncVal = !syncVal; // change our value everytime we pass go
  }
  
  // if we are due to pass go, i.e. timer expired, do so
  if ( syncTimer.isExpired() ) {
    syncTimer.set(PERIOD_DURATION); // aim to pass go in the defined duration
    syncVal = !syncVal; // change our value everytime we pass go
  }
}
