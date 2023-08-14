#include "Box.h"
#include "TreeNode.h"
#include <iostream>


TreeNode::TreeNode(int _nodeID, int _fatherID, int _PB, int _DB, int _level)
{
    nodeID = _nodeID;
    fatherID = _fatherID;
    PB = _PB;
    DB = _DB;
    level = _level;
}

void TreeNode::setBoxes(std::vector<Box*> _boxesVector)
{
    std::cout << "aaa" << std::endl;
    nBoxes = _boxesVector.size();
    boxes = (Box**) malloc(nBoxes * sizeof(Box*));
    /*for(int i = 0; i < nBoxes; i++)
    {
        boxes[i] = _boxesVector.at(i);
    }*/
    copy(_boxesVector.begin(), _boxesVector.end(), boxes);
    std::cout << "bbb" << std::endl;
}



