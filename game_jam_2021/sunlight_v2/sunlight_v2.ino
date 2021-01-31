/*  this is more simulation than game
 */

byte sunDim = 25; // sun brightness
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
#define PERIOD_DURATION 6000
// #define PERIOD_DURATION ((random(2) + 4) * 1000) // throws errors
#define BUFFER_DURATION 100
byte neighborState[6];
byte syncVal = 0;
#define DAY_LENGTH    6000 // ((random(2) + 4) * 1000)
#define MONTH_LENGTH  (DAY_LENGTH * (random(1) + 2) * (random(3) + 7) / 10)
#define SEASON_LENGTH (MONTH_LENGTH  * 3 * (random(3) + 7) / 10)

// store dimness level 32 water
// byte sendData = (sunDim << 5) + (oceanDim);
// << 5 means push data 5 digits left before concatenating
// & 31 gives the last 5 digits


// have a pulse wave that is a fact of life
// grass dims/grows by itself, with single bit rising/setting
// have 1 pip for sun for debugging
// tie water to be dependent on sun?


void setup() {
}

void loop() {
  if (buttonSingleClicked()) {
    oceanID = true;
    oceanDim = 31;
  }
  // sun rising, then setting, then rising again ...
  int pulseProgress = millis() % DAY_LENGTH;
  byte pulseMapped = map(pulseProgress, 0, DAY_LENGTH, 0, 255);
  sunDim = map(sin8_C(pulseMapped), 0, 255, 100, 255);

  // summer, then winter, then summer again ...
  pulseProgress = millis() % SEASON_LENGTH;
  pulseMapped = map(pulseProgress, 0, SEASON_LENGTH, 0, 255);
  snowSat = sin8_C(pulseMapped);

  if (!oceanID) {
    oceanDim = (oceanDim + getOceanLevels()) / 2.1;
  } else {
      pulseProgress = millis() % MONTH_LENGTH;
      pulseMapped = map(pulseProgress, 0, MONTH_LENGTH, 0, 255);
      oceanDim = sin8_C(pulseMapped);
  }
  oceanHue = map(oceanDim, 0, 31, 50, 160);
  plantHue = map(oceanDim, 0, 31, 50, 100);
   
  // run syncLoop to determine sun brightness synced across blinks
  syncLoop();
//  PERIOD_DURATION = DAY_LENGTH;
  
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      setValueSentOnFace((syncVal << 5) + oceanDim, f);
    }
  }
  
  if (sunDim > 200 & plantHue > 75 & flowerDim < 250) {
    flowerDim = flowerDim + 5;
  }
  if (sunDim < 150 & flowerDim > 0) {
    flowerDim = flowerDim - 5;
  }
  // tie flowers to plants
  flowerHue1  = plantHue - map(flowerDim, 0, 255, 0, plantHue);
  flowerHue2  = plantHue - map(flowerDim, 0, 255, 18, plantHue);
  flowerHue3  = plantHue - map(flowerDim, 0, 255, 36, plantHue);
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

void displayLoopTemp() {
//  setColor(makeColorHSB(plantHue, 255 - snowSat, sunDim));
  if (oceanID) {
    setColorOnFace(dim(makeColorHSB(oceanHue, 255 - (255 - snowSat) / 2, 255), sunDim), 0);    
  } else {
    setColorOnFace(dim(makeColorHSB(flowerHue3, 255 - snowSat, 255), sunDim), 0);
  }
  setColorOnFace(dim(makeColorHSB(plantHue, 255 - snowSat, 255), sunDim), 1);
  if (snowSat > 100 & flowerHue1 < 40) {
    setColorOnFace(dim(makeColorHSB(flowerHue2, 155, 255), sunDim), 2);
  } else {
    setColorOnFace(dim(makeColorHSB(flowerHue1, 255 - snowSat, 255), sunDim), 2);
  }  
  setColorOnFace(dim(makeColorHSB(plantHue, 255 - snowSat, 255), sunDim), 3);
  setColorOnFace(dim(makeColorHSB(flowerHue2, 255 - snowSat, 255), sunDim), 4);
  setColorOnFace(dim(makeColorHSB(plantHue, 255 - snowSat, 255), sunDim), 5);
}

void displayLoop() {
//  setColor(makeColorHSB(plantHue, 255 - snowSat, sunDim));
  if (oceanID) {
    setColorOnFace(makeColorHSB(oceanHue, 255 - (255 - snowSat) / 2, 255), 0);    
  } else {
    setColorOnFace(makeColorHSB(flowerHue3, 255 - snowSat, 255), 0);
  }
  setColorOnFace(makeColorHSB(plantHue, 255 - snowSat, 255), 1);
  if (snowSat > 100 & flowerHue1 < 40) {
    setColorOnFace(makeColorHSB(flowerHue2, 155, 255), 2);
  } else {
    setColorOnFace(makeColorHSB(flowerHue1, 255 - snowSat, 255), 2);
  }  
  setColorOnFace(makeColorHSB(plantHue, 255 - snowSat, 255), 3);
  setColorOnFace(makeColorHSB(flowerHue2, 255 - snowSat, 255), 4);
  setColorOnFace(makeColorHSB(plantHue, 255 - snowSat, 255), 5);
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
//    DAY_LENGTH = PERIOD_DURATION - BUFFER_DURATION;
    syncTimer.set(DAY_LENGTH); // aim to pass go in the defined duration
    syncVal = !syncVal; // change our value everytime we pass go
  }
  
  // if we are due to pass go, i.e. timer expired, do so
  if ( syncTimer.isExpired() ) {
    syncTimer.set(DAY_LENGTH); // aim to pass go in the defined duration
    syncVal = !syncVal; // change our value everytime we pass go
  }
}
