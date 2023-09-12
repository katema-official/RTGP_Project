#include "Box.h"
#include "TreeNode.h"
#include <iostream>


TreeNode::TreeNode(int _nodeID, int _fatherID, int _PB, int _DB, int _bestPB, int _level, std::vector<int> _remainingQuantities)
{
    nodeID = _nodeID;
    fatherID = _fatherID;
    PB = _PB;
    DB = _DB;
    bestPB = _bestPB;
    level = _level;
    boxes = nullptr;        //the nullptr initialization is necessary in order to correctly free memory later on
    nBoxes = 0;
    projections = nullptr;
    nProjections = 0;
    remainingQuantities = _remainingQuantities;
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

void TreeNode::printTreeNode()
{
    std::cout << "Node ID: " << nodeID << std::endl;
    std::cout << "Father ID: " << fatherID << std::endl;
    std::cout << "Level in tree: " << level << std::endl;
    std::cout << "PB: " << PB << std::endl;
    std::cout << "DB: " << DB << std::endl;
    std::cout << "Best PB so far: " << bestPB << std::endl;

    std::cout << "Remaining quantities: " << std::endl;
    for(int q : remainingQuantities)
    {
        std::cout << q << " ";
    }
    std::cout << std::endl;

    std::cout << "Boxes: " << std::endl;
    for(int i = 0; i < nBoxes; i++)
    {
        boxes[i]->printBoxInfo();
    }

    std::cout << "Projections: " << std::endl;
    for(int i = 0; i < nProjections; i++)
    {
        projections[i]->printProjectionInfo();
    }
}

