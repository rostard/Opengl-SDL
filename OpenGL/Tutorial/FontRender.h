#pragma once
#undef _sprintf

#include <freetype\ft2build.h>
#include FT_FREETYPE_H
#include <iostream> 
#include <GL\glew.h>
#include <map>
#include <string>

#include "math_3d.h"
#include "pipeline.h" 
#include "font_technique.h"
#include "engine_common.h"

struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	Vector2i Size;       // Size of glyph
	Vector2i Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

class FontRenderer {
public:
	FontRenderer() {
		
	}
	bool Init(char* FontName) {
		if (!LoadFont(FontName)) {
			return false;
		}

		m_pFontTechnique = new FontTechnique();
		if (!m_pFontTechnique->Init()) {
			std::cout << "Couldn't init fontTexhnique" << std::endl;
		}

		GLCheckError();

		Pipeline p;
		m_pFontTechnique->SetWorld(p.GetWorldTrans());
		GLCheckError();
		m_pFontTechnique->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		GLCheckError();
		GenBuffers();
		return true;
	}
	bool LoadFont(char* FontName) {
		if (FT_Init_FreeType(&ft)) {
			std::cout << "ERROR FREETYPE: Could not init library" << std::endl;
			return false;
		}

		if (FT_New_Face(ft, FontName, 0, &face)) {
			std::cout << "" << std::endl;
			return false;
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
		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}


	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, Vector3f color) {
		scale = 0.001f;
		m_pFontTechnique->Enable();
		m_pFontTechnique->SetColor(color);
		glActiveTexture(COLOR_TEXTURE_UNIT);
		glBindVertexArray(VAO);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (auto c = text.begin(); c != text.end(); c++) {
			Character ch = Characters[*c];

			GLfloat xpos = x + ch.Bearing.x * scale;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;
			// Update VBO for each character
			GLfloat Vertices[6][4] = {
				{ xpos, ypos,   0.0, 1.0 },
				{ xpos,     ypos+h,       0.0, 0.0 },
				{ xpos + w, ypos+h,       1.0, 0.0 },

				{ xpos,     ypos,   0.0, 1.0 },
				{ xpos + w, ypos + h,       1.0, 0.0 },
				{ xpos + w, ypos,   1.0, 1.0 }
			};
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
			


			glEnableVertexAttribArray(0);
			//m_pTexture->Bind(COLOR_TEXTURE_UNIT);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisableVertexAttribArray(0);




			/*glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER,0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glDisableVertexAttribArray(0);*/
			x += (ch.Advance >> 6)*scale;
		}
		glBlendFunc(GL_ONE, GL_ZERO);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D,0);
	}
private:
	void GenBuffers() {

		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		
		/*glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);*/
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	FontTechnique *m_pFontTechnique;
	FT_Library ft;
	FT_Face face;
	GLuint VAO, VBO;
	std::map<GLchar, Character> Characters;
};