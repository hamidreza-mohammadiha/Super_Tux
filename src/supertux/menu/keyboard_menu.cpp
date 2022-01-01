//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>,
//                2007 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/menu/keyboard_menu.hpp"

#include "control/input_manager.hpp"
#include "control/keyboard_manager.hpp"
#include "gui/item_controlfield.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "util/gettext.hpp"

KeyboardMenu::KeyboardMenu(InputManager& input_manager) :
  m_input_manager(input_manager)
{
  add_label(_("Setup Keyboard"));
  add_hl();
  add_controlfield(static_cast<int>(Control::UP),         _("Up"));
  add_controlfield(static_cast<int>(Control::DOWN),       _("Down"));
  add_controlfield(static_cast<int>(Control::LEFT),       _("Left"));
  add_controlfield(static_cast<int>(Control::RIGHT),      _("Right"));
  add_controlfield(static_cast<int>(Control::JUMP),       _("Jump"));
  add_controlfield(static_cast<int>(Control::ACTION),     _("Action"));
  add_controlfield(static_cast<int>(Control::PEEK_LEFT),  _("Peek Left"));
  add_controlfield(static_cast<int>(Control::PEEK_RIGHT), _("Peek Right"));
  add_controlfield(static_cast<int>(Control::PEEK_UP),    _("Peek Up"));
  add_controlfield(static_cast<int>(Control::PEEK_DOWN),  _("Peek Down"));
  if (g_config->developer_mode) {
    add_controlfield(static_cast<int>(Control::CONSOLE), _("Console"));
    add_controlfield(static_cast<int>(Control::CHEAT_MENU), _("Cheat Menu"));
    add_controlfield(static_cast<int>(Control::DEBUG_MENU), _("Debug Menu"));
  }
  add_toggle(static_cast<int>(Control::CONTROLCOUNT), _("Jump with Up"), &g_config->keyboard_config.m_jump_with_up_kbd);
  add_hl();
  add_back(_("Back"));
  refresh();
}

std::string
KeyboardMenu::get_key_name(SDL_Keycode key) const
{
  switch (key) {
    case SDLK_UNKNOWN:
      return _("None");
    case SDLK_UP:
      return _("Up cursor");
    case SDLK_DOWN:
      return _("Down cursor");
    case SDLK_LEFT:
      return _("Left cursor");
    case SDLK_RIGHT:
      return _("Right cursor");
    case SDLK_RETURN:
      return _("Return");
    case SDLK_SPACE:
      return _("Space");
    case SDLK_RSHIFT:
      return _("Right Shift");
    case SDLK_LSHIFT:
      return _("Left Shift");
    case SDLK_RCTRL:
      return _("Right Control");
    case SDLK_LCTRL:
      return _("Left Control");
    case SDLK_RALT:
      return _("Right Alt");
    case SDLK_LALT:
      return _("Left Alt");
    case SDLK_RGUI:
      return _("Right Command");
    case SDLK_LGUI:
      return _("Left Command");
    default:
      return SDL_GetKeyName(static_cast<SDL_Keycode>(key));
  }
}

void
KeyboardMenu::menu_action(MenuItem& item)
{
  if (item.get_id() >= 0 && item.get_id() < static_cast<int>(Control::CONTROLCOUNT)) {
    ItemControlField* itemcf = dynamic_cast<ItemControlField*>(&item);
    if (!itemcf) {
      return;
    }
    itemcf->change_input(_("Press Key"));
    m_input_manager.keyboard_manager->bind_next_event_to(static_cast<Control>(item.get_id()));
  }
}

void
KeyboardMenu::refresh()
{
  KeyboardConfig& kbd_cfg = g_config->keyboard_config;
  ItemControlField* micf;

  micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::UP)));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::UP)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::DOWN)));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::DOWN)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::LEFT)));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::LEFT)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::RIGHT)));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::RIGHT)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::JUMP)));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::JUMP)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::ACTION)));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::ACTION)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::PEEK_LEFT)));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::PEEK_LEFT)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::PEEK_RIGHT)));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::PEEK_RIGHT)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::PEEK_UP)));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::PEEK_UP)));
  micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::PEEK_DOWN)));
  if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::PEEK_DOWN)));

  if (g_config->developer_mode) {
    micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::CHEAT_MENU)));
    if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::CHEAT_MENU)));

    micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::DEBUG_MENU)));
    if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::DEBUG_MENU)));
  }

  if (g_config->developer_mode) {
    micf = dynamic_cast<ItemControlField*>(&get_item_by_id(static_cast<int>(Control::CONSOLE)));
    if (micf) micf->change_input(get_key_name(kbd_cfg.reversemap_key(Control::CONSOLE)));
  }
}

/* EOF */
