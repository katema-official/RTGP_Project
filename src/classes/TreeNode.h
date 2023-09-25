#pragma once
#include "Box.h"
#include "Projection.h"
#include <vector>

class TreeNode
{
    public:
        int explorationID;
        int fatherID;
        int creationID;
        int PB;
        int DB;
        int bestPB;
        int level;
        Box** boxes;
        int nBoxes;
        Projection** projections;
        int nProjections;
        Box** falseBoxes;
        int nFalseBoxes;
        std::vector<int> remainingQuantities;
        std::vector<int> childNodesExplorationID;

        TreeNode(int _explorationID, int _fatherID, int _creationID, int _PB, int _DB, int bestPB, int _level, std::vector<int> remainingQuantities);

        ~TreeNode();

        void setBoxes(std::vector<Box*> _boxesVector);

        void setProjections(std::vector<Projection*> _projectionsVector);

        void setFalseBoxes(std::vector<Box*> _boxesVector);

        void printTreeNode();

        void addChildNodeExplorationID(int eID);
};


