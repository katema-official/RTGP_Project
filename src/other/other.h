#pragma once
#include <tuple>

#include <shader_s.h>

#include <glm/glm.hpp>



//to draw informations regarding the instance considered that basically never change, such as
//the container walls and some text
void drawStaticInformations();

//to get the VAO, VBO and EBO to draw a single box
unsigned int* getBuffersToDrawBoxShape();

//to draw a box given the bottom-left vertex and the upper-right one.
//x0, y0, x1, y1 are always expressed between 0 and 1.
void drawBoxShape(Shader& shader, unsigned int* buffers, float x0, float y0, float x1, float y1, glm::vec3 color);



void MyFunc();

unsigned int* getVAOs();

std::tuple<unsigned int, int> swapRectangles(int i, float vertices[], unsigned int indices[]);
