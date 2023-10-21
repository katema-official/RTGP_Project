#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Drawing.h"
#include "../utils/utils.h"
#include "../MeaningfulCoordinates/MeaningfulCoordinates.h"
#include "../classes/TreeNode.h"
#include "../classes/Box.h"
#include "../classes/Projection.h"

#include <iostream>
#include <tuple>
#include <vector>

#include <glm/glm.hpp>

#include <settings.h>
#include <camera.h>

#include "../InputReading/stb_image.h"

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern int fontSize;
extern std::map<GLchar, Character> Characters;
extern float scalingQuantitiesText;

extern int globalOptimumID;

//############################################################################
//################# FUNCTIONS RELATIVE TO THE NODES RENDERING ################
//############################################################################

glm::vec2 getContainerEffectiveDimensions(int wContainer, int hContainer, float maxPortionDedicatedToContainer)
{
    float wContainerTrue = (float) wContainer;
    float hContainerTrue = (float) hContainer;
    
    float pixelsHorizontal = ((float) SCR_WIDTH) * maxPortionDedicatedToContainer;
    float pixelsVertical = ((float) SCR_HEIGHT) * maxPortionDedicatedToContainer;
    float ratio = pixelsVertical / pixelsHorizontal;

    wContainerTrue = ratio * wContainerTrue;

    return glm::vec2(wContainerTrue, hContainerTrue);
}


//-wContainer: the width of the container
//-hContainer: the height of the container
//-thickness: to express how thick the walls of the container should be wrt to SCR_WIDTH and SCR_HEIGHT respectively
//-maxPortionDedicatedToContainer: to express the maximum percentage of the window that should be dedicated to the container
//the remaining are data used to draw boxes or text
void drawStaticInformations(int wC, int hC,
                            float wContainer, float hContainer, 
                            float wThickness, float hThickness,
                            float maxPortionDedicatedToContainer,
                            Shader boxShader, unsigned int* buffersForBox,
                            Shader textShader, std::vector<Box*> obstaclesVector
                            )
{
    if(wThickness > 1 || wThickness <= 0 || 
        hThickness > 1 || hThickness <= 0 || 
        maxPortionDedicatedToContainer > 1 || maxPortionDedicatedToContainer <= 0)
    {
        std::cout << "ERROR::drawStaticInformations: data not between 0 and 1" << std::endl;
        exit;
    }


    glm::vec4 containerColor = glm::vec4(0.0, 0.0, 0.0, 1.0);

    float winnerLength = wContainer > hContainer ? wContainer : hContainer;
    float loserLength =  wContainer > hContainer ? hContainer : wContainer;

    float widthPortion;
    float heightPortion;

    //draw the container
    if(winnerLength == wContainer)
    {
        widthPortion = maxPortionDedicatedToContainer;
        drawBoxShape(boxShader, buffersForBox, 0, 0, 0 + widthPortion, 0 + wThickness, containerColor);  //bottom
        heightPortion = (hContainer * widthPortion) / wContainer;
        drawBoxShape(boxShader, buffersForBox, 0, 0, 0 + hThickness, 0 + heightPortion, containerColor);   //left
        drawBoxShape(boxShader, buffersForBox, 0, 0 + heightPortion, 0 + widthPortion, 0 + heightPortion - wThickness, containerColor);  //top
        drawBoxShape(boxShader, buffersForBox, 0 + widthPortion, 0, 0 + widthPortion - hThickness, 0 + heightPortion, containerColor); //right
    }
    else
    {
        heightPortion = maxPortionDedicatedToContainer;
        drawBoxShape(boxShader, buffersForBox, 0, 0, 0 + hThickness, 0 + heightPortion, containerColor);  //left
        widthPortion = (wContainer * heightPortion) / hContainer;
        drawBoxShape(boxShader, buffersForBox, 0, 0, 0 + widthPortion, 0 + wThickness, containerColor);    //bottom
        drawBoxShape(boxShader, buffersForBox, 0 + widthPortion, 0, 0 + widthPortion - hThickness, 0 + heightPortion, containerColor);    //right
        drawBoxShape(boxShader, buffersForBox, 0, 0 + heightPortion, 0 + widthPortion, 0 + heightPortion - wThickness, containerColor);    //top
    }

    //draw the obstacles
    for(Box* obs: obstaclesVector)
    {
        glm::vec4 coords = fromInputBoxToRelativeCoordinates(obs, wC, hC, wContainer, hContainer, wThickness, hThickness, maxPortionDedicatedToContainer);
        drawBoxShape(boxShader, buffersForBox, coords.x, coords.y, coords.z, coords.w, containerColor);
    }

    //draw the static text
    float rescaleTextFactor = 0.125f;
    glm::vec2 nodeExplorationIdCoords = getNodeExplorationId_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 nodeCreationIdCoords = getNodeCreationId_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 fatherIdCoords = getFatherId_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 levelInTreeCoords = getLevelInTree_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 PBCoords = getPB_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 DBCoords = getDB_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 bestPBCoords = getBestPB_STATIC_Coordinates(maxPortionDedicatedToContainer);
    RenderText(textShader, "Exploration ID: ", nodeExplorationIdCoords.x, nodeExplorationIdCoords.y, rescaleTextFactor, containerColor);
    RenderText(textShader, "Creation ID: ", nodeCreationIdCoords.x, nodeCreationIdCoords.y, rescaleTextFactor, containerColor);
    RenderText(textShader, "Father (exp) ID: ", fatherIdCoords.x, fatherIdCoords.y, rescaleTextFactor, containerColor);
    RenderText(textShader, "Level in tree: ", levelInTreeCoords.x, levelInTreeCoords.y, rescaleTextFactor, containerColor);
    RenderText(textShader, "PB: ", PBCoords.x, PBCoords.y, rescaleTextFactor, containerColor);
    RenderText(textShader, "DB: ", DBCoords.x, DBCoords.y, rescaleTextFactor, containerColor);
    RenderText(textShader, "Best PB: ", bestPBCoords.x, bestPBCoords.y, rescaleTextFactor, containerColor);

    glm::vec4 containerDimensionsCoords = getContainerDimensions_STATIC_Coordinates(maxPortionDedicatedToContainer);
    RenderText(textShader, "Container", containerDimensionsCoords.x, containerDimensionsCoords.y, rescaleTextFactor, containerColor);
    RenderText(textShader, "dimensions: ", containerDimensionsCoords.z, containerDimensionsCoords.w, rescaleTextFactor, containerColor);

    glm::vec2 a = getContainerDimensions_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    std::string dims = std::to_string(wC) + " x " + std::to_string(hC);
    RenderText(textShader, dims, a.x, a.y, rescaleTextFactor, containerColor);
}

