//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//	Updated by GiBy 2013 for SDL2 <giby_the_kid@yahoo.fr>
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

#include "video/gl/gl_renderer.hpp"

#include <iomanip>
#include <iostream>
#include <physfs.h>
#include "SDL.h"

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/drawing_request.hpp"
#include "video/gl/gl_painter.hpp"
#include "video/gl/gl_surface_data.hpp"
#include "video/gl/gl_texture.hpp"
#include "video/util.hpp"

#ifdef USE_GLBINDING
#  include <glbinding/Binding.h>
#  include <glbinding/ContextInfo.h>
#  include <glbinding/gl/extension.h>
#  include <glbinding/callbacks.h>
#endif

#define LIGHTMAP_DIV 5

#ifdef GL_VERSION_ES_CM_1_0
#  define glOrtho glOrthof
bool GLEW_ARB_texture_non_power_of_two = false;
#endif

GLRenderer::GLRenderer() :
  //m_window(),
  //m_glcontext(),
  m_viewport(),
  m_desktop_size(0, 0),
  m_fullscreen_active(false)
{
  m_desktop_size = Size(SDL_GetVideoInfo()->current_w, SDL_GetVideoInfo()->current_h);

#ifdef USE_GLBINDING

  glbinding::Binding::initialize();

#ifdef USE_GLBINDING_DEBUG_OUTPUT
  glbinding::setCallbackMask(glbinding::CallbackMask::After | glbinding::CallbackMask::ParametersAndReturnValue);

  glbinding::setAfterCallback([](const glbinding::FunctionCall & call) {
    std::cout << call.function.name() << "(";

    for (unsigned i = 0; i < call.parameters.size(); ++i)
    {
      std::cout << call.parameters[i]->asString();
      if (i < call.parameters.size() - 1)
        std::cout << ", ";
    }

      std::cout << ")";

    if (call.returnValue)
    {
      std::cout << " -> " << call.returnValue->asString();
    }

    std::cout << std::endl;
  });
#endif

  static auto extensions = glbinding::ContextInfo::extensions();
  log_info << "Using glbinding" << std::endl;
  log_info << "ARB_texture_non_power_of_two: " << static_cast<int>(extensions.find(GLextension::GL_ARB_texture_non_power_of_two) != extensions.end()) << std::endl;

#endif

  // Init the projection matrix, viewport and stuff
  apply_config();

#ifndef GL_VERSION_ES_CM_1_0
  #ifndef USE_GLBINDING
  GLenum err = glewInit();
  #ifdef GLEW_ERROR_NO_GLX_DISPLAY
  if (GLEW_ERROR_NO_GLX_DISPLAY == err)
  {
    log_info << "GLEW couldn't open GLX display" << std::endl;
  }
  else
  #endif
  if (GLEW_OK != err)
  {
    std::ostringstream out;
    out << "GLRenderer: " << glewGetErrorString(err);
    throw std::runtime_error(out.str());
  }
  log_info << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
  log_info << "GLEW_ARB_texture_non_power_of_two: " << static_cast<int>(GLEW_ARB_texture_non_power_of_two) << std::endl;
#  endif
#else
  const char * extensions = (const char *) glGetString(GL_EXTENSIONS);
  if (extensions && (strstr(extensions, "GL_OES_texture_npot") || strstr(extensions, "GL_NV_texture_npot_2D_mipmap") || strstr(extensions, "GL_ARB_texture_non_power_of_two")))
  {
    GLEW_ARB_texture_non_power_of_two = true;
  }
  log_warning << "GLEW_ARB_texture_non_power_of_two: " << static_cast<int>(GLEW_ARB_texture_non_power_of_two) << std::endl;
  log_warning << "GLES extensions: " << extensions << std::endl;
#endif
}

GLRenderer::~GLRenderer()
{
  //SDL_GL_DeleteContext(m_glcontext);
  //SDL_DestroyWindow(m_window);
}

void
GLRenderer::do_take_screenshot()
{
  // [Christoph] TODO: Yes, this method also takes care of the actual disk I/O. Split it?

  SDL_Surface *shot_surf;
  // create surface to hold screenshot
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  shot_surf = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 24, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
#else
  shot_surf = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
#endif
  if (!shot_surf) {
    log_warning << "Could not create RGB Surface to contain screenshot" << std::endl;
    return;
  }

  // read pixels into array
  char* pixels = new char[3 * SCREEN_WIDTH * SCREEN_HEIGHT];
  if (!pixels) {
    log_warning << "Could not allocate memory to store screenshot" << std::endl;
    SDL_FreeSurface(shot_surf);
    return;
  }
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  // copy array line-by-line
  for (int i = 0; i < SCREEN_HEIGHT; i++) {
    char* src = pixels + (3 * SCREEN_WIDTH * (SCREEN_HEIGHT - i - 1));
    if(SDL_MUSTLOCK(shot_surf))
    {
      SDL_LockSurface(shot_surf);
    }
    char* dst = ((char*)shot_surf->pixels) + i * shot_surf->pitch;
    memcpy(dst, src, 3 * SCREEN_WIDTH);
    if(SDL_MUSTLOCK(shot_surf))
    {
      SDL_UnlockSurface(shot_surf);
    }
  }

  // free array
  delete[](pixels);

  // save screenshot
  static const std::string writeDir = PHYSFS_getWriteDir();
  static const std::string dirSep = PHYSFS_getDirSeparator();
  static const std::string baseName = "screenshot";
  static const std::string fileExt = ".bmp";
  std::string fullFilename;
  for (int num = 0; num < 1000; num++) {
    std::ostringstream oss;
    oss << baseName;
    oss << std::setw(3) << std::setfill('0') << num;
    oss << fileExt;
    std::string fileName = oss.str();
    fullFilename = writeDir + dirSep + fileName;
    if (!PHYSFS_exists(fileName.c_str())) {
      SDL_SaveBMP(shot_surf, fullFilename.c_str());
      log_info << "Wrote screenshot to \"" << fullFilename << "\"" << std::endl;
      SDL_FreeSurface(shot_surf);
      return;
    }
  }
  log_warning << "Did not save screenshot, because all files up to \"" << fullFilename << "\" already existed" << std::endl;
  SDL_FreeSurface(shot_surf);
}

