#include "Box.h"

#include <iostream>

Box::Box(int _x0, int _y0, int _xlen, int _ylen, int _ID)
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