unsigned int* getBuffersToDrawBoxShape()
{
    unsigned int* buffers = new unsigned int[3]; //there will be the VAO, the VBO and the EBO

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,                //this works fine under the assumption that in the VBO the vertices
        1, 2, 3                 //are ordered like this: bottom-left, bottom-right, top-left, top-right
    };

    glGenVertexArrays(1, &buffers[0]);
    glGenBuffers(1, &buffers[1]);
    glGenBuffers(1, &buffers[2]);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    //24 because we assume there will be four vertices, each composed of xyz and rgb (and each of them is a float)
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return buffers;

}



void drawBoxShape(Shader& shader, unsigned int* buffers, float x0, float y0, float x1, float y1, glm::vec4 color)
{
    if(x0 < 0 || x0 > 1 ||
        y0 < 0 || y0 > 1 ||
        x1 < 0 || x1 > 1 ||
        y1 < 0 || y1 > 1
    )
    {
        std::cout << "ERROR::drawBoxShape: data not between 0 and 1" << std::endl;
        exit;
    }

    x0 = x0*2 - 1;
    y0 = y0*2 - 1;
    x1 = x1*2 - 1;
    y1 = y1*2 - 1;
    std::vector<float> vertices_vector;
    vertices_vector.push_back(x0);
    vertices_vector.push_back(y0);
    vertices_vector.push_back(0.0f);
    vertices_vector.push_back(color.r);
    vertices_vector.push_back(color.g);
    vertices_vector.push_back(color.b);

    vertices_vector.push_back(x1);
    vertices_vector.push_back(y0);
    vertices_vector.push_back(0.0f);
    vertices_vector.push_back(color.r);
    vertices_vector.push_back(color.g);
    vertices_vector.push_back(color.b);

    vertices_vector.push_back(x0);
    vertices_vector.push_back(y1);
    vertices_vector.push_back(0.0f);
    vertices_vector.push_back(color.r);
    vertices_vector.push_back(color.g);
    vertices_vector.push_back(color.b);

    vertices_vector.push_back(x1);
    vertices_vector.push_back(y1);
    vertices_vector.push_back(0.0f);
    vertices_vector.push_back(color.r);
    vertices_vector.push_back(color.g);
    vertices_vector.push_back(color.b);

    int nVertices = vertices_vector.size();
    float* vertices = (float*) malloc(nVertices * sizeof(float));
    copy(vertices_vector.begin(), vertices_vector.end(), vertices);

    //to free the vector
    std::vector<float>().swap(vertices_vector);

    

    shader.use();
    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nVertices * sizeof(float), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    //the memory was dinamically allocated, it must now be freed
    free(vertices);
}





