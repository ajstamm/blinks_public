/*  Abigail Stamm
 *  Created 21 February 2021
 *  
 *  Aerion variant (Shipyard)
 *  
 *  12 blinks:
 *  6 ships and 6 parts (2 pilots, 2 engines, 2 chassises)
 *  
 *  For simplicity, ships are: 1p2e3c, 1p3e2c, 2p1e3c, 2p3e1c, 3p1e2c, 3p2e1c
 *  Parts can cycle randomly 5 times. After completing a ship, parts change
 *  randomly once - they can change to end of deck. 
 *  Maybe use flashing to denote last card?
 *  
 *  6-blink variant with 3 ships, 3 parts?
 *  
 *  Rethinking: naval shipyard
 *  sails, cannons, body length
 *  
 */

byte sail = random(3);
byte body = random(3);
byte cannon = random(3);
byte ship = random(6);



void setup() {
  // put your setup code here, to run once:
  randomize();
}

void loop() {
  // put your main code here, to run repeatedly:

}
