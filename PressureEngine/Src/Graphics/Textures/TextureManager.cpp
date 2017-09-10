//**********************************************
//Singleton Texture Manager class
//Written by Ben English
//benjamin.english@oit.edu
//
//Modified by me >:)
//
//For use with OpenGL and the FreeImage library
//**********************************************

#include "TextureManager.h"

#define PRESSURE_CUBE_MAP 0x8513
#define PRESSURE_CUBE_MAP_POS_X 0x8515

namespace Pressure {

	TextureManager* TextureManager::m_inst(0);

	TextureManager* TextureManager::Inst()
	{
		if (!m_inst)
			m_inst = new TextureManager();

		return m_inst;
	}

	TextureManager::TextureManager()
	{
		// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
		FreeImage_Initialise();
#endif
	}

	//these should never be called
	//TextureManager::TextureManager(const TextureManager& tm){}
	//TextureManager& TextureManager::operator=(const TextureManager& tm){}

	TextureManager::~TextureManager()
	{
		// call this ONLY when linking with FreeImage as a static library
#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
#endif

		UnloadAllTextures();
		m_inst = 0;
	}

	bool TextureManager::LoadTexture(const char* filename, const unsigned int texID, GLenum image_format, GLint internal_format, GLint level, GLint border)
	{
		//image format
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		//pointer to the image, once loaded
		FIBITMAP *dib(0);
		//pointer to the image data
		BYTE* bits(0);
		//image width and height
		unsigned int width(0), height(0), bpp(0);
		//OpenGL's image ID to map to
		GLuint gl_texID;

		//check the file signature and deduce its format
		fif = FreeImage_GetFileType(filename, 0);
		//if still unknown, try to guess the file format from the file extension
		if (fif == FIF_UNKNOWN)
			fif = FreeImage_GetFIFFromFilename(filename);
		//if still unkown, return failure
		if (fif == FIF_UNKNOWN)
			return false;

		//check that the plugin has reading capabilities and load the file
		if (FreeImage_FIFSupportsReading(fif))
			dib = FreeImage_Load(fif, filename);
		//if the image failed to load, return failure
		if (!dib)
			return false;

		//flips image vertically
		FreeImage_FlipVertical(dib);

		//retrieve the image data
		bits = FreeImage_GetBits(dib);
		//get the image width and height
		width = FreeImage_GetWidth(dib);
		height = FreeImage_GetHeight(dib);
		bpp = FreeImage_GetBPP(dib);
		//if this somehow one of these failed (they shouldn't), return failure
		if ((bits == 0) || (width == 0) || (height == 0) || (bpp == 0))
			return false;

		if (bpp != 24) {
			if (bpp == 32) {
				image_format = GL_BGRA_EXT;
				internal_format = GL_RGBA;
			}
			else return false;
		}

		//if this texture ID is in use, unload the current texture
		if (m_texID.find(texID) != m_texID.end())
			glDeleteTextures(1, &(m_texID[texID]));

		//generate an OpenGL texture ID for this texture
		glGenTextures(1, &gl_texID);
		//store the texture ID mapping
		m_texID[texID] = gl_texID;
		//bind to the new texture ID
		glBindTexture(GL_TEXTURE_2D, gl_texID);
		//store the texture data for OpenGL use
		glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height,
			border, image_format, GL_UNSIGNED_BYTE, (GLvoid*)bits);

		//unbind the texture.
		glBindTexture(GL_TEXTURE_2D, NULL);

		//Free FreeImage's copy of the data
		FreeImage_Unload(dib);

		//return success
		return true;
	}

	bool TextureManager::UnloadTexture(const unsigned int texID)
	{
		bool result(true);
		//if this texture ID mapped, unload it's texture, and remove it from the map
		if (m_texID.find(texID) != m_texID.end())
		{
			glDeleteTextures(1, &(m_texID[texID]));
			m_texID.erase(texID);
		}
		//otherwise, unload failed
		else
		{
			result = false;
		}

		return result;
	}

	bool TextureManager::LoadCubeMap(std::vector<std::string> files, const unsigned int texID, GLenum image_format, GLint internal_format, GLint level, GLint border)
	{
		//image format
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		//pointer to the image, once loaded
		FIBITMAP *dib(0);
		//pointer to the image data
		BYTE* bits(0);
		//image width and height
		unsigned int width(0), height(0), bpp(0);
		//OpenGL's image ID to map to
		GLuint gl_texID;

		//if this texture ID is in use, unload the current texture
		if (m_texID.find(texID) != m_texID.end())
			glDeleteTextures(1, &(m_texID[texID]));

		//generate an OpenGL texture ID for this texture
		glGenTextures(1, &gl_texID);
		//store the texture ID mapping
		m_texID[texID] = gl_texID;

		//bind to the new texture ID
		glBindTexture(PRESSURE_CUBE_MAP, gl_texID);

		for (unsigned int i = 0; i < files.size(); i++) {
			const char* file = files[i].c_str();
			//check the file signature and deduce its format
			fif = FreeImage_GetFileType(file, 0);
			//if still unknown, try to guess the file format from the file extension
			if (fif == FIF_UNKNOWN)
				fif = FreeImage_GetFIFFromFilename(file);
			//if still unkown, return failure
			if (fif == FIF_UNKNOWN)
				return false;

			//check that the plugin has reading capabilities and load the file
			if (FreeImage_FIFSupportsReading(fif))
				dib = FreeImage_Load(fif, file);
			//if the image failed to load, return failure
			if (!dib)
				return false;

			//flips image vertically
			FreeImage_FlipVertical(dib);

			//retrieve the image data
			bits = FreeImage_GetBits(dib);
			//get the image width and height
			width = FreeImage_GetWidth(dib);
			height = FreeImage_GetHeight(dib);
			bpp = FreeImage_GetBPP(dib);
			//if this somehow one of these failed (they shouldn't), return failure
			if ((bits == 0) || (width == 0) || (height == 0) || (bpp == 0))
				return false;

			if (bpp != 24) {
				if (bpp == 32) {
					image_format = GL_BGRA_EXT;
					internal_format = GL_RGBA;
				}
				else return false;
			}
			//store the texture data for OpenGL use
			glTexImage2D(PRESSURE_CUBE_MAP_POS_X + i, level, internal_format, width, height,
				border, image_format, GL_UNSIGNED_BYTE, (GLvoid*)bits);

			//Free FreeImage's copy of the data
			FreeImage_Unload(dib);

		}

		//unbind the texture.
		glBindTexture(PRESSURE_CUBE_MAP, NULL);

		//return success
		return true;
	}


	bool TextureManager::BindTexture(const unsigned int texID, GLint target)
	{
		bool result(true);
		//if this texture ID mapped, bind it's texture as current
		if (m_texID.find(texID) != m_texID.end())
			glBindTexture(target, m_texID[texID]);
		//otherwise, binding failed
		else
			result = false;

		return result;
	}

	void TextureManager::UnbindTexture() {
		glBindTexture(GL_TEXTURE_2D, NULL);
	}

	void TextureManager::UnloadAllTextures()
	{
		//start at the begginning of the texture map
		std::map<unsigned int, GLuint>::iterator i = m_texID.begin();

		//Unload the textures untill the end of the texture map is found
		while (i != m_texID.end()) {
			UnloadTexture(i++->first);
		}

		//clear the texture map
		m_texID.clear();
	}

}