void drawNode_v1(TreeNode* treeNode, unsigned int* boxBuffers,
                    int wC, int hC,
                    float wContainer, float hContainer, 
                    float wThickness, float hThickness, 
                    float maxPortionDedicatedToContainer,
                    Shader& boxShader, Shader& textShader)
{
    float maxDim = wC > hC ? wC : hC;
    float lengthSingleDottedLine = (5 * maxDim) / 250;
    float thicknessProjection = (1 * maxDim) / 250;

    glm::vec2 coordsNodeExplorationID = getNodeExplorationId_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsNodeCreationID = getNodeCreationId_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsFatherID = getFatherId_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsLevelInTree = getLevelInTree_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsPB = getPB_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsDB = getDB_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsBestPB = getBestPB_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);


    glm::vec4 textColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
    float rescaleTextInfo = 0.125;

    RenderText(textShader, std::to_string(treeNode->explorationID), coordsNodeExplorationID.x, coordsNodeExplorationID.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->creationID), coordsNodeCreationID.x, coordsNodeCreationID.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->fatherID), coordsFatherID.x, coordsFatherID.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->level), coordsLevelInTree.x, coordsLevelInTree.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->PB), coordsPB.x, coordsPB.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->DB), coordsDB.x, coordsDB.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->bestPB), coordsBestPB.x, coordsBestPB.y, rescaleTextInfo, textColor);

    glm::vec4 coordsContainer = getLowerLeftAndUpperRightCoordinatesOfContainer(wContainer, hContainer, wThickness, hThickness, maxPortionDedicatedToContainer);

    
    float rescaleTextID = 0.25;
    for(int i = 0; i < treeNode->nBoxes; i++)
    {
        Box* thisBox = treeNode->boxes[i];

        glm::vec4 coords = fromInputBoxToRelativeCoordinates(thisBox, wC, hC, wContainer, hContainer, wThickness, hThickness, maxPortionDedicatedToContainer);
        int ID = thisBox->ID;

        drawBoxShape(boxShader, boxBuffers, coords.x, coords.y, coords.z, coords.w, getColorFromID(ID));
        
        float xDiff = coords.z - coords.x;
        float yDiff = coords.w - coords.y;
        float xText = coords.x + (xDiff / 3.0);
        float yText = coords.y + (yDiff / 3.0);

        RenderText(textShader, std::to_string(ID), xText, yText, rescaleTextID, textColor);

        int xLenInt = (int) thisBox->xlen;
        int yLenInt = (int) thisBox->ylen;
        RenderText(textShader, std::to_string(xLenInt), xText, coords.y, rescaleTextID / 3, textColor);
        RenderText(textShader, std::to_string(yLenInt), coords.x, yText, rescaleTextID / 3, textColor);
    }

    glm::vec4 colorFalseBox = glm::vec4(1.0, 1.0, 1.0, 1.0);
    for(int i = 0; i < treeNode->nFalseBoxes; i++)
    {
        Box* thisBox = treeNode->falseBoxes[i];

        glm::vec4 coords = fromInputBoxToRelativeCoordinates(thisBox, wC, hC, wContainer, hContainer, wThickness, hThickness, maxPortionDedicatedToContainer);

        drawBoxShape(boxShader, boxBuffers, coords.x, coords.y, coords.z, coords.w, colorFalseBox);
    }

    for(int i = 0; i < treeNode->nProjections; i++)
    {
        Projection* thisProjection = treeNode->projections[i];
        std::vector<glm::vec4> linesOfThisProjection = getProjectionAsDottedLine(thisProjection, wC, hC, wContainer, hContainer,
                                                wThickness, hThickness, maxPortionDedicatedToContainer, lengthSingleDottedLine, thicknessProjection);
        for(glm::vec4 line : linesOfThisProjection)
        {
            drawBoxShape(boxShader, boxBuffers, line.x, line.y, line.z, line.w, textColor);
        }
    }

    Character ch = Characters['0'];
    float w = ch.Size.x * scalingQuantitiesText;
    float h = ch.Size.y * scalingQuantitiesText;
    glm::vec2 quantitiesInitialCoords = getRemainingQuantities_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    float x = quantitiesInitialCoords.x;
    float y = quantitiesInitialCoords.y;
    int index = 0;

    float yMax = getContainerDimensions_STATIC_Coordinates(maxPortionDedicatedToContainer).y;   //the lowest y value we can reach in a column before covering other text ("Container dimensions: ")
    float wAdd = Characters[':'].Size.x * scalingQuantitiesText + 0.003;                        //an offset to add to the width of the character so that the colored box behind it covers the ID completely
    float xOffset = Characters['0'].Size.x * 2 * scalingQuantitiesText;                         //when the current column is about to go past yMax, we have to create a new column on the right. This parameter gives a small horizontal offset to avoid overlapping of text
    
    int columnIndex = 0;
    for(int i : treeNode->remainingQuantities)
    {
        int q = i;
        int ID_len = std::to_string(index).length();
        std::string actualText = std::to_string(index) + ": " + std::to_string(q);
        drawBoxShape(boxShader, boxBuffers, x, y, x + (((w + wAdd) * ID_len) / SCR_WIDTH), y + (h / SCR_HEIGHT), getColorFromID(index));
        RenderText(textShader, actualText, x, y, scalingQuantitiesText, textColor);
        index++;
        y -= (h / SCR_HEIGHT) * 1.3;

        //shall we go to the right?
        if(y - (h / SCR_HEIGHT) * 1.3 <= yMax)
        {
            columnIndex++;
            y = quantitiesInitialCoords.y;
            x = quantitiesInitialCoords.x + (((w + wAdd + xOffset) * columnIndex * ID_len) / SCR_WIDTH);
        }
    }


}



