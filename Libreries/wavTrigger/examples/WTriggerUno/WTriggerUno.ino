// ****************************************************************************
//       Sketch: WTriggerUno
// Date Created: 4/22/2015
//
//     Comments: Demonstrates basic serial control of the WAV Trigger from an
//               Arduino UNO.
//
//  Programmers: Jamie Robertson, jamie@robertsonics.com
//
// ****************************************************************************
//
// To use this sketch, you'll need to:
//
// 1) Download and install the AltSoftSerial library.
// 2) Download and install the Metro library.
// 3) Connect 2 wires from the UNO to the WAV Trigger's serial connector:
//
//    Uno           WAV Trigger
//    ===           ===========
//    GND  <------> GND
//    Pin9 <------> RX
//
//    If you want to power the WAV Trigger from the Uno, then close the 5V
//    solder jumper on the WAV Trigger and connect a 3rd wire:
//
//    5V   <------> 5V
//
// 4) Download and install the demo wav files onto the WAV Trigger's microSD
//    card. You can find them here:
//
//
//    You can certainly use your own tracks instead, although the demo may
//    not make as much sense. If you do, make sure your tracks are at least 10
//    to 20 seconds long and have no silence at the start of the file.

#include <Metro.h>
#include <AltSoftSerial.h>    // Arduino build environment requires this
#include <wavTrigger.h>

#define LED 13                // our LED

wavTrigger wTrig;             // Our WAV Trigger object

Metro gLedMetro(500);         // LED blink interval timer
Metro gWTrigMetro(6000);      // WAV Trigger state machine interval timer

byte gLedState = 0;           // LED State
int  gWTrigState = 0;         // WAV Trigger state
int  gRateOffset = 0;         // WAV Trigger sample-rate offset


// ****************************************************************************
void setup() {
  
  // Serial monitor
  Serial.begin(9600);
 
  // Initialize the LED pin
  pinMode(LED,OUTPUT);
  digitalWrite(LED,gLedState);

  // WAV Trigger startup at 57600
  wTrig.start();
  
  // If the Uno is powering the WAV Trigger, we should wait for the WAV Trigger
  //  to finish reset before trying to send commands.
  delay(1000);
  
  // If we're not powering the WAV Trigger, send a stop-all command in case it
  //  was already playing tracks. If we are powering the WAV Trigger, it doesn't
  //  hurt to do this.
  wTrig.stopAllTracks();
  
}


// ****************************************************************************
// This program uses a Metro timer to create a state machine that steps
//  through states at 6 second intervals - you can change this rate above.
//  Each state will execute a new wavTrigger command.

void loop() {
  
int i;

  // If time to do so, perform the next WAV Trigger task and then increment
  //  the state machine variable 
  if (gWTrigMetro.check() == 1) {
      
      switch (gWTrigState) {
  
          // Demonstrates how to fade in a music track 
          case 0:
              wTrig.masterGain(0);                  // Reset the master gain to 0dB                 
              wTrig.trackGain(2, -40);              // Preset Track 2 gain to -40dB
              wTrig.trackPlayPoly(2);               // Start Track 2
              wTrig.trackFade(2, 0, 2000, 0);       // Fade Track 2 to 0dB
          break;

          // Demonstrates how to cross-fade music tracks
          case 1:
              wTrig.trackCrossFade(2, 1, 0, 3000);  // Cross-fade Track 2 to Track 1
          break;
                                 
          // Fade down music and start looping dialog
          case 2:
              wTrig.trackFade(1, -6, 500, 0);
              wTrig.trackPlayPoly(4);               // Start Track 4 poly
              wTrig.trackLoop(4, 1);                // Enable Track 4 looping
          break;

          // Honk the horn 2 times
          case 3:
              wTrig.trackPlayPoly(5);               // Start Track 5 poly
              delay(500);
              wTrig.trackStop(5);                   // Stop Track 5
              delay(250);
              wTrig.trackPlayPoly(5);               // Start Track 5 poly
              delay(500);
              wTrig.trackStop(5);                   // Stop Track 5
          break;
          
          // Fade out and stop dialog
          case 4:
              wTrig.trackLoop(4, 0);                // Disable Track 4 looping
              wTrig.trackFade(4, -50, 5000, 1);     // Fade Track 4 to -50dB and stop
          break;
          
          // This demonstrates playing musical instrument samples, with decay on
          //  release
          case 5:
              wTrig.masterGain(-8);                 // Lower main volume
              wTrig.trackPlayPoly(6);               // Play first note
              delay(1000);
              wTrig.trackPlayPoly(7);               // Play second note
              delay(1000);
              wTrig.trackPlayPoly(8);               // Play third note
              delay(1000);
              wTrig.trackFade(6, -50, 5000, 1);     // Fade Track 6 to -50dB and stop
              wTrig.trackFade(7, -50, 5000, 1);     // Fade Track 7 to -50dB and stop
              wTrig.trackFade(8, -50, 5000, 1);     // Fade Track 8 to -50dB and stop
          break;
   
          // Demonstrates preloading tracks and starting them in sample-sync, and
          //  real-time samplerate control (pitch bending);
          case 6:
              wTrig.trackLoad(6);                   // Load and pause Track 6
              wTrig.trackLoad(7);                   // Load and pause Track 7
              wTrig.trackLoad(8);                   // Load and pause Track 8
              wTrig.resumeAllInSync();              // Start all in sample sync
              for (i = 0; i < 100; i++) {
                gRateOffset -= 200;
                wTrig.samplerateOffset(gRateOffset);
                delay(10);
              }
              for (i = 0; i < 100; i++) {
                gRateOffset += 200;
                wTrig.samplerateOffset(gRateOffset);
                delay(10);
              }
              delay(500);
              wTrig.stopAllTracks();                // Stop all
         break;
         
           
      } // switch
 
      // Increment our state
      gWTrigState++;
      if (gWTrigState > 6)
          gWTrigState = 0;
          
  } // if (gWTrigState.check() == 1)
 
  // If time to do so, toggle the LED
  if (gLedMetro.check() == 1) {
      if (gLedState == 0) gLedState = 1;
      else gLedState = 0;
      digitalWrite(LED, gLedState);
  } // if (gLedMetro.check() == 1)

  // Delay 30 msecs
  delay(30);
}

