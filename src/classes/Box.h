#pragma once

class Box
{
    public:
        int x0;
        int y0;
        int xlen;
        int ylen;
        int ID;

        Box(int _x0, int _y0, int _xlen, int _ylen, int _ID);

        void printBoxInfo();

};