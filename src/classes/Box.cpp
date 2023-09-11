#include "Box.h"

#include <iostream>

Box::Box(float _x0, float _y0, float _xlen, float _ylen, int _ID)
{
    x0 = _x0;
    y0 = _y0;
    xlen = _xlen;
    ylen = _ylen;
    ID = _ID;
}

void Box::printBoxInfo()
{
    std::cout << "Box: " << x0 << " " << y0 << " " << xlen << " " << ylen << " " << ID << std::endl;
}