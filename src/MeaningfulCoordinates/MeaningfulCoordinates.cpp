#include "MeaningfulCoordinates.h"

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
    ret.x = 0 + hContainer;
    ret.y = 0 + wContainer;

    if(winnerLength == wContainer)
    {
        float heightPortion = (hContainer * maxPortionDedicatedToContainer) / wContainer;

        ret.z = 0 + maxPortionDedicatedToContainer - hContainer;
        ret.w = 0 + heightPortion - wContainer;
    }
    else
    {
        float widthPortion = (wContainer * maxPortionDedicatedToContainer) / hContainer;
        ret.z = 0 + widthPortion - hContainer;
        ret.w = 0 + maxPortionDedicatedToContainer - wContainer;
    }

    return ret;
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