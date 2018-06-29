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

#ifndef HEADER_SUPERTUX_SUPERTUX_LEVELSAVESTATE_HPP
#define HEADER_SUPERTUX_SUPERTUX_LEVELSAVESTATE_HPP

#include <string>

#include "math/vector.hpp"

class LevelSaveState
{
public:
  std::string level;
  std::string sector;
  Vector pos;

  LevelSaveState(std::string level_ = "", std::string sector_ = "", Vector pos_ = Vector()) :
    level(level_),
    sector(sector_),
    pos(pos_)
  {}

  std::string getWorld() const;

  static const LevelSaveState & get();
  static void save(const LevelSaveState & state);
  static void erase();

  static bool getLoading();
  static void finishLoading();

private:

  static bool loading;
  static bool initialized;
  static LevelSaveState saved;
  static const char * filename;
};

#endif

/* EOF */
