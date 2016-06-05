# rc-crawler-lights
An Arduino sketch for adding LED lights to an RC car.

I am aware of the Open Source Lights project which has a lot more features, but I wanted to figure out how to make this work on my own.  
I made this for use in an RC4WD Gelande chassis but it should be adaptable for any RC car.

# Required hardware
The required hardware very basic. 

I used power directly from a 2S lipo battery to power the arduino, using a BEC is also an option.

To read the throttle- and steeringposition from the RC receiver the signal wires to the ESC and steering servo can be tapped and directly wired to their pins on the Arduino board. 
Take note that this may interfere with propper operation of the receiver when the arduino is powered of.

LEDs can be connected to the output pins as usual, using current limiting resistors.
For my project I used an Arduino Nano board which has a 40mA current limit on it's digital pins which creates a limit of two LEDs per pin.
Two pins for indicators were implemented since I needed three indicators per side on my project.
