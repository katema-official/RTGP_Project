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

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern int fontSize;
extern std::map<GLchar, Character> Characters;
extern float scalingQuantitiesText;

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


    glm::vec3 containerColor = glm::vec3(0.0, 0.0, 0.0);

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
    glm::vec2 nodeIdCoords = getNodeId_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 fatherIdCoords = getFatherId_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 levelInTreeCoords = getLevelInTree_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 PBCoords = getPB_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 DBCoords = getDB_STATIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 bestPBCoords = getBestPB_STATIC_Coordinates(maxPortionDedicatedToContainer);
    RenderText(textShader, "Node ID: ", nodeIdCoords.x, nodeIdCoords.y, rescaleTextFactor, containerColor);
    RenderText(textShader, "Father ID: ", fatherIdCoords.x, fatherIdCoords.y, rescaleTextFactor, containerColor);
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



void drawBoxShape(Shader& shader, unsigned int* buffers, float x0, float y0, float x1, float y1, glm::vec3 color)
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





void drawTreeNode_v1(TreeNode* treeNode, unsigned int* boxBuffers,
                    int wC, int hC,
                    float wContainer, float hContainer, 
                    float wThickness, float hThickness, 
                    float maxPortionDedicatedToContainer,
                    Shader& boxShader, Shader& textShader)
{
    float maxDim = wC > hC ? wC : hC;
    float lengthSingleDottedLine = (5 * maxDim) / 250;
    float thicknessProjection = (1 * maxDim) / 250;

    glm::vec2 coordsNodeID = getNodeId_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsFatherID = getFatherId_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsLevelInTree = getLevelInTree_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsPB = getPB_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsDB = getDB_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    glm::vec2 coordsBestPB = getBestPB_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);


    glm::vec3 textColor = glm::vec3(0.0, 0.0, 0.0);
    float rescaleTextInfo = 0.125;

    RenderText(textShader, std::to_string(treeNode->nodeID), coordsNodeID.x, coordsNodeID.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->fatherID), coordsFatherID.x, coordsFatherID.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->level), coordsLevelInTree.x, coordsLevelInTree.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->PB), coordsPB.x, coordsPB.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->DB), coordsDB.x, coordsDB.y, rescaleTextInfo, textColor);
    RenderText(textShader, std::to_string(treeNode->bestPB), coordsBestPB.x, coordsBestPB.y, rescaleTextInfo, textColor);

    glm::vec4 coordsContainer = getLowerLeftAndUpperRightCoordinatesOfContainer(wContainer, hContainer, wThickness, hThickness, maxPortionDedicatedToContainer);

    
    float rescaleTextID = 0.25;
    for(int i = 0; i < treeNode->nBoxes; i++)   //treeNode->nBoxes
    {
        Box* thisBox = treeNode->boxes[i];

        glm::vec4 coords = fromInputBoxToRelativeCoordinates(thisBox, wC, hC, wContainer, hContainer, wThickness, hThickness, maxPortionDedicatedToContainer);
        int ID = thisBox->ID;

        drawBoxShape(boxShader, boxBuffers, coords.x, coords.y, coords.z, coords.w, getColorFromID(ID));

        float minDim = thisBox->xlen < thisBox->ylen ? thisBox->xlen : thisBox->ylen;
        
        float xDiff = coords.z - coords.x;
        float yDiff = coords.w - coords.y;
        float xText = coords.x + (xDiff / 3.0);
        float yText = coords.y + (yDiff / 3.0);

        RenderText(textShader, std::to_string(ID), xText, yText, rescaleTextID, textColor);
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

    float wAdd = Characters[':'].Size.x * scalingQuantitiesText + 0.003;
    for(int i : treeNode->remainingQuantities)
    {
        int q = i;
        int ID_len = std::to_string(index).length();
        std::string actualText = std::to_string(index) + ": " + std::to_string(q);
        drawBoxShape(boxShader, boxBuffers, x, y, x + (((w + wAdd) * ID_len) / SCR_WIDTH), y + (h / SCR_HEIGHT), getColorFromID(index));
        RenderText(textShader, actualText, x, y, scalingQuantitiesText, textColor);
        index++;
        y -= (h / SCR_HEIGHT) * 1.3;
    }

    /*
    Character ch = Characters['a'];
    float h = ch.Size.y * scalingQuantitiesText;
    std::cout << "h = " << h << std::endl;
    glm::vec2 quantitiesInitialCoords = getRemainingQuantities_DYNAMIC_Coordinates(maxPortionDedicatedToContainer);
    RenderText(textShader, "0: ", quantitiesInitialCoords.x, quantitiesInitialCoords.y, scalingQuantitiesText, textColor);

    float x = quantitiesInitialCoords.x * SCR_WIDTH;
    float y = quantitiesInitialCoords.y * SCR_HEIGHT;
    float xpos = x + ch.Bearing.x * scalingQuantitiesText;
    float ypos = y - (ch.Size.y - ch.Bearing.y) * scalingQuantitiesText;

    float w = ch.Size.x * scalingQuantitiesText;
    h = ch.Size.y * scalingQuantitiesText;

    std::cout << "x = " << xpos << ", y = " << ypos << ", w = " << w << ", h = " << h << std::endl;
    */

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
