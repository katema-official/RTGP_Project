#include "MeaningfulCoordinates.h"

#include "../classes/Box.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <iostream>


glm::vec4 getLowerLeftAndUpperRightCoordinatesOfContainer(int wContainer, int hContainer, 
                                                        float wThickness, float hThickness,
                                                        float maxPortionDedicatedToContainer)
{
    if(wThickness > 1 || wThickness <= 0 || 
        hThickness > 1 || hThickness <= 0 || 
        maxPortionDedicatedToContainer > 1 || maxPortionDedicatedToContainer <= 0)
    {
        std::cout << "ERROR::getLowerLeftAndUpperRightCoordinatesOfContainer: data not between 0 and 1" << std::endl;
        exit;
    }

    int winnerLength = wContainer > hContainer ? wContainer : hContainer;
    int loserLength =  wContainer > hContainer ? hContainer : wContainer;

    glm::vec4 ret;
    ret.x = 0 + hThickness;
    ret.y = 0 + wThickness;

    if(winnerLength == wContainer)
    {
        float heightPortion = (hContainer * maxPortionDedicatedToContainer) / wContainer;

        ret.z = 0 + maxPortionDedicatedToContainer - hThickness;
        ret.w = 0 + heightPortion - wThickness;
    }
    else
    {
        float widthPortion = (wContainer * maxPortionDedicatedToContainer) / hContainer;
        ret.z = 0 + widthPortion - hThickness;
        ret.w = 0 + maxPortionDedicatedToContainer - wThickness;
    }

    return ret;
}

glm::vec4 fromInputBoxToRelativeCoordinates(Box* inputBox, int wContainer, int hContainer, 
                                            float wThickness, float hThickness,
                                            float maxPortionDedicatedToContainer)
{
    glm::vec4 coordsContainer = getLowerLeftAndUpperRightCoordinatesOfContainer(wContainer, hContainer, wThickness, hThickness, maxPortionDedicatedToContainer);
    float x0Box = inputBox->x0;
    float y0Box = inputBox->y0;
    float x1Box = x0Box + inputBox->xlen;
    float y1Box = y0Box + inputBox->ylen;
    float x0 = (x0Box * 1.0) / wContainer;
    float y0 = (y0Box * 1.0) / hContainer;
    float x1 = (x1Box * 1.0) / wContainer;
    float y1 = (y1Box * 1.0) / hContainer;
    float wDiff = coordsContainer.z - coordsContainer.x;
    float hDiff = coordsContainer.w - coordsContainer.y;
    x0 = (x0 * wDiff) / 1.0;
    y0 = (y0 * hDiff) / 1.0;
    x1 = (x1 * wDiff) / 1.0;
    y1 = (y1 * hDiff) / 1.0;
    x0 += coordsContainer.x;
    y0 += coordsContainer.y;
    x1 += coordsContainer.x;
    y1 += coordsContainer.y;
    return glm::vec4(x0, y0, x1, y1);

}

glm::vec2 getNodeId_STATIC_Coordinates(float maxPortionDedicatedToContainer)
{
    float remainingVerticalSpace = 1.0f - maxPortionDedicatedToContainer;
    float yOffset_1 = remainingVerticalSpace / 7;
    float xOffset_1 = 0.02;
    return glm::vec2(xOffset_1, 1.0f - yOffset_1*2);
}

glm::vec2 getFatherId_STATIC_Coordinates(float maxPortionDedicatedToContainer)
{
    float remainingVerticalSpace = 1.0f - maxPortionDedicatedToContainer;
    float yOffset_1 = remainingVerticalSpace / 7;
    float xOffset_1 = 0.02;
    return glm::vec2(xOffset_1, 1.0f - yOffset_1*4);
}

glm::vec2 getLevelInTree_STATIC_Coordinates(float maxPortionDedicatedToContainer)
{
    float remainingVerticalSpace = 1.0f - maxPortionDedicatedToContainer;
    float yOffset_1 = remainingVerticalSpace / 7;
    float xOffset_1 = 0.02;
    return glm::vec2(xOffset_1, 1.0f - yOffset_1*6);
}

glm::vec2 getPB_STATIC_Coordinates(float maxPortionDedicatedToContainer)
{
    float remainingVerticalSpace = 1.0f - maxPortionDedicatedToContainer;
    float yOffset_1 = remainingVerticalSpace / 7;
    float xOffset_2 = 0.3;
    return glm::vec2(xOffset_2, 1.0f - yOffset_1*2);
}

glm::vec2 getDB_STATIC_Coordinates(float maxPortionDedicatedToContainer)
{
    float remainingVerticalSpace = 1.0f - maxPortionDedicatedToContainer;
    float yOffset_1 = remainingVerticalSpace / 7;
    float xOffset_2 = 0.3;
    return glm::vec2(xOffset_2, 1.0f - yOffset_1*4);
}

glm::vec4 getContainerDimensions_STATIC_Coordinates(float maxPortionDedicatedToContainer)
{
    float verticalSpaceOnTheRight = 0.3;
    float yOffset_2 = verticalSpaceOnTheRight / 5;
    float xOffset_3 = maxPortionDedicatedToContainer + 0.01;
    return glm::vec4(xOffset_3, 0 + yOffset_2 * 4, xOffset_3, 0 + 0 + yOffset_2 * 4 - 0.04);
}



glm::vec2 getNodeId_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer)
{
    glm::vec2 ret = getNodeId_STATIC_Coordinates(maxPortionDedicatedToContainer);
    ret.x += 0.12;
    return ret;
}

glm::vec2 getFatherId_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer)
{
    glm::vec2 ret = getFatherId_STATIC_Coordinates(maxPortionDedicatedToContainer);
    ret.x += 0.12;
    return ret;
}

glm::vec2 getLevelInTree_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer)
{
    glm::vec2 ret = getLevelInTree_STATIC_Coordinates(maxPortionDedicatedToContainer);
    ret.x += 0.12;
    return ret;
}

glm::vec2 getPB_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer)
{
    glm::vec2 ret = getPB_STATIC_Coordinates(maxPortionDedicatedToContainer);
    ret.x += 0.04;
    return ret;
}

glm::vec2 getDB_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer)
{
    glm::vec2 ret = getDB_STATIC_Coordinates(maxPortionDedicatedToContainer);
    ret.x += 0.04;
    return ret;
}

glm::vec2 getContainerDimensions_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer)
{
    float verticalSpaceOnTheRight = 0.3;
    float yOffset_2 = verticalSpaceOnTheRight / 5;
    float xOffset_3 = maxPortionDedicatedToContainer + 0.01;
    return glm::vec2(xOffset_3, 0 + yOffset_2 * 2);
}