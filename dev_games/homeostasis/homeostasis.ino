/*  this is more simulation than game, 
 *  like terrarium, I think
 */

 // need to figure out what randomize() does
 // and how it works
byte sunDim = 0;
byte waterDim = 0;
byte healthDim = 150;
byte foodDim = 0;

enum resources {SUN, WATER, FOOD, INERT};
byte resource = INERT;


void setup() {
  // put your setup code here, to run once:
  /*  need 1 water 100%, 1 sun 100% 
   *  need person 50% in each blink
   *  food and 2nd person appear later
   */
  setColor(dim(WHITE, 0)); 
}

void loop() {
  // put your main code here, to run repeatedly:
  setColorOnFace(dim(WHITE, healthDim), 0);
  setColorOnFace(dim(BLUE, waterDim), 5);
  setColorOnFace(dim(GREEN, foodDim), 1);
  setColorOnFace(dim(YELLOW, sunDim), 3);


  /*  rules:
   *  sun > 50% 
   *    each neighbor with less sun gains 10%
   *    water on tile < by % of sun > 50%
   *  sun in (40-70%) and water in (30-50%)
   *    food starts to grow (up to 2 per tile)
   *    gain 10% food each second in window
   *    lose 5% food if sun/water not optimal
   *    when food > 50%
   *      health > by % food > 50%
   *      when person 100% 2nd person born
   *    when food < 50% 
   *      health < by 5% (2nd person can die)
   *  sun at 100%
   *    sunset begins: sun < 5% per second
   *    until neighbor increases it
   */
  // to assign sun/water randomly
  if (buttonSingleClicked()) {
    resource = SUN;
    sunDim = 255;
  }
  if (buttonDoubleClicked()) {
    resource = WATER;
    waterDim = 255;
  }

  FOREACH_FACE(f) {
    // is there a neighbor here?
    if (!isValueReceivedOnFaceExpired(f)) {
      // is neighbor signaling GO?
      if (getLastValueReceivedOnFace(f) == WATER) {
        waterDim = waterDim + 10;
      }
      if (getLastValueReceivedOnFace(f) == SUN) {
        sunDim = sunDim + 10;
      }
    }
  }

  // setColorOnFace(WHITE, random(5));
  if (sunDim > 220) {
    resource = SUN;
  } else if (waterDim > 200) {
    resource = WATER;
  } else {
    resource = INERT;
  }
  if (sunDim > 100 & sunDim < 200 & 
    waterDim > 100 & waterDim < 200) {
    foodDim = foodDim + 10;
    healthDim = healthDim + 5;
  }
  if (sunDim < 100 & waterDim < 100 & foodDim > 5) {
    foodDim = foodDim - 5;
  }
  if (foodDim == 0 & healthDim > 100) {
    healthDim = healthDim - 5;
  }
  if (sunDim > 200 & waterDim > 100) {
    waterDim = waterDim - 5;
  }
  if (sunDim > 200) { // sunset
    sunDim = sunDim - 10;
  }
  if (waterDim > 200) { // flood recede
    waterDim = waterDim - 10;
  }
  setColorOnFace(dim(WHITE, healthDim), 0);
  setColorOnFace(dim(BLUE, waterDim), 5);
  setColorOnFace(dim(GREEN, foodDim), 1);
  setColorOnFace(dim(YELLOW, sunDim), 3);
  setValueSentOnAllFaces(resource);
  
}