float speedFullyVisibleDuration = 0.5f;
float speedFadeOutDuration = 0.5f;
float initialTimeNewSpeed = -speedFullyVisibleDuration - speedFadeOutDuration - 0.01;

void drawSpeed(Shader& shader, int speed, bool updated, float _initialTimeNewSpeed, float currentTime)
{
    if(updated)
    {
        initialTimeNewSpeed = _initialTimeNewSpeed;
        return;
    }

    if(currentTime > initialTimeNewSpeed + speedFullyVisibleDuration + speedFadeOutDuration) return;
    
    glm::vec4 colorText;
    if(currentTime <= initialTimeNewSpeed + speedFullyVisibleDuration)
    {
        colorText = glm::vec4(0.0, 0.0, 0.0, 1.0);
    }
    else if(currentTime <= initialTimeNewSpeed + speedFullyVisibleDuration + speedFadeOutDuration)
    {
        float a = 0.0f;
        float b = 1.0f;
        float f = 1 - ((currentTime - initialTimeNewSpeed - speedFullyVisibleDuration) / speedFadeOutDuration);
        float transparency = (a * (1.0 - f) + (b * f));
        colorText = glm::vec4(0.0, 0.0, 0.0, transparency);
    }

    RenderText(shader, std::to_string(speed) + "x", 0.005, 0.025, 1, colorText);
    
    return;
}






//############################################################################
//################# FUNCTIONS RELATIVE TO THE TREE RENDERING #################
//############################################################################

unsigned int* getBuffersWithDataToDrawRectangleNode()
{
    unsigned int* buffers = new unsigned int[3]; //there will be the VAO, the VBO and the EBO

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,                //this works fine under the assumption that in the VBO the vertices
        1, 2, 3                 //are ordered like this: bottom-left, bottom-right, top-left, top-right
    };

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
    };

    glGenVertexArrays(1, &buffers[0]);
    glGenBuffers(1, &buffers[1]);
    glGenBuffers(1, &buffers[2]);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return buffers;
}



void drawNodeTextInTree(Shader& textShader, unsigned int* buffersRectangle, Camera camera, glm::vec3 position, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, TreeNode* node)
{
    glm::vec3 nodePosition = position;
    
    textShader.use();

    glm::vec3 textPosition = nodePosition;
    textPosition.z += 1.0f;
    glm::mat4 modelText = glm::mat4(1.0f);
    modelText = glm::translate(modelText, textPosition);
    textShader.setMat4("model", modelText);

    RenderTextInSpace(textShader, "Exploration ID:", -0.45, 0.1, 0.0005, glm::vec4(0.0, 0.0, 0.0, 1.0));
    RenderTextInSpace(textShader, std::to_string(node->explorationID), -0.45, -0.1, 0.001, glm::vec4(0.0, 0.0, 0.0, 1.0));
}




glm::vec3 coordinatesNewNodeToDraw(0.0, 0.0, -100.0);
float verticalSpaceBetweenLevels = 4.0f;    //it's actually the vertical space between the center of a node (square) at level i and the center of a node (square) at level i+1
float horizontalSpaceBetweenNodes = 1.1f;
glm::vec4 colorNormalNode(1.0, 1.0, 1.0, 1.0);
glm::vec4 colorOptimumNode(0.0, 1.0, 0.0, 1.0);

//the tree will have its root node at (0,0) and will be built downward to the right. In this way:
//-all the nodes of the same level in the tree will have the same y coordinate
//-whenever a node has one or more child, we build them going to the level below, and moving to the right if we need to

void drawWholeTreeText(TreeNode* node, std::vector<TreeNode*> nodesVector, Shader& textShaderInSpace, unsigned int* buffersForNodeInTree, Camera camera, glm::mat4 view, glm::mat4 projection)
{
    if(node->explorationID == 0)
    {
        //otherwise at each frame the nodes would be rendered more and more to the right
        coordinatesNewNodeToDraw = glm::vec3(0.0, 0.0, -100.0);
    }

    //draw the current node text. The x and y coordinates are given by the global variable "coordinatesNewNodeToDraw", while the y is given by the level of the node in the tree
    glm::vec3 posNode(coordinatesNewNodeToDraw.x, -1.0 * node->level * verticalSpaceBetweenLevels, coordinatesNewNodeToDraw.z);
    drawNodeTextInTree(textShaderInSpace, buffersForNodeInTree, camera, posNode, view, projection, node);

    return;

    //if this node has at least one child
    if((node->childNodesExplorationID).size() > 0)
    {
        int currentChildLocalIndex = 0;

        //for each child of this node
        for(int i : node->childNodesExplorationID)
        {
            TreeNode* child = nodesVector.at(i);
            if(currentChildLocalIndex > 0)
            {
                coordinatesNewNodeToDraw.x += horizontalSpaceBetweenNodes;
            }

            //draw the child
            drawWholeTreeText(child, nodesVector, textShaderInSpace, buffersForNodeInTree, camera, view, projection);

            currentChildLocalIndex++;
        }
    }

    //drawNodeInTree(nodeInTreeShader, textShaderInSpace, buffersForNodeInTree, camera, glm::vec3(0.0, 0.0, -100.0), view, projection, 0);
    //drawNodeInTree(nodeInTreeShader, textShaderInSpace, buffersForNodeInTree, camera, glm::vec3(10.0, 5.0, -100.0), view, projection, 1);

}






