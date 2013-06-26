/*
 tdogl::Texture
 
 Copyright 2012 Thomas Dalling - http://tomdalling.com/
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#pragma once

#include <GL/glew.h>

namespace tdogl {
	
	/**
	 Represents an OpenGL texture
	 */
	class Texture {
	public:
		enum Format { Grayscale, GrayscaleAlpha, RGB, RGBA, BGR, BGRA };

		/**
		 Creates a texture from a bitmap.
		 
		 @param format The format of the texture data
		 @param width The width in pixels of the texture data
		 @param height The height in pixels of the texture data
		 @param pixels The image pixel data
		 @param minMagFilter  GL_NEAREST or GL_LINEAR
		 @param wrapMode GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, or GL_CLAMP_TO_BORDER
		 */
		Texture(const Format& format,
				const GLsizei width,
				const GLsizei height,
				const unsigned char *pixels,
				GLint minMagFilter = GL_LINEAR,
				GLint wrapMode = GL_CLAMP_TO_EDGE);
		
		/**
		 Deletes the texture object with glDeleteTextures
		 */
		~Texture();

		/**
		 @result The texture format
		 */
		Format format() const;
		
		/**
		 @result The texture object, as created by glGenTextures
		 */
		GLuint object() const;
		
		/**
		 @result The original width (in pixels) of the bitmap this texture was made from
		 */
		GLfloat originalWidth() const;

		/**
		 @result The original height (in pixels) of the bitmap this texture was made from
		 */
		GLfloat originalHeight() const;

		/**
		Update the texture object pixel data
		 @param pixels The pixel data to update the texture object with
		 @param width The width in pixels of the new image data 
		 @param height The height in pixels of the new image data
		 @param xoffset The top-left x coordinate for the updated texture region
		 @param yoffset The top-left y coordinate for the updated texture region
		 @param level The image level
		 */
		void subImage2D(unsigned char *pixels,
						GLsizei width, GLsizei height,
						GLint offsetx = 0, GLint offsety = 0,
						GLint level = 0);
		
	private:
		Format _format;
		GLuint _object;
		GLfloat _originalWidth;
		GLfloat _originalHeight;
		
		//copying disabled
		Texture(const Texture&);
		const Texture& operator=(const Texture&);
	};
	
}
