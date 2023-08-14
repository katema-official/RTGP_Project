#pragma once
#include "Box.h"
#include <vector>

class TreeNode
{
    public:
        int nodeID;
        int fatherID;
        int PB;
        int DB;
        int level;
        Box** boxes;
        int nBoxes;

        TreeNode(int _nodeID, int _fatherID, int _PB, int _DB, int _level);

        void setBoxes(std::vector<Box*> _boxesVector);
};


