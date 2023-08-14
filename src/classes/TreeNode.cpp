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
    boxes = nullptr;        //the nullptr initialization is necessary in order to correctly free memory later on
    projections = nullptr;
}

TreeNode::~TreeNode()
{
    for(int i = 0; i < nBoxes; i++)
    {
        delete boxes[i];
    }
    delete [] boxes;

    for(int i = 0; i < nProjections; i++)
    {
        delete projections[i];
    }
    delete [] projections;
}

void TreeNode::setBoxes(std::vector<Box*> _boxesVector)
{
    nBoxes = _boxesVector.size();
    boxes = (Box**) malloc(nBoxes * sizeof(Box*));
    /*for(int i = 0; i < nBoxes; i++)
    {
        boxes[i] = _boxesVector.at(i);
    }*/
    copy(_boxesVector.begin(), _boxesVector.end(), boxes);
}

void TreeNode::setProjections(std::vector<Projection*> _projectionsVector)
{
    nProjections = _projectionsVector.size();
    projections = (Projection**) malloc(nProjections * sizeof(Projection*));
    copy(_projectionsVector.begin(), _projectionsVector.end(), projections);
}