//actually we also add the colors
void addModelMatrix_Nodes(std::vector<glm::mat4>& modelVector, std::vector<glm::vec4>& colorVector, std::vector<int>& indicesVector, std::vector<glm::vec3>& nodesPositions, TreeNode* node, std::vector<TreeNode*> nodesVector)
{
    indicesVector.push_back(node->explorationID);

    if(node->explorationID == 0)
    {
        coordinatesNewNodeToDraw = glm::vec3(0.0, 0.0, -100.0);
    }

    //the model matrix of a node just needs to be translated
    glm::vec3 posNode(coordinatesNewNodeToDraw.x, -1.0 * node->level * verticalSpaceBetweenLevels, coordinatesNewNodeToDraw.z);
    nodesPositions.push_back(posNode);
    glm::mat4 modelNode = glm::mat4(1.0f);
    modelNode = glm::translate(modelNode, posNode);

    modelVector.push_back(modelNode);

    if(node->explorationID == globalOptimumID)
    {
        colorVector.push_back(colorOptimumNode);
    }
    else
    {
        colorVector.push_back(colorNormalNode);
    }

    //if this node has at least one child
    if((node->childNodesExplorationID).size() > 0)
    {
        int currentChildLocalIndex = 0;

        //for each child of this node
        for(int i : node->childNodesExplorationID)
        {
            TreeNode* child = nodesVector.at(i);
            if(currentChildLocalIndex > 0)
            {
                coordinatesNewNodeToDraw.x += horizontalSpaceBetweenNodes;
            }

            //recursive call
            addModelMatrix_Nodes(modelVector, colorVector, indicesVector, nodesPositions, child, nodesVector);

            currentChildLocalIndex++;
        }
    }
}

//actually we also add the colors
void addModelMatrix_Bridges(std::vector<glm::mat4>& modelVector, std::vector<glm::vec4>& colorVector, TreeNode* node, std::vector<TreeNode*> nodesVector, int& count)
{
    if(node->explorationID == 0)
    {
        coordinatesNewNodeToDraw = glm::vec3(0.0, 0.0, -100.0);
    }

    glm::vec3 posNode(coordinatesNewNodeToDraw.x, -1.0 * node->level * verticalSpaceBetweenLevels, coordinatesNewNodeToDraw.z);

    //if this node has at least one child
    if((node->childNodesExplorationID).size() > 0)
    {
        int currentChildLocalIndex = 0;
        float xLastChild = posNode.x;

        //for each child of this node
        for(int i : node->childNodesExplorationID)
        {
            TreeNode* child = nodesVector.at(i);
            if(currentChildLocalIndex > 0)
            {
                coordinatesNewNodeToDraw.x += horizontalSpaceBetweenNodes;
            }

            glm::vec3 posChild(coordinatesNewNodeToDraw.x, -1.0 * (node->level + 1) * verticalSpaceBetweenLevels, posNode.z);

            //recursive call
            addModelMatrix_Bridges(modelVector, colorVector, child, nodesVector, count);

            //now draw the vertical bridge between the current node and the child node
            if(currentChildLocalIndex == 0)
            {
    
                glm::vec3 verticalBridgePos = glm::vec3(posNode.x, posChild.y + (verticalSpaceBetweenLevels / 2.0f), posNode.z - 10.0f);
                glm::mat4 modelVBridge = glm::mat4(1.0f);
                modelVBridge = glm::translate(modelVBridge, verticalBridgePos);
                modelVBridge = glm::scale(modelVBridge, glm::vec3(0.25, verticalSpaceBetweenLevels, 1.0f));

                modelVector.push_back(modelVBridge);
                colorVector.push_back(colorNormalNode);
                count++;
            }
            else
            {
                //for the others, half a bridge
                glm::vec3 halfVerticalBridgePos = glm::vec3(posChild.x, posChild.y + (verticalSpaceBetweenLevels / 4.0f), posNode.z - 10.0f);
                glm::mat4 modelHalfVBridge = glm::mat4(1.0f);
                modelHalfVBridge = glm::translate(modelHalfVBridge, halfVerticalBridgePos);
                modelHalfVBridge = glm::scale(modelHalfVBridge, glm::vec3(0.25, verticalSpaceBetweenLevels / 2.0f, 1.0f));

                modelVector.push_back(modelHalfVBridge);
                colorVector.push_back(colorNormalNode);
                count++;
            }
            xLastChild = posChild.x;

            currentChildLocalIndex++;
        }

        //now that all the child have been drawn, we can create the horizontal bridge
        glm::vec3 posLastChild(xLastChild, -1.0 * (node->level + 1) * verticalSpaceBetweenLevels, posNode.z);

        glm::vec3 horizontalBridgePos = glm::vec3(glm::mix(posNode.x, posLastChild.x, 0.5f), posNode.y - (verticalSpaceBetweenLevels) / 2.0f, posNode.z - 10.0f);
        glm::mat4 modelHBridge = glm::mat4(1.0f);
        modelHBridge = glm::translate(modelHBridge, horizontalBridgePos);
        modelHBridge = glm::scale(modelHBridge, glm::vec3(posLastChild.x - posNode.x, 0.25, 1.0f));

        modelVector.push_back(modelHBridge);
        colorVector.push_back(colorNormalNode);
        count++;
    }
}


