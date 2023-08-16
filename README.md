# Simulation
A little project for learning the concepts of graphic development
using OpenGL for a simulation of chaotic ODE systems.  
Yet implemented are two: Lorenz- and Roessler-System.

The project uses the OpenGL support libraries GLFT/GLAD and depends
on the boost C++ library for solving ODE systems.

## Display modes:

Some display modes of the ODE-solution are available:
* A: static projection to (x,y)-plane
* B: rotating around x-axis with constant rotation time
* C: rotating around y-axis with constant rotation time
* D: mouse driven rotating around both x- and y-axis

## Usage:
* mouse button 1: (only display mode A) select at cursor position an initial value in (x,y)-plane with z=0 and restart simulation
* mouse button 2: successive selecting display modes A to D
* mouse wheel: select zoom-value (range 0.1 to 5.0)

* keybord key FORWARD: change rotation direction to forward
* keybord key BACKWARD: change rotation direction to backward
* keyboard keys UP/DOWN: stop rotation
* keyboard key ESC exit the program

## Screenshot display mode D:
  
![Simulation](https://github.com/JW-Schuetz/ChaosSimulation/assets/40438317/cea51866-98b3-40d1-9ac5-eeeef2d1f4a9)
  
