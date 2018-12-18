#include"Arduino.h"
#include "notedata.h"

// Floppy drive head in forward direction
#define HEADF 0
// Floppy drive head in reverse direction
#define HEADR 1

#define MYBUTTON 2
#define MYLED 13
#define MYSTEP 3
#define MYDIR 4

void reset_pos(char step_pin, char dir_pin, bool rev = true);
void play_note(char step_pin, char dir_pin, int freq, int dur);
void auto_step(char step_pin, char dir_pin);

class Note
{
  public:
    Note(int frq, int len) : nfrq(frq), nlen(len) {}
    int nfrq = 100; // Note frequency in Hz
    int nlen = 100; // Note length in ms
};

const int cenalen = 11;
Note cenalow[cenalen] = {Note(NOTE_G2, EIGHT_NOTE), Note(NOTE_AS2, SIXTEENTH_NOTE), Note(NOTE_F2, SIXTEENTH_NOTE), Note(REST, SIXTEENTH_NOTE), Note(NOTE_G2, HALF_NOTE + EIGHT_NOTE),
    Note(REST, SIXTEENTH_NOTE), Note(NOTE_AS2, EIGHT_NOTE), Note(NOTE_A2, SIXTEENTH_NOTE), Note(NOTE_F2, SIXTEENTH_NOTE), Note(REST, SIXTEENTH_NOTE), Note(NOTE_G2, HALF_NOTE+SIXTEENTH_NOTE)};

Note cenamed[cenalen] = {Note(NOTE_G3, EIGHT_NOTE), Note(NOTE_AS3, SIXTEENTH_NOTE), Note(NOTE_F3, SIXTEENTH_NOTE), Note(REST, SIXTEENTH_NOTE), Note(NOTE_G3, HALF_NOTE + EIGHT_NOTE),
    Note(REST, SIXTEENTH_NOTE), Note(NOTE_AS3, EIGHT_NOTE), Note(NOTE_A3, SIXTEENTH_NOTE), Note(NOTE_F3, SIXTEENTH_NOTE), Note(REST, SIXTEENTH_NOTE), Note(NOTE_G3, HALF_NOTE+SIXTEENTH_NOTE)};

Note cenahigh[cenalen] = {Note(NOTE_G4, EIGHT_NOTE), Note(NOTE_AS4, SIXTEENTH_NOTE), Note(NOTE_F4, SIXTEENTH_NOTE), Note(REST, SIXTEENTH_NOTE), Note(NOTE_G4, HALF_NOTE + EIGHT_NOTE),
    Note(REST, SIXTEENTH_NOTE), Note(NOTE_AS4, EIGHT_NOTE), Note(NOTE_A4, SIXTEENTH_NOTE), Note(NOTE_F4, SIXTEENTH_NOTE), Note(REST, SIXTEENTH_NOTE), Note(NOTE_G4, HALF_NOTE+SIXTEENTH_NOTE)};

/*
   Current position of the read/write head, floppy drive generally has
   80 total steps of travel
*/
char position = 79;
/*
   Current direction of the head.  Needed to keep head moving in the same direction until it hits the end of its travel
*/
bool dir = HEADF;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(MYBUTTON, INPUT);
  pinMode(MYLED, OUTPUT);
  pinMode(MYSTEP, OUTPUT);
  pinMode(MYDIR, OUTPUT);

  reset_pos(MYSTEP, MYDIR, HEADR);

  delay(1000);

  for (int i = 0; i < cenalen; i++)
  {
    play_note(MYSTEP, MYDIR, cenalow[i].nfrq, cenalow[i].nlen);
  }
}

void loop() {
//  while(true)
//  {
//    Serial.println((digitalRead(MYBUTTON) ? "YES":"NO"));
//    if(digitalRead(MYBUTTON))
//    {
//      for (int i = 0; i < alen; i++)
//      {
//        play_note(MYSTEP, MYDIR, test[i].nfrq, test[i].nlen);
//      }
//    }
//  }
}

/*
   Bring head back to it's default position
   step_pin - pin number for floppy drive step
   dir_pin - pin number for floppy drive direction
   rev - go to the end of the travel instead of home
*/
void reset_pos(char step_pin, char dir_pin, bool dir)
{
  char waitTime = 5;
  if (dir == HEADF)
  {
    digitalWrite(dir_pin, LOW);
    for (char i = 0; i < 80; i++)
    {
      digitalWrite(step_pin, HIGH);
      delay(waitTime);
      digitalWrite(step_pin, LOW);
      delay(waitTime);
    }
    digitalWrite(dir_pin, HIGH);
    position = 79;
  } else
  {
    digitalWrite(dir_pin, HIGH);
    for (char i = 0; i < 80; i++)
    {
      digitalWrite(step_pin, HIGH);
      delay(waitTime);
      digitalWrite(step_pin, LOW);
      delay(waitTime);
    }
    digitalWrite(dir_pin, LOW);
    position = 0;
  }
}

//TODO: fix
void auto_step(char step_pin, char dir_pin)
{
  if (position == 0 && dir == HEADR) // Head at home and reversing
  {
    // Start going forward
    dir = HEADF;
    digitalWrite(dir_pin, LOW);
  } else if (position == 79 && dir == HEADF) // Head at end, going forward
  {
    // Start going backward
    dir = HEADR;
    digitalWrite(dir_pin, HIGH);
  }
  // Perform the actual step
  digitalWrite(step_pin, HIGH);
  digitalWrite(step_pin, LOW);
  // Increment/decrement position accordingly
  position += (dir ? -1 : 1);
}

void play_note(char step_pin, char dir_pin, int freq, int dur)
{
  if(freq == 0)
  {
    delay(dur);
    return;
  }
  unsigned long int period = 1000000 / freq;
  unsigned long int dur_micro = (long)dur * (long)(1000);
  unsigned long int num_steps = dur_micro / period;
  int remainder = (dur * 1000) % period;
  
  for (int i = 0; i < num_steps; i++)
  {
    auto_step(step_pin, dir_pin);
    delayMicroseconds(period);
  }
  delayMicroseconds(remainder);
}




/*
   This program refers to forward motion of the read/write head to be in the
   direction towards the center of the disc, and reverse motion to be towards
   the outside of the disc.  Home, beginning, etc. is at the maximum point of
   travel in the reverse direction.  Likewise, end is the maximum point of
   travel in the forward direction.
*/