//let's generate the transformation matrices
unsigned int getVAOWithDataToDrawNodesInTree(std::vector<int>& nodesIndices, std::vector<glm::vec3>& nodesPositions, std::vector<TreeNode*> nodesVector)
{
    //first, we get the model matrices
    std::vector<glm::mat4> modelMatrices;
    std::vector<glm::vec4> colors;

    addModelMatrix_Nodes(modelMatrices, colors, nodesIndices, nodesPositions, nodesVector.at(0), nodesVector);

    int nNodes = nodesVector.size();
    glm::mat4* modelMatricesArray = new glm::mat4[nNodes];
    copy(modelMatrices.begin(), modelMatrices.end(), modelMatricesArray);

    glm::vec4* colorsArray = new glm::vec4[nNodes];
    copy(colors.begin(), colors.end(), colorsArray);


    unsigned int modelVBO;
    glGenBuffers(1, &modelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    glBufferData(GL_ARRAY_BUFFER, nNodes * sizeof(glm::mat4), &modelMatricesArray[0], GL_STATIC_DRAW);

    unsigned int colorVBO;
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, nNodes * sizeof(glm::vec4), &colorsArray[0], GL_STATIC_DRAW);



    unsigned int* buffers = new unsigned int[3]; //there will be the VAO, the VBO and the EBO

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,                //this works fine under the assumption that in the VBO the vertices
        1, 2, 3                 //are ordered like this: bottom-left, bottom-right, top-left, top-right
    };

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
    };

    glGenVertexArrays(1, &buffers[0]);
    glGenBuffers(1, &buffers[1]);
    glGenBuffers(1, &buffers[2]);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);     //this attribute comes from a different vertex buffer

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

    glVertexAttribDivisor(1, 1);    //tell OpenGL this is an instanced vertex attribute.



    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);     //this attribute comes from a different vertex buffer

    // set attribute pointers for matrix (4 times vec4)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(2, 1);    //tell OpenGL this is an instanced vertex attribute.
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);


    unsigned int VAO = buffers[0];
    return VAO;
}

//let's generate the transformation matrices
unsigned int getVAOWithDataToDrawBridgesInTree(int& count, std::vector<TreeNode*> nodesVector)
{
    //first, we get the model matrices
    std::vector<glm::mat4> modelMatrices;
    std::vector<glm::vec4> colors;

    addModelMatrix_Bridges(modelMatrices, colors, nodesVector.at(0), nodesVector, count);

    int nBridges = count;
    glm::mat4* modelMatricesArray = new glm::mat4[nBridges];
    copy(modelMatrices.begin(), modelMatrices.end(), modelMatricesArray);

    glm::vec4* colorsArray = new glm::vec4[nBridges];
    copy(colors.begin(), colors.end(), colorsArray);


    unsigned int modelVBO;
    glGenBuffers(1, &modelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    glBufferData(GL_ARRAY_BUFFER, nBridges * sizeof(glm::mat4), &modelMatricesArray[0], GL_STATIC_DRAW);

    unsigned int colorVBO;
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, nBridges * sizeof(glm::vec4), &colorsArray[0], GL_STATIC_DRAW);



    unsigned int* buffers = new unsigned int[3]; //there will be the VAO, the VBO and the EBO

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,                //this works fine under the assumption that in the VBO the vertices
        1, 2, 3                 //are ordered like this: bottom-left, bottom-right, top-left, top-right
    };

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
    };

    glGenVertexArrays(1, &buffers[0]);
    glGenBuffers(1, &buffers[1]);
    glGenBuffers(1, &buffers[2]);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);     //this attribute comes from a different vertex buffer

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

    glVertexAttribDivisor(1, 1);    //tell OpenGL this is an instanced vertex attribute.



    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);     //this attribute comes from a different vertex buffer

    // set attribute pointers for matrix (4 times vec4)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(2, 1);    //tell OpenGL this is an instanced vertex attribute.
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);


    unsigned int VAO = buffers[0];
    return VAO;
}




