//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
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

#include "editor/scroller_widget.hpp"

#include <math.h>

#include "editor/editor.hpp"
#include "gui/menu_manager.hpp"
#include "supertux/globals.hpp"
#include "video/drawing_context.hpp"
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/viewport.hpp"

namespace {

#ifdef __ANDROID__
const float TOPLEFT = 16 * 2;
const float MIDDLE = 48 * 2;
const float BOTTOMRIGHT = 80 * 2;
const float SIZE = 96 * 2;
const float SPEED = 0.8;
#else
const float TOPLEFT = 16;
const float MIDDLE = 48;
const float BOTTOMRIGHT = 80;
const float SIZE = 96;
const float SPEED = 2;
#endif

}

#ifdef __ANDROID__
int EditorScrollerWidget::rendered = EditorScrollerWidget::SCROLLER_BOTTOM;
#else
int EditorScrollerWidget::rendered = EditorScrollerWidget::SCROLLER_TOP;
#endif

EditorScrollerWidget::EditorScrollerWidget(Editor& editor) :
  m_editor(editor),
  m_scrolling(),
  m_scrolling_vec(0, 0),
  m_mouse_pos(0, 0)
{
}

bool
EditorScrollerWidget::can_scroll() const
{
  int ypos = (rendered == SCROLLER_TOP) ? 0 : SCREEN_HEIGHT - SIZE;
  return m_scrolling && m_mouse_pos.x < SIZE && m_mouse_pos.y < SIZE + ypos && m_mouse_pos.y >= ypos;
}

void
EditorScrollerWidget::draw(DrawingContext& context)
{
  if (rendered == SCROLLER_NONE) return;
  if (MenuManager::instance().is_active()) return;

  int ypos = rendered == SCROLLER_TOP ? 0 : SCREEN_HEIGHT - SIZE;

  context.color().draw_filled_rect(Rectf(Vector(0, ypos), Vector(SIZE, SIZE + ypos)),
                                     Color(0.9f, 0.9f, 1.0f, 0.6f),
                                     MIDDLE, LAYER_GUI-10);
  context.color().draw_filled_rect(Rectf(Vector(MIDDLE - 8, MIDDLE - 8 + ypos), Vector(MIDDLE + 8, MIDDLE + 8 + ypos)),
                                     Color(0.9f, 0.9f, 1.0f, 0.6f),
                                     8, LAYER_GUI-20);
  if (can_scroll()) {
    draw_arrow(context, m_mouse_pos);
  }

  draw_arrow(context, Vector(TOPLEFT, MIDDLE));
  draw_arrow(context, Vector(BOTTOMRIGHT, MIDDLE));
  draw_arrow(context, Vector(MIDDLE, TOPLEFT));
  draw_arrow(context, Vector(MIDDLE, BOTTOMRIGHT));
}

void
EditorScrollerWidget::draw_arrow(DrawingContext& context, const Vector& pos)
{
  Vector ypos = Vector(0, rendered == SCROLLER_TOP ? 0 : SCREEN_HEIGHT - SIZE);
  Vector dir = pos - Vector(MIDDLE, MIDDLE);
  if (dir.x != 0 || dir.y != 0) {
    // draw a triangle
    dir = glm::normalize(dir) * 8.0f;
    Vector dir2 = Vector(-dir.y, dir.x);
    context.color().draw_triangle(pos + dir + ypos, pos - dir + dir2 + ypos, pos - dir - dir2 + ypos,
                                    Color(1, 1, 1, 0.5), LAYER_GUI-20);
  }
}

void
EditorScrollerWidget::update(float dt_sec)
{
  if (rendered == SCROLLER_NONE) return;
  if (!can_scroll()) return;

  m_editor.scroll(m_scrolling_vec * 32.0f * dt_sec);
}

bool
EditorScrollerWidget::on_mouse_button_up(const SDL_MouseButtonEvent& button)
{
  m_scrolling = false;
  return false;
}

bool
EditorScrollerWidget::on_mouse_button_down(const SDL_MouseButtonEvent& button)
{
  if (button.button == SDL_BUTTON_LEFT) {
    if (rendered == SCROLLER_NONE) return false;

    int ypos = (rendered == SCROLLER_TOP) ? 0 : SCREEN_HEIGHT - SIZE;

    if (m_mouse_pos.x < SIZE && m_mouse_pos.y < SIZE + ypos && m_mouse_pos.y >= ypos) {
      m_scrolling = true;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
  return false;
}

bool
EditorScrollerWidget::on_mouse_motion(const SDL_MouseMotionEvent& motion)
{
  if (rendered == SCROLLER_NONE) return false;

  int ypos = (rendered == SCROLLER_TOP) ? 0 : SCREEN_HEIGHT - SIZE;
  m_mouse_pos = VideoSystem::current()->get_viewport().to_logical(motion.x, motion.y);
  if (m_mouse_pos.x < SIZE && m_mouse_pos.y < SIZE + ypos && m_mouse_pos.y >= ypos) {
    m_scrolling_vec = m_mouse_pos - Vector(MIDDLE, MIDDLE + ypos);
    if (m_scrolling_vec.x != 0 || m_scrolling_vec.y != 0) {
      float norm = glm::length(m_scrolling_vec);
      //m_scrolling_vec *= powf(static_cast<float>(M_E), norm / 16.0f - 1.0f);
      norm = std::min(MIDDLE / 2, norm) * SPEED;
      m_scrolling_vec *= norm;
    }
  }
  return false;
}

bool
EditorScrollerWidget::on_key_down(const SDL_KeyboardEvent& key)
{
  if (key.keysym.sym == SDLK_F9) {
    rendered = !rendered;
  }
  return false;
}

/* EOF */
