#pragma once

#include <vector>
#include "../classes/Box.h"
#include "../classes/TreeNode.h"


//to read the total number of nodes explored by the B&B algorithm
int readNodesNumber();

//to read all the main informations regarding the instance to visualize
void readNodesInformations(int& wContainer, int& hContainer, std::vector<Box*>& obstaclesVector, std::vector<TreeNode*>& treeNodesVector);

void cosa(int& a);






