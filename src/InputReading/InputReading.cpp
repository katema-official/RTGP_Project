#include "InputReading.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "../classes/Box.h"
#include "../classes/TreeNode.h"
#include "../classes/Projection.h"


int readNodesNumber()
{
    std::string line;
    std::ifstream nodesFile("./InputReading/Files/nodesNumber.txt");
    getline(nodesFile, line);
    int nOfNodes = stoi(line);

    nodesFile.close();
    return nOfNodes;
}

void readNodesInformations(int& wContainer, int& hContainer, std::vector<Box*>& obstaclesVector, std::vector<TreeNode*>& treeNodesVector)
{
    std::string line;
    std::ifstream infoFile("./InputReading/Files/nodesInformations.txt");

    //first read the dimensions
    getline(infoFile, line);
    std::string arrOfDimensions[2];
    int i = 0;
    std::stringstream ssin(line);
    while (ssin.good() && i < 2)    //another way to do this: https://www.techiedelight.com/split-string-cpp-using-delimiter/, with getline
    {
        ssin >> arrOfDimensions[i];
        i++;
    }
    wContainer = stoi(arrOfDimensions[0]);
    hContainer = stoi(arrOfDimensions[1]);
    getline(infoFile, line);

    //then read the obstacles
    bool obstaclesFinished = false; 
    while (obstaclesFinished == false) {
        getline (infoFile, line);
        if(line == "-----") 
        {
            obstaclesFinished = true;
        }
        else
        {
            std::string arrOfObstacleData[4];
            int i = 0;
            std::stringstream ssin(line);
            while (ssin.good() && i < 4)
            {
                ssin >> arrOfObstacleData[i];
                i++;
            }
            Box* newObstacle = new Box(stoi(arrOfObstacleData[0]), stoi(arrOfObstacleData[1]),
                                        stoi(arrOfObstacleData[2]), stoi(arrOfObstacleData[3]), -1);
            obstaclesVector.push_back(newObstacle);
        }
    }
    
    //now we have to read and store, one by one, the nodes of the tree produced
    //std::cout << getline(infoFile, line) << std::endl;
    while (infoFile.peek() != EOF) {

        getline(infoFile, line);
        int nodeID = stoi(line);
        getline(infoFile, line);
        int fatherID = stoi(line);
        getline(infoFile, line);
        int PB = stoi(line);
        getline(infoFile, line);
        int DB = stoi(line);
        getline(infoFile, line);
        int bestPB = stoi(line);
        getline(infoFile, line);
        int levelInTree = stoi(line);

        getline(infoFile, line);
        std::vector<int> remainingQuantities;
        std::stringstream ss(line);
        std::string s;
        const char delim = ' ';
        while(getline(ss, s, delim))
        {
            remainingQuantities.push_back(stoi(s));
        }


        bool finishedReadingBoxes = false;
        std::vector<Box*> boxesPlaced;
        while(!finishedReadingBoxes)
        {
            getline(infoFile, line);
            if(line == "-----")
            {
                finishedReadingBoxes = true;
            }
            else
            {
                std::string arrOfBoxData[5];
                int i = 0;
                std::stringstream ssin(line);
                while (ssin.good() && i < 5)
                {
                    ssin >> arrOfBoxData[i];
                    i++;
                }
                Box* newBox = new Box(stoi(arrOfBoxData[0]), stoi(arrOfBoxData[1]),
                                            stoi(arrOfBoxData[2]), stoi(arrOfBoxData[3]), stoi(arrOfBoxData[4]));
                boxesPlaced.push_back(newBox);
            }  
        }

        bool finishedReadingProjections = false;
        std::vector<Projection*> projections;
        while(!finishedReadingProjections)
        {
            getline(infoFile, line);
            if(line == "-----")
            {
                finishedReadingProjections = true;
            }
            else
            {
                std::string arrOfProjectionsData[4];
                int i = 0;
                std::stringstream ssin(line);
                while (ssin.good() && i < 4)
                {
                    ssin >> arrOfProjectionsData[i];
                    i++;
                }
                Projection* newProj = new Projection(stoi(arrOfProjectionsData[0]), stoi(arrOfProjectionsData[1]),
                                            stoi(arrOfProjectionsData[2]), stoi(arrOfProjectionsData[3]));
                projections.push_back(newProj);
            }  
        }
        
        
        TreeNode* newTreeNode = new TreeNode(nodeID, fatherID, PB, DB, bestPB, levelInTree, remainingQuantities);
        newTreeNode->setBoxes(boxesPlaced);
        newTreeNode->setProjections(projections);
        treeNodesVector.push_back(newTreeNode);


        // Output the text from the file
        //std::cout << line << std::endl;
    }

    infoFile.close();
    return;

}

void cosa(int& a)
{
    a = 10;
}






