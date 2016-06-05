# RC Crawler Lights
An Arduino sketch for adding LED lights to an RC car.  
Ch1 and Ch2 are connected to the Arduino to allow the lights to follow what the car is doing.  
Light features included:  
* Running lights, always on.
* Headlights, these turn on on first throttle input.
* Blinking indicators for the left and right side, these follow the steering input. When no signal from the receiver is found the hazard lights will blink.
* Tail- and brake lights. Taillights are always on and will turn brighter when no throttle input is given.
* Brake light, on when no throttle input is given. Can be used for a third brake light.
* Reverse light, on when driving in reverse.

I am aware of the Open Source Lights project which has a lot more features, but I wanted to figure out how to make this work on my own.  
I made this for use in an RC4WD Gelande chassis but it should be adaptable for any RC car.

# Hardware
The required hardware very basic. 

I used power directly from a 2S lipo battery to power the arduino, using a BEC is also an option.

To read the throttle- and steeringposition from the RC receiver the signal wires to the ESC and steering servo can be tapped and directly wired to their pins on the Arduino board. 
Take note that this may interfere with propper operation of the receiver when the arduino is powered of.

LEDs can be connected to the output pins using current limiting resistors, as usual on Arduino projects.  
For my project I used an Arduino Nano board which has a 40mA current limit on it's digital pins which creates a limit of two LEDs per pin.  
Two pins for indicators were implemented since I needed three indicators per side on my project.
