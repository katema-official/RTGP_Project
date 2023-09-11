#pragma once

class Box
{
    public:
        float x0;
        float y0;
        float xlen;
        float ylen;
        int ID;

        Box(float _x0, float _y0, float _xlen, float _ylen, int _ID);

        void printBoxInfo();

};