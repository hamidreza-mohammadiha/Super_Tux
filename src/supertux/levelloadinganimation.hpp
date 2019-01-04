//  SuperTux -- LevelIntro screen
//  Copyright (C) 2008 Christoph Sommer <christoph.sommer@2008.expires.deltadevelopment.de>
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

#ifndef HEADER_SUPERTUX_SUPERTUX_LEVELLOADINGANIMATION_HPP
#define HEADER_SUPERTUX_SUPERTUX_LEVELLOADINGANIMATION_HPP

#include "sprite/sprite.hpp"
#include "supertux/level.hpp"
#include "supertux/screen.hpp"
#include "supertux/timer.hpp"

class DrawingContext;
class Surface;
class PlayerStatus;

/**
 * Screen that welcomes the player to a level
 */
class LevelLoadingAnimation : public Screen
{
public:
  LevelLoadingAnimation();
  virtual ~LevelLoadingAnimation();

  void setup();
  void draw(Renderer& renderer);
  void update(float elapsed_time);

private:
  LevelLoadingAnimation(const LevelLoadingAnimation&);
  LevelLoadingAnimation& operator=(const LevelLoadingAnimation&);
};

#endif

/* EOF */
