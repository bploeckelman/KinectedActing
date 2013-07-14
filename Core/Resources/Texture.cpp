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

#include "Texture.h"
#include <stdexcept>
#include <cassert>

using namespace tdogl;

static GLenum TextureFormatForBitmapFormat(const Texture::Format& format, bool srgb)
{
	switch (format) {
		case Texture::Format::Grayscale: return GL_LUMINANCE;
		case Texture::Format::GrayscaleAlpha: return GL_LUMINANCE_ALPHA;
		case Texture::Format::RGB: return (srgb ? GL_SRGB : GL_RGB);
		case Texture::Format::RGBA: return (srgb ? GL_SRGB_ALPHA : GL_RGBA);
		case Texture::Format::BGR: return (srgb ? GL_RGB8 : GL_BGR_EXT);
		case Texture::Format::BGRA: return (srgb ? GL_RGBA8 : GL_BGRA_EXT);
		default: throw std::runtime_error("Unrecognised Bitmap::Format");
	}
}

Texture::Texture(const Format& format, const GLsizei width, const GLsizei height, const unsigned char *pixels, GLint minMagFilter, GLint wrapMode) :
	_format(format),
	_originalWidth((GLfloat) width),
	_originalHeight((GLfloat) height)
{
	glGenTextures(1, &_object);
	glBindTexture(GL_TEXTURE_2D, _object);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexImage2D(GL_TEXTURE_2D, 0,
				 TextureFormatForBitmapFormat(format, true), (GLsizei) width, (GLsizei) height, 0,
				 TextureFormatForBitmapFormat(format, false), GL_UNSIGNED_BYTE, (GLvoid *) pixels);
	if (minMagFilter == GL_NEAREST_MIPMAP_NEAREST || minMagFilter == GL_NEAREST_MIPMAP_LINEAR
	 || minMagFilter == GL_LINEAR_MIPMAP_NEAREST  || minMagFilter == GL_LINEAR_MIPMAP_LINEAR) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &_object);
}

Texture::Format Texture::format() const
{
	return _format;
}

GLuint Texture::object() const
{
	return _object;
}

GLfloat Texture::originalWidth() const
{
	return _originalWidth;
}

GLfloat Texture::originalHeight() const
{
	return _originalHeight;
}

void Texture::subImage2D(unsigned char *pixels,
						 GLsizei width, GLsizei height,
						 GLint xoffset /*= 0*/,
						 GLint yoffset /*= 0*/,
						 GLint level /*= 0*/)
{
	assert(width <= originalWidth() && height <= originalHeight());
	glBindTexture(GL_TEXTURE_2D, object());
	glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height,
					TextureFormatForBitmapFormat(_format, false), GL_UNSIGNED_BYTE, (GLvoid *) pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
}
