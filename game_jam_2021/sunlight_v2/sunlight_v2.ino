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

// sun (day) time = (random(4) + 3) * 1000
// water (month) time = sun * (random(3) + 3) * (random(3) + 7) / 10
// snow (season) time = water  * (random(2) + 2) * (random(3) + 7) / 10
#define DAY_LENGTH    ((random(4) + 3) * 1000)
#define MONTH_LENGTH  (DAY_LENGTH * (random(3) + 3) * (random(3) + 7) / 10)
#define SEASON_LENGTH (MONTH_LENGTH  * (random(2) + 2) * (random(3) + 7) / 10)

// SYNCHRONIZED CELEBRATION
Timer syncTimer;
#define PERIOD_DURATION 2000
#define BUFFER_DURATION 200
byte neighborState[6];
byte syncVal = 0;

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
  sunDim = sin8_C(pulseMapped);

  // summer, then winter, then summer again ...
  pulseProgress = millis() % SEASON_LENGTH;
  pulseMapped = map(pulseProgress, 0, SEASON_LENGTH, 0, 255);
  snowSat = sin8_C(pulseMapped);

  if (!oceanID) {
    oceanDim = (oceanDim + getOceanLevels()) / 2.2;
  } else {
      pulseProgress = millis() % MONTH_LENGTH;
      pulseMapped = map(pulseProgress, 0, MONTH_LENGTH, 0, 255);
      oceanDim = sin8_C(pulseMapped);
  }
  oceanHue = map(oceanDim, 0, 31, 50, 160);
  plantHue = map(oceanDim, 0, 31, 50, 100);
   
  // run syncLoop to determine sun brightness synced across blinks
  syncLoop();
  
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

void displayLoop() {
  setColor(makeColorHSB(plantHue, 255 - snowSat, sunDim));
  setColorOnFace(makeColorHSB(flowerHue1, 255 - snowSat, sunDim), 2);
  setColorOnFace(makeColorHSB(flowerHue2, 255 - snowSat, sunDim), 4);
  setColorOnFace(makeColorHSB(flowerHue3, 255 - snowSat, sunDim), 0);
  if (snowSat > 100 & flowerHue1 < 40) {
    setColorOnFace(makeColorHSB(flowerHue1, 255, sunDim), 2);
    setColorOnFace(makeColorHSB(flowerHue2, 255, sunDim), 4);
    setColorOnFace(makeColorHSB(flowerHue3, 255, sunDim), 0);    
  }
  if (oceanID) {
    setColorOnFace(makeColorHSB(oceanHue, 255 - (255 - snowSat) / 2 , sunDim), 0);    
  }
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
  // if we are due to pass go, i.e. timer expired, do so
  if ( (didNeighborChange && syncTimer.getRemaining() < PERIOD_DURATION - BUFFER_DURATION)
       || syncTimer.isExpired()
     ) {
    syncTimer.set(PERIOD_DURATION); // aim to pass go in the defined duration
    syncVal = !syncVal; // change our value everytime we pass go
  }
}
