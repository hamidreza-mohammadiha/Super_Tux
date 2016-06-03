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

#include <sstream>
#include <boost/format.hpp>

class PlayerStatus;

LevelLoadingAnimation::LevelLoadingAnimation() //: player_sprite(SpriteManager::current()->create("images/creatures/tux/tux.sprite"))
{
  //Show walking tux animation
  //player_sprite->set_action("small-walk-right");
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
  context.draw_filled_rect(Vector(0, 0), Vector(SCREEN_WIDTH, SCREEN_HEIGHT), Color(0.0f, 0.0f, 0.0f, 1.0f), 0);
  static int counter = 0;
  counter++;

  //player_sprite->draw(context, Vector((SCREEN_WIDTH - player_sprite->get_current_hitbox_width()) / 2, SCREEN_HEIGHT / 2), LAYER_FOREGROUND1);
  std::stringstream ss;
  for (int i = -1; i < counter % 4; i++)
  {
    ss << "* ";
  }
  context.draw_center_text(Resources::normal_font, ss.str(), Vector(0, SCREEN_HEIGHT / 2), LAYER_FOREGROUND1, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

/* EOF */
