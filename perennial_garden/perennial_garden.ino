/*   Abigail Stamm
 *   Updated 2 February 2021
 *   
 *   This is more simulation than game. To play, load the game onto any number of blinks. 
 *   When the program is done loading, click one blink to designate it a water source and 
 *   wait as flowers begin to grow. If you want more flowers, add more water sources.
 *   
 *   Both the water table and the seasons fluctuate on loops of up to 16 seconds, so be 
 *   patient before watering more than 1/6 of your tiles.
 *   
 *   Revised to allow you to turn off water tiles.
 */

byte sunDim = 255; // actual sun brightness 
byte waterDim = 0; // ocean brightness - for water tiles
byte isWater = false; // is a water tile
byte waterHue = 50; // shade of water
byte plantHue = 50; // shade of plants
byte flowerDim = 0; // shade modifier for flowers 1-3
byte flowerHue1 = 50;
byte flowerHue3 = 50;
byte flowerHue2 = 50;
byte snowSat = 0; // whiteness of tile

// synchronization values, from puzzle 101
Timer syncTimer;
#define PERIOD_DURATION 8000
#define BUFFER_DURATION 400
byte neighborState[6];
byte syncVal = 0;
int dayLength = 8000; // (random(2) + 4) * 1000;
int monthLength = dayLength * 2 * (random(3) + 7) / 10;
int seasonLength = monthLength * 2 * (random(3) + 7) / 10;

void setup() {
}

void loop() {
  if (buttonSingleClicked()) { // define water tile
    isWater = !isWater;
    if (isWater) {
      waterDim = 31;
    } else {
      waterDim = 0;
    }
  }
  // sun rising, then setting, then rising again ...
  int pulseProgress = millis() % dayLength;
  byte pulseMapped = map(pulseProgress, 0, dayLength, 0, 255);
  sunDim = map(sin8_C(pulseMapped), 0, 255, 100, 255); // sunset

  // summer, then winter, then summer again ...
  pulseProgress = millis() % seasonLength;
  pulseMapped = map(pulseProgress, 0, seasonLength, 0, 255);
  snowSat = map(sin8_C(pulseMapped), 0, 255, 0, 25);

  if (!isWater) { // water level in adjacent tiles
    waterDim = (waterDim + getWaterLevels()) / 2.1;
  } else {
      pulseProgress = millis() % monthLength;
      pulseMapped = map(pulseProgress, 0, monthLength, 0, 31); // force into 0-31 range
      waterDim = sin8_C(pulseMapped);
  }
  waterHue = map(waterDim, 0, 31, 100, 160);
  plantHue = map(waterDim, 0, 31, 50, 100);
   
  // run syncLoop to determine sunlight synced across blinks
  syncLoop();
  
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      setValueSentOnFace((syncVal << 5) + waterDim, f);
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

// check and average neighbors' water levels
byte getWaterLevels() {
  byte neighbor = 0;
  byte denom = 0;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) { // is there a neighbor?
      neighbor = neighbor + getWater(getLastValueReceivedOnFace(f));
      denom = denom + 1;
    }
  }
  neighbor = neighbor / denom;
  return neighbor;
}

byte getWater(byte data) {
    return (data & 31); // returns bits 2 to 6
}

void displayLoop() {
  if (isWater) {
    setColorOnFace(makeColorHSB(waterHue, 255 - (255 + (snowSat * 10)) / 2, sunDim), 0);    
  } else {
    setColorOnFace(makeColorHSB(flowerHue3, 255 - (snowSat * 10), sunDim), 0);
  }
  setColorOnFace(makeColorHSB(plantHue, 255 - (snowSat * 10), sunDim), 1);
  if (snowSat > 4 & flowerHue1 < 4) {
    setColorOnFace(dim(makeColorHSB(flowerHue2, 155, 255), sunDim), 2);
  } else {
    setColorOnFace(dim(makeColorHSB(flowerHue1, 255 - (snowSat * 10), 255), sunDim), 2);
  }  
  setColorOnFace(dim(makeColorHSB(plantHue, 255 - (snowSat * 10), 255), sunDim), 3);
  setColorOnFace(dim(makeColorHSB(flowerHue2, 255 - (snowSat * 10), 255), sunDim), 4);
  setColorOnFace(dim(makeColorHSB(plantHue, 255 - (snowSat * 10), 255), sunDim), 5);
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
  // if we are due to pass go, i.e. timer expired, do so
  if ((didNeighborChange && syncTimer.getRemaining() < PERIOD_DURATION - BUFFER_DURATION) ||
      syncTimer.isExpired()) {
    syncTimer.set(PERIOD_DURATION); // aim to pass go in the defined duration
    syncVal = !syncVal; // change our value everytime we pass go
  }
}
