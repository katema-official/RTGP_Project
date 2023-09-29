#include "Utils.h"

#include <ft2build.h>
#include FT_FREETYPE_H 

#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <string>
#include <fstream>

#include <shader_s.h>
#include <settings.h>
#include <camera.h>
#include "Drawing/Drawing.h"

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
    //float aspect = ((float) SCR_WIDTH) / ((float) SCR_HEIGHT);
    //glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.1f, 1000.0f);
    textShader.use();
    textShader.setMat4("projection", projection);

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
            //std::cout << c << std::endl;
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	return textShader;
}

// render line of text
// -------------------
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec4 color)
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
    glUniform4f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z, color.w);
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
        float vertices[6][5] = {
            { xpos,     ypos + h, 0.0f,   0.0f, 0.0f },            
            { xpos,     ypos,     0.0f,   0.0f, 1.0f },
            { xpos + w, ypos,     0.0f,   1.0f, 1.0f },

            { xpos,     ypos + h, 0.0f,   0.0f, 0.0f },
            { xpos + w, ypos,     0.0f,   1.0f, 1.0f },
            { xpos + w, ypos + h, 0.0f,   1.0f, 0.0f }           
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



//################################### SIMILAR FUNCTIONS BUT TO RENDER TEXT IN 3D ENVIRONMENT ###################################

// render line of text in space
// ----------------------------
void RenderTextInSpace(Shader& shader, std::string text, float x, float y, float scale, glm::vec4 color)
{
	// OpenGL state (for rendering text)
	// ------------
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // activate corresponding render state	
    shader.use();
    glUniform4f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z, color.w);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_Text);

    // iterate through all characters
    std::string::const_iterator c;
    bool first = false;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos;
        float ypos;
        xpos = x + ch.Bearing.x * scale;
        ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        //std::cout << "xpos and ypos= " << xpos << ", " << ypos << std::endl;
        

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][5] = {
            { xpos,     ypos + h, 0.0f,   0.0f, 0.0f },            
            { xpos,     ypos,     0.0f,   0.0f, 1.0f },
            { xpos + w, ypos,     0.0f,   1.0f, 1.0f },

            { xpos,     ypos + h, 0.0f,   0.0f, 0.0f },
            { xpos + w, ypos,     0.0f,   1.0f, 1.0f },
            { xpos + w, ypos + h, 0.0f,   1.0f, 0.0f }           
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

glm::vec4 getColorFromID(int ID)
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
    glm::vec4 ret = glm::vec4(rgb[0], rgb[1], rgb[2], 1.0);
    delete rgb;
    return ret;
}



//##########################################################################################################

// cross platform bitmap headers
struct BitMapFileHeaderStruct
{
	short type;
	int filesize;
	short reserved1;
	short reserved2;
	int offset;
};

struct BitMapInfoHeaderStruct
{
	int infoheadersize;
	int width;
	int height;
	short planes;
	short bitsperpixel;
	int compression;
	int imagesize;
	int xpixelspermeter;
	int ypixelspermeter;
	int colorsused;
	int importantcolorsused;
};







// helper function to convert a normal buffer to a bitmap buffer
unsigned char* toBitmapBuffer( int* bufferSize , const int imageWidth , const int imageHeight , unsigned char* buffer )
{
    int rowSize = ( ( 24 * imageWidth + 31) / 32 ) * 4;
    int padding = 0;
    while ( ( imageWidth*3 + padding ) % 4 != 0 ) padding++;
    *bufferSize = rowSize * imageHeight;
    unsigned char* bitmapBuffer = new unsigned char[*bufferSize];
    memset( bitmapBuffer , 0 , *bufferSize );

    for ( int y = 0 ; y < imageHeight ; ++y )
    {
        for ( int x = 0 ; x < imageWidth ; ++x )
		{
		    // position in buffer
		    int pos = y*imageWidth*4 + x*4;
		    // position in padded bitmap buffer
			int newpos = ( imageHeight - y - 1 ) * ( imageWidth*3 + padding ) + x*3;
			bitmapBuffer[newpos+0] = buffer[pos+2]; // swap r and b
			bitmapBuffer[newpos+1] = buffer[pos+1]; // g stays
			bitmapBuffer[newpos+2] = buffer[pos+0]; // swap b and r
		}
    }
    return bitmapBuffer;
}



