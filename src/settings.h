#pragma once

#include <map>

inline int fontSize = 192;
inline unsigned int VAO_Text;
inline unsigned int VBO_Text;
inline unsigned int SCR_WIDTH = 1280;
inline unsigned int SCR_HEIGHT = 720;

inline float scalingQuantitiesText = 0.15;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};
inline std::map<GLchar, Character> Characters;






