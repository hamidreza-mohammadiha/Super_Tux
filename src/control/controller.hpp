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

#ifndef HEADER_SUPERTUX_CONTROL_CONTROLLER_HPP
#define HEADER_SUPERTUX_CONTROL_CONTROLLER_HPP

#include <iosfwd>
#include <boost/optional.hpp>

#include "math/vector.hpp"

enum class Control {
  LEFT = 0,
  RIGHT,
  UP,
  DOWN,

  JUMP,
  ACTION,

  START,
  ESCAPE,
  MENU_SELECT,
  MENU_SELECT_SPACE,
  MENU_BACK,
  REMOVE,

  CHEAT_MENU,
  DEBUG_MENU,
  CONSOLE,

  PEEK_LEFT,
  PEEK_RIGHT,
  PEEK_UP,
  PEEK_DOWN,

  CONTROLCOUNT
};

std::ostream& operator<<(std::ostream& os, Control control);

std::string Control_to_string(Control control);
boost::optional<Control> Control_from_string(const std::string& text);

class Controller
{
public:
  Controller();
  virtual ~Controller();

  virtual void update();

  void set_control(Control control, bool value);

  /** returns true if the control is pressed down */
  bool hold(Control control) const;

  /** returns true if the control has just been pressed down this frame */
  bool pressed(Control control) const;

  /** returns true if the control has just been released this frame */
  bool released(Control control) const;

  void reset();

  bool mouse_pressed() const;
  Vector mouse_pos() const;
  void set_mouse(int x, int y, bool pressed);

protected:
  /** current control status */
  bool m_controls[static_cast<int>(Control::CONTROLCOUNT)];

  /** control status at last frame */
  bool m_old_controls[static_cast<int>(Control::CONTROLCOUNT)];
  bool mousePressed;
  Vector mousePos;

private:
  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;
};

#endif

/* EOF */
