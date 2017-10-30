# Sound Voltex/K-Shoot Mania Controller Script for an Arduino Uno

![noah](https://github.com/5argon/UnoSDVXCon/raw/master/images/head.jpg)
![near](https://github.com/5argon/UnoSDVXCon/raw/master/images/splash.gif)
![laur](https://github.com/5argon/UnoSDVXCon/raw/master/images/laurtrim.gif)

** NOTE ** The knob function as a rapid key press as you can see from the gif, so this means it is not suitable for the official e-amusement cloud game as the game is expecting a button hold for knobs unlike K-Shoot. See [this issue](https://github.com/5argon/UnoSDVXCon/issues/2) for details.

If you build your own controller for the game called [K-Shoot Mania](http://kshoot.client.jp) you will need some way to connect it to your computer. In the case that you want to use an Arduino Uno as a USB interface, this script (.ino file) will provides a working logic from the controller's input to emulate your computer's keyboard + button light feedback.

Previous alternatives are connecting buttons to disassembled [Joystick circuit](http://homingpuyo.blog91.fc2.com/blog-entry-650.html), or using an [Arduino Leonardo](http://sdvxii.pancakeapps.com/LEONARDO). Arduino Uno cannot interface as a keyboard as easily as Leonardo. Only [Leonardo, Micro, and Due](https://www.arduino.cc/en/Reference/MouseKeyboard) can use the built-in keyboard functions.

## Video demo

The controller with Arduino Uno in this video uses this script. The knob miss probably results from my skill, not the script. (My rank is 9 in SDVX, I could not play perfectly.)

[cry of viyella - Laur](https://www.youtube.com/watch?v=efWP1qvjdQ0)

[Fall in the Vortex - LAriA](https://www.youtube.com/watch?v=rYstLXN4oDg)

## Features

- Plug into your computer as a USB keyboard. (HID interface standard) sdksldksldkdlksdlkdlskdlsddsskdkdkkkkklslskdlsdqooooppppoowowowwoqqqqqpqwwwwwqppqpqpqowowoow
- Supports LED response on all 4 BT + 2 FX + START button.
- Supports encoders with 2 output signals cable (quadrature encoded), e.g. Copal RES20D50-201-1
- Change Uno's input pin easily in the code to suit your project.
- Adjust corresponding keys in the code.
- Adjust sensitivity of knobs and buttons in the code.

## Requires

- A [firmware flash hack](http://mitchtech.net/arduino-usb-hid-keyboard/) that will make your Arduino HID compatible but loses ability to program. If you want to reupload the program again you need to flash the firmware back. (Flashing this firmware cannot brick your Uno.)

## How to use

- Make a controller around an Arduino Uno.
- Look at the SDVXCon.ino's top part and config things according to your connections to Uno's pin. Please read the comments before deciding which pin for each component, as the knobs requires specific pin. (Important)
- Upload the code to your Uno with the [Arduino IDE](https://www.arduino.cc/en/Main/Software).
- Do the [firmware flash hack](http://mitchtech.net/arduino-usb-hid-keyboard/). In that link, you will need a jumper, a cable or something to short circuit 2 pins on the Uno to begin flashing.
- Reconnect and done!

## Contact

You can post an issue here or DM me on Twitter. ([@5argondesu](https://twitter.com/5argondesu))
