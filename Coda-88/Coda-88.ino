/*
 
    Coda 88 is a MIDI sound module with three layer 
    of dynamics for full 88 keys master keyboards
 
    https://github.com/sandrolab/coda-88

    Author: 
    Sandro L'Abbate
    https://sandrolabbate.com

    Coding support:
    Alfredo Ardia 
    https://alfredoardia.com

    WAV Trigger:
    https://robertsonics.com/wav-trigger/

    WIRING:
    
    MIDI          Uno
    ====          ===
    GND  <------> GND
    5V   <------> 5V
    RX   <------> RX 

    Uno           WAV Trigger
    ===           ===========
    GND  <------> GND
    Pin9 <------> RX

    If you want to power the WAV Trigger from the Uno, then close the 5V
    solder jumper on the WAV Trigger and connect a 3rd wire:

    5V   <------> 5V
    
    SAMPLES NUMERATION (ex. 032.wav):
    020 Start sound
    021 -> 108 Piano
    109 -> 196 Medio (+88)
    197 -> 284 Forte (+176)
    285 -> 372 Release (+264)
    373 Damper Down
    374 Damper Up

    IMPORTANT: 
    1) Disconnect RX (MIDI <---> Uno) before upload
    2) Press reset button on the Arduino after every boot or strange beahavior

    
*/

#include <MIDI.h>
#include <Metro.h>
#include <AltSoftSerial.h>
#include <wavTrigger.h>

// SAMPLES POSITION
#define layerMedio 88
#define layerForte 176
#define layerRelease 264
#define firstRel 285
#define lastRel 372
#define pedalDown 373
#define pedalUp 374
// Dampers range (90 = G6)
#define dampEnd 90 

// VELOCITY
#define velMinPiano 0
#define velMaxPiano 75
#define velMinMedio 75
#define velMaxMedio 110
#define velMinForte 110
#define velMaxForte 127

// GAIN
// -70 to +4
#define gainMaster 4 
// -70 to +10
#define gainMinPiano -50 
#define gainMaxPiano -20
#define gainMinMedio -20
#define gainMaxMedio -5
#define gainMinForte -5
#define gainMaxForte 0
#define gainStartSound -20
#define gainRelease -55
#define gainDamperUp -30
#define gainDamperDown -20

#define AfterPedalLength 16

// Ser 'true' for dumper pedal or release-key sounds
bool RELSOUND = false; 
bool PEDALSOUND = false; 

byte DamperState;
int AfterPedal[AfterPedalLength];
int KeyCount = 0;

MIDI_CREATE_DEFAULT_INSTANCE();
wavTrigger wTrig; // Our WAV Trigger object

void setup() {
  
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(KeyOn);
  MIDI.setHandleNoteOff(KeyOff);
  MIDI.setHandleControlChange(Sustain);
  
  wTrig.start(); // WAV Trigger startup at 57600
  wTrig.masterGain(gainMaster);
  wTrig.trackGain(pedalDown, gainDamperDown);
  wTrig.trackGain(pedalUp, gainDamperUp);
  wTrig.trackGain(20, gainStartSound);
  
  for (int i = firstRel; i <= lastRel; i++) { 
    wTrig.trackGain(i, gainRelease); 
  }
  
  delay(500);
  wTrig.trackPlaySolo(20); // Ready

}

void KeyOn (byte ch, byte note, byte vel) {

  int gain;

  // Piano
  if (vel < velMaxPiano) {
    gain = map(vel, velMinPiano, velMaxPiano, gainMinPiano, gainMaxPiano);
    wTrig.trackGain(note, gain);
    wTrig.trackPlayPoly(note);
  }

  // Medio
  else if (vel >= velMinMedio && vel < velMaxMedio) {
    gain = map(vel, velMinMedio, velMaxMedio, gainMinMedio, gainMaxMedio);
    wTrig.trackGain((note + layerMedio), gain);
    wTrig.trackPlayPoly(note + layerMedio);
  }

  // Forte
  else  {
    gain = map(vel, velMinForte, velMaxForte, gainMinForte, gainMaxForte);
    wTrig.trackGain((note + layerForte), gain);
    wTrig.trackPlayPoly(note + layerForte);
  }

  // Tenuto
  for (int i = 0; i < AfterPedalLength; i++) {
    if (AfterPedal[i] == note) AfterPedal[i] = 0; 
  }

}

void KeyOff (byte ch, byte noteoff, byte vel) {

  if (RELSOUND) wTrig.trackPlayPoly(noteoff + layerRelease);
  
  if (noteoff <= dampEnd) { 
    switch (DamperState) {
      case 0: 
        wTrig.trackStop(noteoff); 
        wTrig.trackStop(noteoff + layerMedio); 
        wTrig.trackStop(noteoff + layerForte); 
        break;
      case 127:
        AfterPedal[KeyCount++] = noteoff;
        if (KeyCount == (AfterPedalLength - 1)) KeyCount = 0;
        break; 
    } 
  }

}

void Sustain (byte ch, byte num, byte val) {

  DamperState = val;

  if (PEDALSOUND) {
    wTrig.trackPlayPoly(pedalUp); 
    wTrig.trackStop(pedalDown);
  }
  
  for (int i = 0; i < AfterPedalLength; i++) {
    wTrig.trackStop(AfterPedal[i]);
    wTrig.trackStop(AfterPedal[i] + layerMedio);
    wTrig.trackStop(AfterPedal[i] + layerForte);
  }

  if (DamperState == 127) {
    if (PEDALSOUND) wTrig.trackPlayPoly(pedalDown);
    KeyCount = 0;
    for (int i = 0; i < AfterPedalLength; i++) {
      AfterPedal[i] = 0; 
    }
  }

}

void loop() { 
  
  MIDI.read(); 
  
}
