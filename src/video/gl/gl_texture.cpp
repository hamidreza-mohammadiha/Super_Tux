//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "supertux/gameconfig.hpp"
#include "video/gl/gl_texture.hpp"
#include "video/texture_manager.hpp"
#include "util/log.hpp"

#ifdef USE_GLBINDING
  #include <glbinding/ContextInfo.h>
#endif

namespace {

#ifdef GL_VERSION_ES_CM_1_0
inline bool is_power_of_2(int v)
{
  return (v & (v-1)) == 0;
}
#endif

inline int next_power_of_two(int val)
{
  int result = 1;
  while(result < val)
    result *= 2;
  return result;
}

} // namespace

GLTexture::GLTexture(unsigned int width, unsigned int height) :
  m_handle(),
  m_texture_width(),
  m_texture_height(),
  m_image_width(),
  m_image_height(),
  m_pixels(NULL)
{
#ifdef GL_VERSION_ES_CM_1_0
  assert(is_power_of_2(width));
  assert(is_power_of_2(height));
#endif
  m_texture_width  = width;
  m_texture_height = height;
  m_image_width  = width;
  m_image_height = height;

  assert_gl("before creating texture");

  reupload();
  TextureManager::current()->register_texture(this);
}

GLTexture::GLTexture(SDL_Surface* image) :
  m_handle(),
  m_texture_width(),
  m_texture_height(),
  m_image_width(),
  m_image_height(),
  m_pixels(NULL)
{
#ifdef GL_VERSION_ES_CM_1_0
  m_texture_width = next_power_of_two(image->w);
  m_texture_height = next_power_of_two(image->h);
#else
#  ifdef USE_GLBINDING
  static auto extensions = glbinding::ContextInfo::extensions();
  if (extensions.find(GLextension::GL_ARB_texture_non_power_of_two) != extensions.end())
  {
    m_texture_width  = image->w;
    m_texture_height = image->h;
  }
#  else
  if (GLEW_ARB_texture_non_power_of_two)
  {
    m_texture_width  = image->w;
    m_texture_height = image->h;
  }
#  endif
  else
  {
    m_texture_width = next_power_of_two(image->w);
    m_texture_height = next_power_of_two(image->h);
  }
#endif

  m_image_width  = image->w;
  m_image_height = image->h;

  SDL_Surface* convert;
  if (image->flags & SDL_SRCCOLORKEY) {
    // Palette image with colorkey transparency - RGBA5551
    convert = SDL_CreateRGBSurface(0,
                                   m_image_width, m_image_height, 16,
                                   0x0000f800, 0x000007c0, 0x0000003e, 0x00000001);
  } else if (image->format->BitsPerPixel == 32) {
    // 32-bit image with alpha channel - RGBA4444
    convert = SDL_CreateRGBSurface(0,
                                   m_image_width, m_image_height, 16,
                                   0x0000f000, 0x00000f00, 0x000000f0, 0x0000000f);
  } else {
    // 24-bit image or palette without transparency - RGB565
    convert = SDL_CreateRGBSurface(0,
                                   m_image_width, m_image_height, 16,
                                   0x0000f800, 0x000007e0, 0x0000001f, 0x00000000);
  }

  if(convert == 0) {
    throw std::runtime_error("Couldn't create texture: out of memory");
  }

  SDL_FillRect(convert, NULL, 0x00000000);
  //SDL_SetColorKey(image, 0, 0); // Some images use colorkey transparency
  SDL_SetAlpha(image, 0, SDL_ALPHA_OPAQUE);
  SDL_BlitSurface(image, 0, convert, 0);
  //SDL_Rect r = {m_image_width / 2, m_image_height / 2, 4, 4};
  //SDL_FillRect(convert, &r, 0xff000000);

  m_pixels = convert;

  reupload();
  TextureManager::current()->register_texture(this);

  assert_gl("creating texture");
}

void GLTexture::reupload()
{
  assert_gl("before creating texture");
  glGenTextures(1, &m_handle);

  try {
    GLenum sdl_format = GL_RGBA;
    GLenum pixel_packing = GL_UNSIGNED_BYTE;

    if (m_pixels) {
      if (m_pixels->format->Amask == 0)
        sdl_format = GL_RGB;
      if (m_pixels->format->Gmask == 0x000007c0)
        pixel_packing = GL_UNSIGNED_SHORT_5_5_5_1;
      if (m_pixels->format->Gmask == 0x00000f00)
        pixel_packing = GL_UNSIGNED_SHORT_4_4_4_4;
      if (m_pixels->format->Gmask == 0x000007e0)
        pixel_packing = GL_UNSIGNED_SHORT_5_6_5;
    }

    glBindTexture(GL_TEXTURE_2D, m_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#if defined(GL_UNPACK_ROW_LENGTH) || defined(USE_GLBINDING)
    //glPixelStorei(GL_UNPACK_ROW_LENGTH, convert->pitch/convert->format->BytesPerPixel);
#else
    /* OpenGL ES doesn't support UNPACK_ROW_LENGTH, let's hope SDL didn't add
     * padding bytes, otherwise we need some extra code here... */
    //assert(convert->pitch == m_texture_width * convert->format->BytesPerPixel);
#endif

    glTexImage2D(GL_TEXTURE_2D, 0, sdl_format,
                 m_texture_width, m_texture_height, 0, sdl_format,
                 pixel_packing, NULL);

    if (m_pixels)
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_image_width, m_image_height,
                      sdl_format, pixel_packing, m_pixels->pixels);

    set_texture_params();
  } catch(...) {
    glDeleteTextures(1, &m_handle);
    throw;
  }
}

GLTexture::~GLTexture()
{
  TextureManager::current()->remove_texture(this);
  if (m_pixels)
    SDL_FreeSurface(m_pixels);
  glDeleteTextures(1, &m_handle);
}

void
GLTexture::set_texture_params()
{
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_LINEAR));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_LINEAR));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));

  assert_gl("set texture params");
}

/* EOF */