void
GLRenderer::flip()
{
  assert_gl("drawing");
  SDL_GL_SwapBuffers();
  GLPainter::reset_last_texture();
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1, 1, 1, 1);
}

void
GLRenderer::resize(int w, int h)
{
  g_config->window_size = Size(w, h);

  apply_config();
}

void
GLRenderer::apply_config()
{
  apply_video_mode();

  // setup opengl state and transform
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  Size target_size = m_desktop_size;

  float pixel_aspect_ratio = 1.0f;
  if (g_config->aspect_size != Size(0, 0))
  {
    pixel_aspect_ratio = calculate_pixel_aspect_ratio(m_desktop_size,
                                                      g_config->aspect_size);
  }
  else if (g_config->use_fullscreen)
  {
    pixel_aspect_ratio = calculate_pixel_aspect_ratio(m_desktop_size,
                                                      target_size);
  }

#if defined(__ANDROID__)
  Size max_size(800, 600);
#else
  Size max_size(3840, 2160);
#endif
  Size min_size(640, 480);

  Vector scale;
  Size logical_size;
  calculate_viewport(min_size, max_size, target_size,
                     pixel_aspect_ratio,
                     g_config->magnification,
                     scale,
                     logical_size,
                     m_viewport);

  SCREEN_WIDTH = logical_size.width;
  SCREEN_HEIGHT = logical_size.height;

  if (m_viewport.x != 0 || m_viewport.y != 0)
  {
    // Clear both buffers so that we get a clean black border without junk
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapBuffers();
  }

  glViewport(m_viewport.x, m_viewport.y, m_viewport.w, m_viewport.h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, 0);
  check_gl_error("Setting up view matrices");
}

void
GLRenderer::apply_video_mode()
{
  //SCREEN_WIDTH = SDL_GetVideoInfo()->current_w;
  //SCREEN_HEIGHT = SDL_GetVideoInfo()->current_h;
#ifdef ANDROID
  SDL_SetVideoMode(m_desktop_size.width, m_desktop_size.height, SDL_GetVideoInfo()->vfmt->BitsPerPixel, SDL_OPENGL | SDL_DOUBLEBUF | SDL_FULLSCREEN);
#else
  //m_desktop_size.width = 1280;
  //m_desktop_size.height = 800;
  m_desktop_size.width = 640;
  m_desktop_size.height = 480;
  SDL_SetVideoMode(m_desktop_size.width, m_desktop_size.height, 16, SDL_OPENGL | SDL_DOUBLEBUF);
#endif
}

void
GLRenderer::start_draw()
{
}

void
GLRenderer::end_draw()
{
}

void
GLRenderer::draw_surface(const DrawingRequest& request)
{
  GLPainter::draw_surface(request);
}

void
GLRenderer::draw_surface_part(const DrawingRequest& request)
{
  GLPainter::draw_surface_part(request);
}

void
GLRenderer::draw_gradient(const DrawingRequest& request)
{
  GLPainter::draw_gradient(request);
}

void
GLRenderer::draw_filled_rect(const DrawingRequest& request)
{
  GLPainter::draw_filled_rect(request);
}

void
GLRenderer::draw_inverse_ellipse(const DrawingRequest& request)
{
  GLPainter::draw_inverse_ellipse(request);
}

void
GLRenderer::draw_line(const DrawingRequest& request)
{
  GLPainter::draw_line(request);
}

void
GLRenderer::draw_triangle(const DrawingRequest& request)
{
  GLPainter::draw_triangle(request);
}

Vector
GLRenderer::to_logical(int physical_x, int physical_y) const
{
  return Vector(static_cast<float>(physical_x - m_viewport.x) * SCREEN_WIDTH / m_viewport.w,
                static_cast<float>(physical_y - m_viewport.y) * SCREEN_HEIGHT / m_viewport.h);
}

void
GLRenderer::set_gamma(float gamma)
{
  //Uint16 ramp[256];
  //SDL_CalculateGammaRamp(gamma, ramp);
  //SDL_SetWindowGammaRamp(m_window, ramp, ramp, ramp);
}

/* EOF */
