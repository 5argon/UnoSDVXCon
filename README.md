#Sound Voltex Controller Script for Arduino Uno

This is for building your own physical controller for the game called [K-Shoot Mania](http://kshoot.client.jp) with an Arduino Uno as a USB interface.

Previous alternatives are connecting buttons to disassembled [Joystick circuit](http://homingpuyo.blog91.fc2.com/blog-entry-650.html), or using an [Arduino Leonardo](http://sdvxii.pancakeapps.com/LEONARDO). Arduino Uno cannot interface as a keyboard as easily as Leonardo. Only [Leonardo, Micro, and Due](https://www.arduino.cc/en/Reference/MouseKeyboard) can use the built-in keyboard function.

##Features
- Not a joystick, but emulates computer keyboard via USB. (HID interface standard) sdksldksldkdlksdlkdlskdlsddsskdkdkkkkklslskdlsdqooooppppoowowowwoqqqqqpqwwwwwqppqpqpqowowoow
- Supports LED response on all 4 BT + 2 FX + START button.
- Supports encoders with 2 signals (quadrature encoded), e.g. Copal RES20D50-201-1
- Change Uno's input pin easily in the code to suit your project.
- Adjust corresponding keys in the code.
- Adjust sensitivity of knobs and buttons in the code.

##Requires
- A firmware flash hack that will make your Arduino HID compatible but loses ability to program. (Need to flash the firmware back to reprogram) (Cannot brick your Uno.)
- 

Please wait after I tested the code more.
