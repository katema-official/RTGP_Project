#include "Utils.h"

#include <ft2build.h>
#include FT_FREETYPE_H 

#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <string>

#include <shader_s.h>
#include <settings.h>

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern int fontSize;
extern unsigned int VAO_Text, VBO_Text;


//*********************************************************************************
//******************************** TEXT RENDERING *********************************
//*********************************************************************************

extern std::map<GLchar, Character> Characters;



//to initialize all the things that allow to perform text rendering
Shader initTextRendering(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT)
{
	// compile and setup the shader for the text
    // ----------------------------
	Shader textShader("shader_text.vs", "shader_text.fs");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        exit;
    }

    FT_Face face;
    if (FT_New_Face(ft, "../fonts/ARIAL.TTF", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
        exit;
    }
    else
    {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, fontSize);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads (text)
    // -----------------------------------
    glGenVertexArrays(1, &VAO_Text);
    glGenBuffers(1, &VBO_Text);

    glBindVertexArray(VAO_Text);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Text);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	return textShader;
}

// render line of text
// -------------------
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
	if(x < 0 || x > 1 ||
		y < 0 || y > 1)
	{
        std::cout << "ERROR::RenderText: data not between 0 and 1" << std::endl;
        exit;
    }
	x = x * SCR_WIDTH;
	y = y * SCR_HEIGHT;

	// OpenGL state (for rendering text)
	// ------------
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_Text);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Text);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

//*********************************************************************************


float* HSVtoRGB(float h, float s, float v)
{
    float* rgb = new float[3];
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		rgb[0] = v;
		rgb[1] = v;
		rgb[2] = v;
		return rgb;
	}
	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch(i) {
		case 0:
			rgb[0] = v;
			rgb[1] = t;
			rgb[2] = p;
			break;
		case 1:
			rgb[0] = q;
			rgb[1] = v;
			rgb[2] = p;
			break;
		case 2:
			rgb[0] = p;
			rgb[1] = v;
			rgb[2] = t;
			break;
		case 3:
			rgb[0] = p;
			rgb[1] = q;
			rgb[2] = v;
			break;
		case 4:
			rgb[0] = t;
			rgb[1] = p;
			rgb[2] = v;
			break;
		default:		// case 5:
			rgb[0] = v;
			rgb[1] = p;
			rgb[2] = q;
			break;
	}

    return rgb;
}

glm::vec3 getColorFromID(int ID)
{
	int H = (49 * ID) % 360;
	float offset = 1.0f / ((float) ID + 1);

	//std::cout << "offset = " << offset << std::endl;

	while(offset > 0.5f)
	{
		offset -= 0.5f;
	}
	float S = 0.50f + offset;

	offset *= 0.2f;
	while(offset > 0.5f)
	{
		offset -= 0.5f;
	}
	float V = 1.0f - offset;

	float* rgb = HSVtoRGB(H, S, V);
    glm::vec3 ret = glm::vec3(rgb[0], rgb[1], rgb[2]);
    delete rgb;
    return ret;
}


