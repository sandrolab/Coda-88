# Coda-88

Minimal piano sound module/expander with dynamics based on Arduino and WAV Trigger

![](https://www.sandrolabbate.com/assets/2018_coda_close.jpg)
<br>
*© 2018 [Sandro L'Abbate](https://www.sandrolabbate.com "Sandro L'Abbate")*

## Features
- 88-key keyboard compatible
- MIDI connection
- Zero boot time
- Low latency 
- Polyphony (14-stereo)
- Realistic dynamics (3 sampling layers)
- Dumper and release-key effects

## Parts
- Arduino UNO
- WAV Trigger
- 6N138
- Diode 1N914
- Resistor 220 Ohm (x2) 
- Resistor 4.7K
- Switch (optional - you need to disconnect RX before upload)

## Schema
![Coda-88_Schema](https://github.com/sandrolab/Coda-88/blob/1e316c8b399025062a1382ba375b062ecdef1a1b/Schema.jpg "Coda-88_Schema")<br>
*WAV Trigger SVG from [sjpiper145](https://github.com/sjpiper145/Sparkfun_wavTrigger_SVG "sjpiper145")*

## Arduino

### 1) Libraries
- AltSoftSerial
- Metro
- MIDI
- wavTrigger

### 2) Dumper pedal and release-key sounds
Set `true` these two variables (leave `false` for a better performance):
```cpp
bool RELSOUND = true;
bool PEDALSOUND = true;
```
### 3) Upload
Disconnect (or use switch button) RX connection from MIDI to Arduino before upload

### 4) Boot/reset
Press reset button on the Arduino after every boot (or strange behavior)

## WAV Trigger

### Samples numeration (ex. 032.wav)
| Samples | Start | End |
| ------------ | ------------ | ------------ |
| Init sound | `020`  |   |
| Piano  | `021 `  | `108`  |
| Medio | `109`  | `196`  |
| Forte  | `197`  | `284`  |
| Release keys  | `285`  | `372` |
| Pedal down  | `373`  |   |
| Pedal up | `374`  |   |

### Piano Samples
[Download](https://bit.ly/Coda88_PianoSamples "Download") this folder and copy its content into the WAV Trigger microSD.

These sample kit comes from [Iowa Piano](https://theremin.music.uiowa.edu/MISpiano.html "Iowa Piano") (main piano) and [Salamander Piano](https://freepats.zenvoid.org/Piano/acoustic-grand-piano.html "Salamander Piano") (only for dumper and release sounds).
Both libraries are free licensed.

See [here](https://robertsonics.com/microsd-cards-for-audio/ "here") for the best microSD cards for audio.

### Official Library
https://github.com/robertsonics/WAV-Trigger-Arduino-Serial-Library

------------

### [Video demonstration](https://youtu.be/3IshDsRlHGw "Video demonstration")

### [Coffee?](https://ko-fi.com/sandrolab "Coffee?")
