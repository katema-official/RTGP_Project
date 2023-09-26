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



void drawNodeInTree(Shader& rectangleShader, Shader& textShader, unsigned int* buffersRectangle, Camera camera, glm::vec3 position, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, TreeNode* node)
{
    rectangleShader.use();
    
    glm::vec3 nodePosition = position;
    glm::mat4 modelNode = glm::mat4(1.0f);
    modelNode = glm::translate(modelNode, nodePosition);
    rectangleShader.setMat4("model", modelNode);
    
    glBindVertexArray(buffersRectangle[0]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



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

void drawWholeTree(TreeNode* node, std::vector<TreeNode*> nodesVector, Shader& nodeInTreeShader, Shader& textShaderInSpace, unsigned int* buffersForNodeInTree, Camera camera, glm::mat4 view, glm::mat4 projection)
{
    if(node->explorationID == 0)
    {
        //otherwise at each frame the nodes would be rendered more and more to the right
        coordinatesNewNodeToDraw = glm::vec3(0.0, 0.0, -100.0);
        nodeInTreeShader.use();
        nodeInTreeShader.setVec4("color", colorNormalNode);   //to call this just one time
    }

    
    if(node->explorationID == globalOptimumID)
    {
        nodeInTreeShader.use();
        nodeInTreeShader.setVec4("color", colorOptimumNode);
    }

    //draw the current node. The x and y coordinates are given by the global variable "coordinatesNewNodeToDraw", while the y is given by the level of the node in the tree
    glm::vec3 posNode(coordinatesNewNodeToDraw.x, -1.0 * node->level * verticalSpaceBetweenLevels, coordinatesNewNodeToDraw.z);
    drawNodeInTree(nodeInTreeShader, textShaderInSpace, buffersForNodeInTree, camera, posNode, view, projection, node);

    if(node->explorationID == globalOptimumID)  //to return to the original color once the optimum has been colored differently
    {   
        nodeInTreeShader.use();
        nodeInTreeShader.setVec4("color", colorNormalNode);
    }

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
                //posChild.x += horizontalSpaceBetweenNodes;
            }

            glm::vec3 posChild(coordinatesNewNodeToDraw.x, -1.0 * (node->level + 1) * verticalSpaceBetweenLevels, posNode.z);
            //if(node->explorationID == 0) std::cout << posChild.x << "," << posChild.y << "," << posChild.z << std::endl;

            //draw the child
            drawWholeTree(child, nodesVector, nodeInTreeShader, textShaderInSpace, buffersForNodeInTree, camera, view, projection);

            //now draw the vertical bridge between the current node and the child node
            if(currentChildLocalIndex == 0)
            {
                //for the first child, a full bridge
                nodeInTreeShader.use();
    
                glm::vec3 verticalBridgePos = glm::vec3(posNode.x, posChild.y + (verticalSpaceBetweenLevels / 2.0f), posNode.z - 10.0f);
                glm::mat4 model = glm::mat4(1.0f);
                
                model = glm::translate(model, verticalBridgePos);
                model = glm::scale(model, glm::vec3(0.25, verticalSpaceBetweenLevels, 1.0f));

                nodeInTreeShader.setMat4("model", model);
                //if(node->explorationID == 0) std::cout << verticalBridgePos.x << "," << verticalBridgePos.y << "," << verticalBridgePos.z << std::endl;
                
                glBindVertexArray(buffersForNodeInTree[0]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            else
            {
                //for the others, half a bridge
                nodeInTreeShader.use();

                glm::vec3 halfVerticalBridgePos = glm::vec3(posChild.x, posChild.y + (verticalSpaceBetweenLevels / 4.0f), posNode.z - 10.0f);
                glm::mat4 model = glm::mat4(1.0f);

                model = glm::translate(model, halfVerticalBridgePos);
                model = glm::scale(model, glm::vec3(0.25, verticalSpaceBetweenLevels / 2.0f, 1.0f));

                nodeInTreeShader.setMat4("model", model);
                //if(node->explorationID == 0) std::cout << halfVerticalBridgePos.x << "," << halfVerticalBridgePos.y << "," << halfVerticalBridgePos.z << std::endl;

                glBindVertexArray(buffersForNodeInTree[0]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            xLastChild = posChild.x;

            currentChildLocalIndex++;
        }

        //now that all the child have been drawn, we can create the horizontal bridge
        glm::vec3 posLastChild(xLastChild, -1.0 * (node->level + 1) * verticalSpaceBetweenLevels, posNode.z);

        nodeInTreeShader.use();

        glm::vec3 horizontalBridgePos = glm::vec3(glm::mix(posNode.x, posLastChild.x, 0.5f), posNode.y - (verticalSpaceBetweenLevels) / 2.0f, posNode.z - 10.0f);
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, horizontalBridgePos);
        model = glm::scale(model, glm::vec3(posLastChild.x - posNode.x, 0.25, 1.0f));

        nodeInTreeShader.setMat4("model", model);

        glBindVertexArray(buffersForNodeInTree[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
    }

    

    //drawNodeInTree(nodeInTreeShader, textShaderInSpace, buffersForNodeInTree, camera, glm::vec3(0.0, 0.0, -100.0), view, projection, 0);
    //drawNodeInTree(nodeInTreeShader, textShaderInSpace, buffersForNodeInTree, camera, glm::vec3(10.0, 5.0, -100.0), view, projection, 1);

}






//actually we also add the colors
void addModelMatrix_Nodes(std::vector<glm::mat4>& modelVector, std::vector<glm::vec4>& colorVector, std::vector<int>& indicesVector, TreeNode* node, std::vector<TreeNode*> nodesVector)
{
    indicesVector.push_back(node->explorationID);

    if(node->explorationID == 0)
    {
        coordinatesNewNodeToDraw = glm::vec3(0.0, 0.0, -100.0);
    }

    //the model matrix of a node just needs to be translated
    glm::vec3 posNode(coordinatesNewNodeToDraw.x, -1.0 * node->level * verticalSpaceBetweenLevels, coordinatesNewNodeToDraw.z);
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
            addModelMatrix_Nodes(modelVector, colorVector, indicesVector, child, nodesVector);

            currentChildLocalIndex++;
        }
    }
}

void addModelMatrix_Bridges(TreeNode* node, std::vector<TreeNode*> nodesVector, Camera camera)
{

}


//let's generate the transformation matrices
unsigned int getVAOWithDataToDrawNodesInTree(std::vector<int>& modelIndices, std::vector<TreeNode*> nodesVector)
{
    //first, we get the model matrices
    std::vector<glm::mat4> modelMatrices;
    std::vector<glm::vec4> colors;

    addModelMatrix_Nodes(modelMatrices, colors, modelIndices, nodesVector.at(0), nodesVector);

    int nNodes = nodesVector.size();
    glm::mat4* modelMatricesArray = new glm::mat4[nNodes];
    copy(modelMatrices.begin(), modelMatrices.end(), modelMatricesArray);

    glm::vec4* colorsArray = new glm::vec4[nNodes];
    copy(colors.begin(), colors.end(), colorsArray);


    unsigned int modelVBO;
    glGenBuffers(1, &modelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    glBufferData(GL_ARRAY_BUFFER, nodesVector.size() * sizeof(glm::mat4), &modelMatricesArray[0], GL_STATIC_DRAW);

    unsigned int colorVBO;
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, nodesVector.size() * sizeof(glm::vec4), &colorsArray[0], GL_STATIC_DRAW);



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




void drawAllNodesInTree(std::vector<TreeNode*> originalNodesVector, std::vector<int> recursiveNodesVector, Shader& nodeInTreeShader, unsigned int VAO, Camera camera, glm::mat4 view, glm::mat4 projection)
{
    int amount = recursiveNodesVector.size();

    nodeInTreeShader.use();
    nodeInTreeShader.setMat4("projection", projection);
    nodeInTreeShader.setMat4("view", view);

    glBindVertexArray(VAO);

    //for(int i = 0; i < recursiveNodesVector.size(); i++)
    //{
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, amount);
    //}



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
