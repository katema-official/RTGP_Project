#pragma once
#include "Box.h"
#include "Projection.h"
#include <vector>

class TreeNode
{
    public:
        int nodeID;
        int fatherID;
        int PB;
        int DB;
        int bestPB;
        int level;
        Box** boxes;
        int nBoxes;
        Projection** projections;
        int nProjections;

        TreeNode(int _nodeID, int _fatherID, int _PB, int _DB, int bestPB, int _level);

        ~TreeNode();

        void setBoxes(std::vector<Box*> _boxesVector);

        void setProjections(std::vector<Projection*> _projectionsVector);

        void printTreeNode();
};


