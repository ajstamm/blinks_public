/*  Abigail Stamm, 28 March 2021
 *  Game based on Eiyo by Thundergryph Games and
 *  Pans Labyrinth by Guardian of Fun
 */

byte score = 0; // minimum 0 to maximum 56 (max final 52)
byte soldier = random(8); // 3 possible types
  // 1-4 = foot (cyan); 5 = cavalier (magenta); 6-8 = archer (green)
Color soldierColors = {OFF, CYAN, MAGENTA, GREEN}
byte faceColors = {0, 0, 0, 0, 0, 0}
byte faceBrightness = {70, 70, 70, 70, 70, 70}
byte enemy = true;

void setup() {
  // put your setup code here, to run once:
  randomize();  
  newArmy();
  shuffleSoldiers(enemy);
}

void loop() {
  // put your main code here, to run repeatedly:
  

}

void shuffleSoldiers() {
  for (byte i = 0; i < 6; i++) {
    soldier = random(8);
    if (soldier < 5)  {
      faceColor[i+1] = colorSoldier[1];
    } else if (soldier < 6) {
      faceColor[i+1] = colorSoldier[2];
    } else {
      faceColor[i+1] = colorSoldier[3];
    }
  }
  if (!enemy) {
    faceColor[6] = OFF;
  }
}
