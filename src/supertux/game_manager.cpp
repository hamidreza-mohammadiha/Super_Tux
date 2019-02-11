//  SuperTux
//  Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
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

#include "supertux/game_manager.hpp"

#include "supertux/levelset_screen.hpp"
#include "supertux/player_status.hpp"
#include "supertux/savegame.hpp"
#include "supertux/screen.hpp"
#include "supertux/screen_fade.hpp"
#include "supertux/screen_manager.hpp"
#include "supertux/world.hpp"
#include "supertux/levelsavestate.hpp"
#include "control/input_manager.hpp"
#include "util/file_system.hpp"
#include "util/log.hpp"
#include "util/reader.hpp"
#include "util/reader_document.hpp"
#include "util/reader_mapping.hpp"
#include "worldmap/level_tile.hpp"
#include "worldmap/worldmap.hpp"
#include "worldmap/worldmap_screen.hpp"

GameManager::GameManager() :
  m_savegame(),
  m_next_worldmap(),
  m_next_spawnpoint()
{
}

void
GameManager::start_level(const World& world, const std::string& level_filename)
{
  m_savegame = Savegame::from_file(world.get_savegame_filename());

  auto screen = std::make_unique<LevelsetScreen>(world.get_basedir(),
                                                 level_filename,
                                                 *m_savegame);
  ScreenManager::current()->push_screen(std::move(screen));
}

void
GameManager::start_worldmap(const World& world, const std::string& spawnpoint, const std::string& worldmap_filename)
{
  try
  {
    m_savegame = Savegame::from_file(world.get_savegame_filename());
    LevelSaveState::save(LevelSaveState(world.get_basedir()));

    auto filename = m_savegame->get_player_status().last_worldmap;
    // If we specified a worldmap filename manually,
    // this overrides the default choice of "last worldmap"
    if (!worldmap_filename.empty())
    {
      filename = worldmap_filename;
    }

    // No "last worldmap" found and no worldmap_filename
    // specified. Let's go ahead and use the worldmap
    // filename specified in the world.
    if (filename.empty())
    {
      filename = world.get_worldmap_filename();
    }

    auto worldmap = std::make_unique<worldmap::WorldMap>(filename, *m_savegame, spawnpoint);
    auto worldmap_screen = std::make_unique<worldmap::WorldMapScreen>(std::move(worldmap));
    ScreenManager::current()->push_screen(std::move(worldmap_screen));

    if (LevelSaveState::getLoading() && LevelSaveState::get().level != "" && LevelSaveState::get().sector != "")
    {
      worldmap->setup();
      worldmap::LevelTile* level = worldmap->at_level();
      if (!level)
      {
        log_warning << "No level to enter at worldmap" << std::endl;
      }
      else
      {
        std::string levelfile = world.get_basedir() + "/" + level->get_name();
        if (LevelSaveState::get().level != levelfile)
        {
          log_warning << "Saved level " << LevelSaveState::get().level << " does not match worlmap level " << levelfile << std::endl;
        }
        else
        {
          // Hack: press a button on a controller
          InputManager::current()->get_controller().reset();
          InputManager::current()->get_controller().set_control(Control::ACTION, true);
          worldmap->update(0.0f);
          InputManager::current()->get_controller().set_control(Control::ACTION, false);
        }
      }
    }
  }
  catch(std::exception& e)
  {
    log_fatal << "Couldn't start world: " << e.what() << std::endl;
  }
}

bool
GameManager::load_next_worldmap()
{
  if (m_next_worldmap.empty())
  {
    return false;
  }
  std::unique_ptr<World> world = World::from_directory(m_next_worldmap);
  m_next_worldmap = "";
  if (!world)
  {
    log_warning << "Can't load world '" << m_next_worldmap << "'" <<  std::endl;
    return false;
  }
  start_worldmap(*world, m_next_spawnpoint); // New world, new savegame
  return true;
}

void
GameManager::set_next_worldmap(const std::string& worldmap, const std::string &spawnpoint)
{
  m_next_worldmap = worldmap;
  m_next_spawnpoint = spawnpoint;
}

/* EOF */
