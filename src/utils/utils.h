#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_s.h>

Shader initTextRendering(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT);

void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec4 color);




void RenderTextInSpace(Shader& shader, std::string text, float x, float y, float scale, glm::vec4 color);



//function that converts HSV to RGB.
//H ranges between 0 to 360. s and v range from 0 to 1.
//Based on: https://www.cs.rit.edu/~ncs/color/t_convert.html 
float* HSVtoRGB(float h, float s, float v);

//function that generates an unique color from an integer ID
glm::vec4 getColorFromID(int ID);



//###########################################################################################

//to load a bitmap font in a 2D texture. Returns:
//<false, ...> if there was an error in generating the texture
//<true, id> if the texture, of id "id", was correctly created
std::tuple<bool, unsigned int, int*> generateBitmapFont(const std::string& fontFilename, const int fontSize, const std::string& bitmapFilename, const std::string& widthsFilename);


float* getTextureCoordinatesOfCharacterInBitmap(char c);


