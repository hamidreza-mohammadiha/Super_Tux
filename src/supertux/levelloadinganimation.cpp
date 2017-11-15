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

#include "supertux/levelloadinganimation.hpp"

#include "control/input_manager.hpp"
#include "math/random_generator.hpp"
#include "sprite/sprite_manager.hpp"
#include "supertux/fadeout.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/sector.hpp"
#include "supertux/resources.hpp"
#include "supertux/player_status.hpp"
#include "util/gettext.hpp"

#include <math.h>
#include <sstream>
#include <boost/format.hpp>

class PlayerStatus;

LevelLoadingAnimation::LevelLoadingAnimation()
{
}

LevelLoadingAnimation::~LevelLoadingAnimation()
{
}

void
LevelLoadingAnimation::setup()
{
}

void
LevelLoadingAnimation::update(float elapsed_time)
{
}

void
LevelLoadingAnimation::draw(DrawingContext& context)
{
  static int counter = -1;
  counter++;

  for (int i = 0; i < 12; i ++)
  {
    float x = -cosf(i * 30 * M_PI / 180.0f) * SCREEN_HEIGHT / 12;
    float y = sinf(i * 30 * M_PI / 180.0f) * SCREEN_HEIGHT / 12;
    context.draw_center_text(Resources::normal_font, "*", Vector(x, SCREEN_HEIGHT / 2 + y), LAYER_FOREGROUND1,
                             Color((i + counter) % 12 / 12.0f, (i + counter) % 12 / 12.0f, 0.5f, 1.0f));
  }
}

/* EOF */
