#pragma once

#include <glm/glm.hpp>

//to get the x0, y0, x1, y1 coordinates that represent the lower left and upper right point
//of the container of this instance. We will store these coordinates in a vec4
glm::vec4 getLowerLeftAndUpperRightCoordinatesOfContainer(int wContainer, int hContainer, 
                                                        float wThickness, float hThickness,
                                                        float maxPortionDedicatedToContainer);









