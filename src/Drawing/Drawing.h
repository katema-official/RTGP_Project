#pragma once
#include <tuple>

#include <shader_s.h>
#include "../classes/TreeNode.h"

#include <glm/glm.hpp>
#include <camera.h>

//to get the dimension of the container such that it will respect the aspect ratio of the application
glm::vec2 getContainerEffectiveDimensions(int wContainer, int hContainer, float maxPortionDedicatedToContainer);

//to draw informations regarding the instance considered that basically never change, such as
//the container walls and some text
void drawStaticInformations(int wC, int hC,
                            float wContainer, float hContainer, 
                            float wThickness, float hThickness,
                            float maxPortionDedicatedToContainer,
                            Shader boxShader, unsigned int* buffersForBox,
                            Shader textShader, std::vector<Box*> obstaclesVector
                            );

//to get the VAO, VBO and EBO to draw a single box
unsigned int* getBuffersToDrawBoxShape();

//to draw a box given the bottom-left vertex and the upper-right one.
//x0, y0, x1, y1 are always expressed between 0 and 1.
void drawBoxShape(Shader& shader, unsigned int* buffers, float x0, float y0, float x1, float y1, glm::vec4 color);

//to draw a node of the tree
void drawNode_v1(TreeNode* treeNode, unsigned int* boxBuffers,
                    int wC, int hC,
                    float wContainer, float hContainer, 
                    float wThickness, float hThickness, 
                    float maxPortionDedicatedToContainer,
                    Shader& boxShader, Shader& textShader);

//to temporarily draw the new "jump" (speed) factor betwee nodes (1x, 10x, 100x...)
void drawSpeed(Shader& shader, int speed, bool updated, float _initialTimeNewSpeed, float currentTime);







//to get the VAO, VBO and EBO (where the VBO is already filled with the four vertices)
//to draw a square, that will represent a node in the tree representation
unsigned int* getBuffersWithDataToDrawRectangleNode();

//to draw a node in the tree as a rectangle in a 3D space with a text on it
void drawNodeInTree(Shader& rectangleShader, Shader& textShader, unsigned int* buffersRectangle, Camera camera, glm::vec3 position);




unsigned int* getVAOs();

std::tuple<unsigned int, int> swapRectangles(int i, float vertices[], unsigned int indices[]);