void drawAllNodesInTree(std::vector<TreeNode*> originalNodesVector, std::vector<int> recursiveNodesVector, Shader& nodeInTreeShader, unsigned int VAO_Nodes, Camera camera, glm::mat4 view, glm::mat4 projection)
{
    int amount = recursiveNodesVector.size();

    nodeInTreeShader.use();
    nodeInTreeShader.setMat4("projection", projection);
    nodeInTreeShader.setMat4("view", view);

    glBindVertexArray(VAO_Nodes);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, amount);
}

void drawAllBridgesInTree(std::vector<TreeNode*> originalNodesVector, int count, Shader& nodeInTreeShader, unsigned int VAO_Bridges, Camera camera, glm::mat4 view, glm::mat4 projection)
{
    int amount = count;

    nodeInTreeShader.use();
    nodeInTreeShader.setMat4("projection", projection);
    nodeInTreeShader.setMat4("view", view);

    glBindVertexArray(VAO_Bridges);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, amount);
}

void drawTextInTree(int lettersCount, Shader& textInTreeShader, unsigned int VAO_textInTree, Camera camera, glm::mat4 view, glm::mat4 projection, unsigned int bitmapTexture)
{
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bitmapTexture);
    textInTreeShader.use();
    textInTreeShader.setMat4("projection", projection);
    textInTreeShader.setMat4("view", view);
    textInTreeShader.setVec4("textColor", glm::vec4(0.0, 0.0, 0.0, 1.0));
    glBindVertexArray(VAO_textInTree);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, lettersCount);

    glDisable(GL_BLEND);
    //glDisable(GL_CULL_FACE);
}









void addModelMatrixAndTextureCoordinates_Nodes(std::vector<std::string> stringsVector, std::vector<glm::vec3> nodesPositions, int* textWidths, int nTextWidths,
    std::vector<glm::mat4>& modelMatrices, std::vector<glm::vec2>& textureOffsets, float xOffset, float yOffset, float scaleFactor, int& count)
{
    for(int index = 0; index < stringsVector.size(); index++)   //for each string that we want to draw
    {
        std::string word = stringsVector.at(index);

        //we first convert the string to a char array, so to have access to each single character individually
        int length = word.length();
        char* lettersArray = new char[length + 1];
        strcpy(lettersArray, word.c_str());

        glm::vec3 nodeCenter = nodesPositions.at(index);
        glm::vec3 currentPosition = nodeCenter + glm::vec3(0.0, 0.0, 10.0);
        currentPosition.y += yOffset;
        currentPosition.x += xOffset;

        //now that we have access to each single letter... 
        for(int i = 0; i < length; i++)
        {
            //...let's get its texture coordinate offset
            glm::vec2 texOffset = getTextureCoordinatesOffsetOfCharacterInBitmap(lettersArray[i]);
            textureOffsets.push_back(texOffset);
            count++;

            //and let's also get the right transformation matrix for each single letter
            glm::mat4 modelLetter = glm::mat4(1.0f);
            modelLetter = glm::translate(modelLetter, currentPosition);
            modelLetter = glm::scale(modelLetter, glm::vec3(scaleFactor, scaleFactor, 1.0f));
            modelMatrices.push_back(modelLetter);
            
            char letter = lettersArray[i];
            int k = (int) letter;
            float width = ((float) textWidths[k] / 48.0) * scaleFactor; //TODO: in case, modify
            currentPosition.x += width;
            currentPosition.z += 0.001f; //so that the next gliph goes "on top" of the prevoius one
        }
    }

}

