# Tilting Ball Game

## CICS 256 Project


Use the readme as a place for brainstorming and just jotting down ideas rn. Make edits, commit straight. 

Can be done straight from browser. Let's try to use markdown features to organize as well. 

This will be archived and replaced with an actual ReadMe with all presentable details once project is done. 

Can make basic website via GitHub Pages as well, but let's not think about that rn. 

## Project Pitch

<a href="https://github.com/suobset/256BallTilt/blob/main/assets/CICS156%20Final%20Project%20Presentation.pdf">Link to project Pitch (uploaded to repository)</a>

<a href="https://sites.google.com/umass.edu/rollgame/home">Link to Project Website with Updates</a>

### Timeline:

* Week 1: Ordering components, brainstorming/planning as much as possible before they arrive
* Week 2: Attaching components, getting familiar with tilt sensor/using LED display
* Week 3: Coding basic ball movement with button input, possibly start using tilt sensor
* Week 4: Tilt sensor ball movement finalized
* Week 5: Adding additional features (Winning a level, failing a level, obstacles)
* Week 6: Tweaking, bug fixing, play testing, polishing

### components

* Neopixel LED display (32x32)
* 4 buttons to test ball moving input
* Tilt sensor
* Our makerboards (or potentially an Arduino Uno or Raspberry Pi, if computation/latency errors occur)

## Installation 

* Arduino Zero Documentation: https://docs.arduino.cc/hardware/zero
* Get started with Arduino Zero: https://www.arduino.cc/en/Guide/ArduinoZero
* Test ```BlinkExample``` under examples 
* Connect Acclerometer with Arduino Zero
* Test ```accl_example``` under examples
* Connecting Neopixel with Arduino Zero: https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/connecting-with-jumper-wires
* Use ```3v3``` or ```VBUS``` for red, ```GND``` for black. Find an EXACT 5V port. 
* Install RGB Matrix, AdaFruit GFZ, and others as listed in above link
* Test ```neoPixel_32_example``` under examples