//adapted from https://levelup.gitconnected.com/how-to-create-a-bitmap-font-with-freetype-58e8c31878a9
std::tuple<bool, unsigned int, int*> generateBitmapFont(const std::string& fontFilename, const int fontSize, const std::string& bitmapFilename, const std::string& widthsFilename)
{

    //first check if the bitmap file already exists.
    std::string fullPath = "./bitmapFonts/" + bitmapFilename;
    std::ifstream f(fullPath.c_str());
    if(f.good())
    {
        std::cout << "Bitmap exists" << std::endl;
    }
    else
    {
        std::cout << "Bitmap does not exist" << std::endl;
    }



    FT_Library lib;
    FT_Error error;
    FT_Face face;
    FT_UInt glyphIndex;

    // init freetype
    error = FT_Init_FreeType( &lib );
    if ( error != FT_Err_Ok )
    {
        std::cout << "BitmapFontGenerator > ERROR: FT_Init_FreeType failed, error code: " << error << std::endl;
        return std::make_tuple(false, 0, nullptr);
    }

    // load font
    error = FT_New_Face( lib , fontFilename.c_str() , 0 , &face );
    if ( error == FT_Err_Unknown_File_Format )
    {
        std::cout << "BitmapFontGenerator > ERROR: failed to open file \"" << fontFilename << "\", unknown file format" << std::endl;
        return std::make_tuple(false, 0, nullptr);
    }
    else if ( error )
    {
        std::cout << "BitmapFontGenerator > ERROR: failed to open file \"" << fontFilename << "\", error code: " << error << std::endl;
        return std::make_tuple(false, 0, nullptr);
    }

    // set font size
    error = FT_Set_Pixel_Sizes( face , 0 , fontSize );
    if ( error )
    {
        std::cout << "BitmapFontGenerator > failed to set font size, error code: " << error << std::endl;
    }



    // create bitmap font
    int imageWidth = (fontSize+2)*16;
    int imageHeight = (fontSize+2)*8;

    // create a buffer for the bitmap
    unsigned char* buffer = new unsigned char[imageWidth*imageHeight*4];
    memset( buffer , 0 , imageWidth*imageHeight*4 );

    // create an array to save the character widths
    int* widths = new int[128];



    // we need to find the character that goes below the baseline by the biggest value
    int maxUnderBaseline = 0;
    for ( int i = 32 ; i < 127 ; ++i )
    {
        // get the glyph index from character code
        glyphIndex = FT_Get_Char_Index( face , i );

        // load the glyph image into the slot
        error = FT_Load_Glyph( face , glyphIndex , FT_LOAD_DEFAULT );
        if ( error )
        {
            std::cout << "BitmapFontGenerator > failed to load glyph, error code: " << error << std::endl;
        }

        // get the glyph metrics
        const FT_Glyph_Metrics& glyphMetrics = face->glyph->metrics;

        // find the character that reaches below the baseline by the biggest value
        int glyphHang = (glyphMetrics.horiBearingY-glyphMetrics.height)/64;
        if( glyphHang < maxUnderBaseline )
        {
            maxUnderBaseline = glyphHang;
        }
    }



    // draw all characters
    for ( int i = 0 ; i < 128 ; ++i )
    {
        // get the glyph index from character code
        glyphIndex = FT_Get_Char_Index( face , i );

        // load the glyph image into the slot
        error = FT_Load_Glyph( face , glyphIndex , FT_LOAD_DEFAULT );
        if ( error )
        {
            std::cout << "BitmapFontGenerator > failed to load glyph, error code: " << error << std::endl;
        }

        // convert to an anti-aliased bitmap
        error = FT_Render_Glyph( face->glyph , FT_RENDER_MODE_NORMAL );
        if ( error )
        {
            std::cout << "BitmapFontGenerator > failed to render glyph, error code: " << error << std::endl;
        }

        // save the character width
        widths[i] = face->glyph->metrics.width/64;

        // find the tile position where we have to draw the character
        int x = (i%16)*(fontSize+2);
        int y = (i/16)*(fontSize+2);
        x += 1; // 1 pixel padding from the left side of the tile
        y += (fontSize+2) - face->glyph->bitmap_top + maxUnderBaseline - 1;

        // draw the character
        const FT_Bitmap& bitmap = face->glyph->bitmap;
        for ( int xx = 0 ; xx < bitmap.width ; ++xx )
        {
            for ( int yy = 0 ; yy < bitmap.rows ; ++yy )
            {
                unsigned char r = bitmap.buffer[(yy*(bitmap.width)+xx)];
                buffer[(y+yy)*imageWidth*4+(x+xx)*4+0] = r;
                buffer[(y+yy)*imageWidth*4+(x+xx)*4+1] = r;
                buffer[(y+yy)*imageWidth*4+(x+xx)*4+2] = r;
                buffer[(y+yy)*imageWidth*4+(x+xx)*4+3] = 255;
            }
        }
    }


    //CREATE BITMAP AND WIDTH FILES
    // convert the buffer to the bitmap format
    int paddedSize = 0;
    unsigned char* bitmapBuffer = toBitmapBuffer( &paddedSize , imageWidth , imageHeight , buffer );

    BitMapFileHeaderStruct bmfh;
    bmfh.type = 0x4d42; // 0x4d42 = 'BM'
    bmfh.filesize = 14 + sizeof( BitMapInfoHeaderStruct ) + paddedSize;
    bmfh.reserved1 = 0; // not used
    bmfh.reserved2 = 0; // not used
    bmfh.offset = 54;

    BitMapInfoHeaderStruct bmih;
    bmih.infoheadersize = sizeof( BitMapInfoHeaderStruct );
    bmih.width = imageWidth;
    bmih.height = imageHeight;
    bmih.planes = 1;
    bmih.bitsperpixel = 24;
    bmih.compression = 0;
    bmih.imagesize = 0;
    bmih.xpixelspermeter = 0;
    bmih.ypixelspermeter = 0;
    bmih.colorsused = 0;
    bmih.importantcolorsused = 0;

    FILE* file;
    std::string bitmapFilenameFull = "./bitmapFonts/" + bitmapFilename;
    if ( ( file = fopen( bitmapFilenameFull.c_str() , "wb" ) ) == NULL )
    {
        std::cout << "BitmapFontGenerator > failed to save bitmap file \"" << bitmapFilename << "\"" << std::endl;
        return std::make_tuple(false, 0, nullptr);
    }

    // write file header
    fwrite( &bmfh.type , sizeof(short) , 1 , file );
    fwrite( &bmfh.filesize , sizeof(int) , 1 , file );
    fwrite( &bmfh.reserved1 , sizeof(short) , 1 , file );
    fwrite( &bmfh.reserved2 , sizeof(short) , 1 , file );
    fwrite( &bmfh.offset , sizeof(int) , 1 , file );

    // write info header
    fwrite( &bmih , sizeof(BitMapInfoHeaderStruct) , 1 , file );

    // write data
    fwrite( bitmapBuffer , sizeof(unsigned char) , paddedSize , file );

    fclose( file );
    delete[] bitmapBuffer;
    delete[] buffer;

    // save the widths file
    std::ofstream ofs;
    std::string widthsFilenameFull = "./bitmapFonts/" + widthsFilename;
    ofs.open( widthsFilenameFull );
    if ( ofs.is_open() )
    {
        for ( int i = 0 ; i < 128 ; ++i )
        {
            ofs << widths[i] << std::endl;
        }
        ofs.close();
    }
    else
    {
        std::cout << "BitmapFontGenerator > failed to save widths file \"" << bitmapFilename << "\"" << std::endl;
        return std::make_tuple(false, 0, nullptr);
    }

    //delete[] widths;

    

    // shutdown freetype
	error = FT_Done_FreeType( lib );
	if ( error != FT_Err_Ok )
	{
		std::cout << "BitmapFontGenerator > ERROR: FT_Done_FreeType failed, error code: " << error << std::endl;
		return std::make_tuple(false, 0, nullptr);
	}

    unsigned int texture = loadAndReturnBitmapTexture(bitmapFilename);
	
    
    return std::make_tuple(true, texture, widths);
}




