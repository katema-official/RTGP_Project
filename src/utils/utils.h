#pragma once

//function that converts HSV to RGB.
//H ranges between 0 to 360. s and v range from 0 to 1.
//Based on: https://www.cs.rit.edu/~ncs/color/t_convert.html 
float* HSVtoRGB(float h, float s, float v);

//function that generates an unique color from an integer ID
float* getColorFromID(int ID);