unsigned int getVAONodesText(std::vector<std::string> stringsVector, std::vector<glm::vec3> nodesPositions, int* textWidths, int& lettersCount, float xOffset, float yOffset, float scale)
{
    int nTextWidths = 128;  //textWidths has a standard length: 128

    std::vector<glm::mat4> modelVector;
    std::vector<glm::vec2> offsetCoordinatesVector;

    //given all the texts we have to render and the position in space of their corresponding node,
    //we have to produce two vectors:
    //-one with the positions                   of each individual character of each text (actually, the transformation mat4 to apply to them)
    //-one with the texture coordinate offset   of each individual character of each text
    //This is because each position of these vectors will correspond to an instance that will 
    //be drawn from the same quad, where each instance is differen from the others because of 
    //its transform matrix and its texture offset (that determines the glyph drawn)
    addModelMatrixAndTextureCoordinates_Nodes(stringsVector, nodesPositions, textWidths, nTextWidths, modelVector, offsetCoordinatesVector, xOffset, yOffset, scale, lettersCount);
    
    int nLettersTotal = lettersCount;
    glm::mat4* modelLettersArray = new glm::mat4[nLettersTotal];
    copy(modelVector.begin(), modelVector.end(), modelLettersArray);

    glm::vec2* offsetCoordinatesArray = new glm::vec2[nLettersTotal];
    copy(offsetCoordinatesVector.begin(), offsetCoordinatesVector.end(), offsetCoordinatesArray);

    unsigned int modelVBO;
    glGenBuffers(1, &modelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    glBufferData(GL_ARRAY_BUFFER, nLettersTotal * sizeof(glm::mat4), &modelLettersArray[0], GL_STATIC_DRAW);

    unsigned int offsetTextureVBO;
    glGenBuffers(1, &offsetTextureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, offsetTextureVBO);
    glBufferData(GL_ARRAY_BUFFER, nLettersTotal * sizeof(glm::vec2), &offsetCoordinatesArray[0], GL_STATIC_DRAW);


    unsigned int* buffers = new unsigned int[3]; //there will be the VAO, the VBO and the EBO

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,                //this works fine under the assumption that in the VBO the vertices
        1, 2, 3                 //are ordered like this: bottom-left, bottom-right, top-left, top-right
    };

    //glm::vec2 textureCoordinatesOffset = getTextureCoordinatesOffsetOfCharacterInBitmap('c');

    //With these texture coordinates, one would draw only the bottom-left element of the bitmap.
    //It will be the instance array of offsets to effectively determine the character drawn from
    //the bitmap. Each offset is returned from getTextureCoordinatesOffsetOfCharacterInBitmap(character)
    float x0_texture = 0.0;
    float y0_texture = 0.0;
    float x1_texture = 1.0/16.0;
    float y1_texture = 1.0/8.0;


    float vertices[] = {
        -0.5f, -0.5f, 0.0f, //  x0_texture, y0_texture,
        0.5f, -0.5f, 0.0f,  //  x1_texture, y0_texture,
        -0.5f, 0.5f, 0.0f,  //  x0_texture, y1_texture,
        0.5f, 0.5f, 0.0f,   //  x1_texture, y1_texture
    };
    float textureCoordinatesForVBO[] = {
        x0_texture, y0_texture,
        x1_texture, y0_texture,
        x0_texture, y1_texture,
        x1_texture, y1_texture
    };
    //delete[] textureCoordinates;

    glGenVertexArrays(1, &buffers[0]);
    glGenBuffers(1, &buffers[1]);
    glGenBuffers(1, &buffers[2]);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int textureVBO;
    glGenBuffers(1, &textureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);  //this atribute comes from a different vertex buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinatesForVBO), textureCoordinatesForVBO, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);



    glBindBuffer(GL_ARRAY_BUFFER, offsetTextureVBO);    

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    glVertexAttribDivisor(2, 1);    //instanced vertex attribute



    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(3, 1);    //instanced vertex attribute
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);


    /*
    unsigned int offsetTextureVBO;
    glGenBuffers(1, &offsetTextureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, offsetTextureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinatesOffset), textureCoordinatesOffset, GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    */


    glBindVertexArray(0);

    return buffers[0];
}

unsigned int loadAndReturnBitmapTexture(const std::string& bitmapName)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); 
    
    std::string dir = "./bitmapFonts/";
    std::string fullPath = dir + bitmapName;
    const char* f = fullPath.c_str();
    unsigned char *data = stbi_load(f, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}























unsigned int* getVAOs()
{
    unsigned int* VAOs = new unsigned int[2];
    unsigned int* VBOs = new unsigned int[2];
    unsigned int* EBOs = new unsigned int[2];

    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(2, EBOs);



    float vertices_1[] = {
        // positions         // colors
        -0.5f, -0.5f, 0.0f,  0.50f, 0.25f, 0.75f,  //bl
         0.0f, -0.5f, 0.0f,  0.50f, 0.25f, 0.75f,  //br
         -0.5f, 0.5f, 0.0f,  0.50f, 0.25f, 0.75f,  //tl
         0.0f, 0.5f, 0.0f, 0.50f, 0.25f, 0.75f,    //tr


         0.0f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f,  //bl
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f,  //br
         0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 1.0f,   //tl
         0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 1.0f    //tr
    };

    unsigned int indices_1[] = {  // note that we start from 0!
        0, 1, 2,  
        1, 2, 3,

        4, 5, 6,
        5, 6, 7
    };

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_1), vertices_1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_1), indices_1, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    float vertices_2[] = {
        // positions         // colors
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  //bl
         0.0f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  //br
         -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  //tl
         0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,    //tr


         0.0f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  //bl
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  //br
         0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   //tl
         0.5f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f    //tr
    };

    unsigned int indices_2[] = {  // note that we start from 0!
        0, 1, 2,  
        1, 2, 3,

        4, 5, 6,
        5, 6, 7
    };

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAOs[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_2), indices_2, GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    return VAOs;
}

std::tuple<unsigned int, int> swapRectangles(int i, float vertices[], unsigned int indices[])
{

    return std::make_tuple(10, 5);
}