float* getTextureCoordinatesOfCharacterInBitmap(char c)
{
    float id = c;
    int id_int = c;
    if(id_int < 0 || id_int > 127)
    {
        std::cout << "ERROR: getTextureCoordinatesOfCharacterInBitmap input not in correct range" << std::endl;
    }
    int rows = 8;
    int columns = 16;
    int rowIndex = id / columns;
    int columnIndex = id_int % columns;
    float xOffset = (1.0 / 16.0) / 1024.0;
    float yOffset = (1.0 / 8.0) / 8.0;
    float xToAdd = 1.0 / 16.0;
    float yToAdd = 1.0 / 8.0;

    float x0 = (float) columnIndex / (float) columns;

    float y0 = ((float) (rows - 1 - rowIndex)) / (float) rows;      // + yOffset;

    float x1 = x0 + xToAdd;

    float y1 = y0 + yToAdd;     // - yOffset;

    

    //std::cout << "rowIndex = " << rowIndex << " columnIndex = " << columnIndex << " x0 = " << x0 << " y0 = " << y0 << " x1 = " << x1 << " y1 = " << y1 << std::endl;

    float* textureCoordinates = new float[4];
    textureCoordinates[0] = x0;
    textureCoordinates[1] = y0;
    textureCoordinates[2] = x1;
    textureCoordinates[3] = y1; 

    //std::cout << "ID = " << id << std::endl;
    
    return textureCoordinates;
}

