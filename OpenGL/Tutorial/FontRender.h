#pragma once
#include <freetype\ft2build.h>
#include <iostream> 
#include <GL\glew.h>
#include <map>
#include "math_3d.h"
#include FT_FREETYPE_H
struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	Vector2i Size;       // Size of glyph
	Vector2i Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

class FontRenderer {
public:
	FontRenderer(char* FontName) {
		if (FT_Init_FreeType(&ft)) {
			std::cout << "ERROR FREETYPE: Could not init library" << std::endl;
		}
		if (FT_New_Face(ft,FontName,0,&face)) {
			std::cout << "" << std::endl;
		}
		FT_Set_Pixel_Sizes(face, 0, 48);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for (GLubyte c = 0; c < 128; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cout << "ERROR FREETYPE: Could not load Glyph" << std::endl;
				continue;
			}
			//Generate textures
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//store character
			Character ch = {
				texture,
				Vector2i(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				Vector2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<GLchar, Character>(c, ch));
		}
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	 }
private:
	FT_Library ft;
	FT_Face face;
	std::map<GLchar, Character> Characters;
};