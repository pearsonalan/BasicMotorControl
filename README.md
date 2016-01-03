# BasicMotorControl

The Motor class implements driving logic for a stepper motor. This can be used
to operate a motor like the 28BYJ-48 using a transistor array such as
a ULN2003a or ULN2803a

This project includes a Makefile for compiling the code and uploading it to
arduino.  To use this you may need to edit the `PORT` assignment at the top
of the Makefile.  Also the Makefile assumes Arduino 1.6.4 and an Arduino Uno
using an ATmega328p chip.

```
make
make upload
```

## Simulation

The sim directory contains a simulator based on the simavr project to simulate
a board that can run the BasicMotorControl program.
