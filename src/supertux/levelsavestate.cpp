//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//                2014 Ingo Ruhnke <grumbel@gmail.com>
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

#include <stdexcept>
#include <physfs.h>

#include "supertux/levelsavestate.hpp"
#include "util/reader_mapping.hpp"
#include "util/reader_document.hpp"
#include "util/writer.hpp"
#include "util/log.hpp"

bool LevelSaveState::loading = true;
bool LevelSaveState::initialized = false;
LevelSaveState LevelSaveState::saved;
const char * LevelSaveState::filename = "last-level";

const LevelSaveState & LevelSaveState::get()
{
  if (!initialized)
  {
    initialized = true;
    if (PHYSFS_exists(filename))
    {
      try
      {
        auto doc = ReaderDocument::from_file(filename);
        auto root = doc.get_root();
        if(root.get_name() != "last-level")
        {
          throw std::runtime_error("file is not a supertux-savegame file");
        }
        auto lisp = root.get_mapping();
        lisp.get("level", saved.level);
        lisp.get("sector", saved.sector);
        lisp.get("x", saved.pos.x);
        lisp.get("y", saved.pos.y);
      }
      catch(const std::exception& e)
      {
        log_fatal << "Couldn't load " << filename << ": " << e.what() << std::endl;
        saved = LevelSaveState();
      }
    }
  }

  return saved;
}

void LevelSaveState::save(const LevelSaveState & state)
{
  //log_warning << "saving level " << state.level << " sector " << state.sector << " pos " << state.pos.x << ":" << state.pos.y << " getLoading " << getLoading() << std::endl;
  if (state.level.find("levels/misc") == 0 || getLoading()) //  || (saved.level == state.level && saved.sector != "" && state.sector == "")
  {
    //log_warning << "Not saving" << std::endl;
    return; // Main menu demo level, ignore it
  }
  saved = state;
  Writer writer(filename);
  writer.start_list("last-level");
  writer.write("level", saved.level);
  writer.write("sector", saved.sector);
  writer.write("x", saved.pos.x);
  writer.write("y", saved.pos.y);
  writer.end_list("last-level");
}

void LevelSaveState::erase()
{
  if (PHYSFS_exists(filename))
  {
    PHYSFS_delete(filename);
  }
}

std::string LevelSaveState::getWorld() const
{
  // Return "levels/XXX" part of the level path, here we depend on the fact that all worldmaps are inside 'levels' directory
  std::string::size_type s = level.find("/");
  if (s == std::string::npos)
  {
    return "";
  }
  s = level.find("/", s + 1);
  return level.substr(0, s);
}

bool LevelSaveState::getLoading()
{
  return loading;
}

void LevelSaveState::finishLoading()
{
  loading = false;
}
