# OpenRowingMonitor
Better rowing machine monitor for Lidl (Crivit) Rowing machine

This is an attempt to improve the Lidl Rowing machine monitor. 

I recently bought this machine. Even though the rowing mechanism is quite good, the monitor is pretty bad and it does not even measure training time properly.
So, I've tried to improve it by directly using sensor's signals and processing them in a better way.

# DISCLAIMER: 

This is a project I started for fun and it sould not be considered accurrate in any way. It requires to dissasembly your monitor and could lead to broke your machine. 
So, if you break it, it is only your responsability. 

# How does it works

Basically, the ergometer has 4 wires. VCC, GND, Spinning sensor and seat sensor. We dont need VCC since its 3.3V and we cannot use it as a power source for the arduino (?). All you need to do is to connect GNDs to each other (Rowing machine with arduino) and connect Spinning sensor to A0 and seat sensor to A1. Would be nice to add a little capacitor in parallel between the sensors and GND to prevent noise, but in my experience its enough with the debounce mechanism inside the code. It uses a timer interrupt to check the analog inputs every 20ms, more than enough for our needs.

When you power it on, it will show the main screen:

![image](https://user-images.githubusercontent.com/65807194/129487906-eb1df45e-8502-4836-8924-c8bddca48a47.png)

If you start rowing (triggered by wheel sensor) it will change into training screen:

![image](https://user-images.githubusercontent.com/65807194/129487998-bcbe2150-66ec-42d3-a2d2-798ba0e578fc.png)


# Materials used:
- Arduino UNO
- I2C OLED Display

# Version 0.1 Initial release.

What is working:
Measuring:
1. Time
2. Current partial /500m
3. Total partial /500m
4. Distance rowed
5. Strokes per minute

# Work to do:
1. Add the posibility to select from different trainings or even setting the desired time or distance.
2. Save data to a external SD card to have the possibility to review the trainings later.
