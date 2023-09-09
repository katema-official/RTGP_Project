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




