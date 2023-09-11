#pragma once

#include <glm/glm.hpp>

#include "../classes/Box.h"
#include "../classes/Projection.h"

#include <vector>

//to get the x0, y0, x1, y1 coordinates that represent the lower left and upper right point
//of the container of this instance. We will store these coordinates in a vec4
glm::vec4 getLowerLeftAndUpperRightCoordinatesOfContainer(float wContainer, float hContainer, 
                                                        float wThickness, float hThickness,
                                                        float maxPortionDedicatedToContainer);

//to get the bottomLeft and upperRight coordinates of a box expressed in a coordinate system relative to
//the container built, given an "inputBox" with integer x0, y0, xlen, ylen
glm::vec4 fromInputBoxToRelativeCoordinates(Box* inputBox, 
                                                        int wC, int hC,
                                                        float wContainer, float hContainer, 
                                                        float wThickness, float hThickness,
                                                        float maxPortionDedicatedToContainer);

//from a projection, get a vector of int[4], where each of them represents the x0, y0, x1, y1 coordinates
//of small rectangles that will be used to represent the projection
std::vector<glm::vec4> getProjectionAsDottedLine(Projection* proj,
                                            int wC, int hC,
                                            float wContainer, float hContainer, 
                                            float wThickness, float hThickness,
                                            float maxPortionDedicatedToContainer,
                                            int lengthSingleDottedLine,
                                            float thicknessProjection);

//functions to obtain the x, y coordinates of the static lines of text, like "Node ID", "Father ID"...
//In this way, if one wants to modify them, it only has to touch these functions
glm::vec2 getNodeId_STATIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getFatherId_STATIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getLevelInTree_STATIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getPB_STATIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getDB_STATIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getBestPB_STATIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec4 getContainerDimensions_STATIC_Coordinates(float maxPortionDedicatedToContainer);  //vec4 because the first two coordinates are for "Container", the latter two for "dimensions: "

//functions to obtain the x,y coordinates of the dynamic lines of text.
glm::vec2 getNodeId_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getFatherId_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getLevelInTree_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getPB_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getDB_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getBestPB_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer);
glm::vec2 getContainerDimensions_DYNAMIC_Coordinates(float maxPortionDedicatedToContainer);


