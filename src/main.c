/******************************************************************************
     Filename: main.c

       Author: David C. Drake (david@davidcdrake.com)

Last modified: 3/14/2014

  Description: Function definitions for MazeCrawler, a 3D, first-person, maze-
               navigation game developed for the Pebble smartwatch (SDK 2.0).
               Copyright 2014, David C. Drake. More information available
               online: http://davidcdrake.com/mazecrawler
******************************************************************************/

#include "maze_crawler.h"

/******************************************************************************
   Function: show_narration

Description: Displays narration text via the narration window. The type of
             narration is determined by "g_current_narration" and the specific
             text is further determined by "g_narration_page_num".

     Inputs: None.

    Outputs: None.
******************************************************************************/
void show_narration(void)
{
  static char narration_str[NARRATION_STR_LEN + 1];

  // Ensure the narration window has been initialized:
  if (g_narration_window == NULL)
  {
    init_narration();
  }

  // Determine whether the current narration is finished:
  if ((g_current_narration == INTRO_NARRATION && g_narration_page_num > 3) ||
      (g_current_narration < STATS_NARRATION && g_narration_page_num > 1))
  {
    if (window_stack_get_top_window() == g_narration_window)
    {
      window_stack_pop(false /* not animated */);
    }
    deinit_narration();

    return;
  }

  // Determine what text should be displayed:
  switch (g_current_narration)
  {
    case INTRO_NARRATION:
      switch (g_narration_page_num)
      {
        case 0:
          strcpy(narration_str,
"You have fallen into a vast network of mazes. Each maze has an exit...");
          break;
        case 1:
          strcpy(narration_str,
"...but each exit leads down to yet another, deeper level of the labyrinth.");
          break;
        case 2:
          strcpy(narration_str,
"Will you ever escape, or are you doomed to roam these halls to the end of "
"your days?");
          break;
        default:
          strcpy(narration_str,
"You do not know, but here you are, brave explorer, and you must try!");
          break;
      }
      break;
    case CONTROLS_NARRATION:
      switch (g_narration_page_num)
      {
        case 0:
          strcpy(narration_str, "Forward: \"Up\"\nBackward: \"Down\"\n\n"
                                "Left: \"Up\" x 2\nRight: \"Down\" x 2");
          break;
        default:
          strcpy(narration_str, "More information available online:\n\n"
                                "davidcdrake.com/\n           mazecrawler");
          break;
      }
      break;
    case GAME_INFO_NARRATION:
      switch (g_narration_page_num)
      {
        case 0:
          strcpy(narration_str,
"MazeCrawler was designed and programmed by David C. Drake:\ndavidcdrake.com");
          break;
        default:
          strcpy(narration_str,
"Thanks for playing, and special thanks to all those who helped make this "
"possible!");
          break;
      }
      break;
    default: // case STATS_NARRATION:
      switch (g_narration_page_num)
      {
        case 0: // This may reach 62 characters.
          strcpy(narration_str, "Mazes Completed:\n  ");
          if (g_player->level == MAX_SMALL_INT_VALUE &&
              g_player->achievement_unlocked[MAX_LEVEL_ACHIEVEMENT])
          {
            strcat(narration_str, "9999");
          }
          else
          {
            cat_int_onto_str(narration_str, g_player->level - 1);
          }
          strcat(narration_str, "\nBest Time:\n  ");
          if (g_player->level == 1)
          {
            strcat(narration_str, "--:--");
          }
          else
          {
            cat_time_onto_str(narration_str, g_player->best_time);
          }
          strcat(narration_str, "\nPoints:\n  ");
          cat_int_onto_str(narration_str, g_player->points);
          break;
        default:
          while (g_narration_page_num  - 1 < NUM_ACHIEVEMENTS &&
                 g_player->achievement_unlocked[g_narration_page_num - 1] == false)
          {
            g_narration_page_num++;
          }
          if (g_narration_page_num - 1 < NUM_ACHIEVEMENTS)
          {
            strcpy(narration_str, "Achievements:\n  \"");
            strcat(narration_str, get_achievement_name(g_narration_page_num - 1));
            strcat(narration_str, "\": ");
            strcat(narration_str, get_achievement_description(g_narration_page_num - 1));
          }
          else
          {
            if (window_stack_get_top_window() == g_narration_window)
            {
              window_stack_pop(false /* not animated */);
            }
            deinit_narration();

            return;
          }
          break;
      }
      break;
  }

  // Finally, display the current narration text:
  text_layer_set_text(g_narration_text_layer, narration_str);
  show_window(g_narration_window);

  // Add text one character at a time via the narration timer:
  /*g_narration_timer = app_timer_register(NARRATION_TIMER_DURATION,
                                         narration_timer_callback,
                                         NULL);*/
}

/******************************************************************************
   Function: show_window

Description: Displays a given window. (Assumes that window has already been
             initialized.)

     Inputs: window - Pointer to the desired window.

    Outputs: None.
******************************************************************************/
void show_window(Window *window)
{
  if (window == NULL)
  {
    return;
  }
  else if (!window_stack_contains_window(window))
  {
    window_stack_push(window, false /* not animated */);
  }
  else
  {
    while (window_stack_get_top_window() != window)
	{
      window_stack_pop(false /* not animated */);
    }
  }
}

/******************************************************************************
   Function: player_init

Description: Initializes the global player struct.

     Inputs: None.

    Outputs: None.
******************************************************************************/
void player_init(void)
{
  int16_t i;

  g_player->position = GPoint(0, 0);
  g_player->direction = rand() % NUM_DIRECTIONS;
  g_player->level = 1;
  g_player->points = 0;
  g_player->best_time = MAX_SECONDS;
  for (i = 0; i < NUM_ACHIEVEMENTS; ++i)
  {
    g_player->achievement_unlocked[i] = false;
  }
}

/******************************************************************************
   Function: update_status_bar

Description: Updates the text displayed in the lower status bar.

     Inputs: ctx - Pointer to the relevant graphics context.

    Outputs: None.
******************************************************************************/
void update_status_bar(GContext *ctx)
{
  static char level_str[LEVEL_STR_LEN + 1],
              time_str[TIME_STR_LEN + 1];

  // Display the current level number:
  strcpy(level_str, "L. ");
  cat_int_onto_str(level_str, g_player->level);
  text_layer_set_text(g_level_text_layer, level_str);

  // Display the amount of time spent in the current maze:
  strcpy(time_str, "");
  cat_time_onto_str(time_str, g_maze->seconds);
  text_layer_set_text(g_time_text_layer, time_str);

  // Draw the compass:
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx,
                       GPoint(HALF_SCREEN_WIDTH,
                              GRAPHICS_FRAME_HEIGHT + STATUS_BAR_HEIGHT / 2),
                       COMPASS_RADIUS);
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_outline(ctx, g_compass_path);
  gpath_draw_filled(ctx, g_compass_path);
}

/******************************************************************************
   Function: update_compass

Description: Updates the rotation angle of the compass needle according to the
             player's current direction.

     Inputs: None.

    Outputs: None.
******************************************************************************/
void update_compass(void)
{
  switch(g_player->direction)
  {
    case NORTH:
      gpath_rotate_to(g_compass_path, TRIG_MAX_ANGLE / 2);
      break;
    case SOUTH:
      gpath_rotate_to(g_compass_path, 0);
      break;
    case EAST:
      gpath_rotate_to(g_compass_path, TRIG_MAX_ANGLE * 0.75);
      break;
    default: // case WEST:
      gpath_rotate_to(g_compass_path, TRIG_MAX_ANGLE / 4);
      break;
  }
}

/******************************************************************************
   Function: reposition_player

Description: Moves and orients the player to the maze's starting position and
             direction.

     Inputs: None.

    Outputs: None.
******************************************************************************/
void reposition_player(void)
{
  g_player->position = g_maze->entrance;
  g_player->direction = g_maze->starting_direction;
  update_compass();
}

/******************************************************************************
   Function: move_player

Description: Attempts to move the player one cell forward in a given direction
             (with help from the "shift_position" function). A wall in that
             direction will prevent this movement.

     Inputs: direction - Desired direction of movement.

    Outputs: Returns "true" if the player successfully moved.
******************************************************************************/
bool move_player(const int16_t direction)
{
  if (shift_position(&(g_player->position), direction))
  {
    layer_mark_dirty(window_get_root_layer(g_graphics_window));
    check_for_maze_completion();

    return true;
  }
  /*else if (g_player->vibrations_on)
  {
    vibes_short_pulse();
  }*/

  return false;
}

/******************************************************************************
   Function: shift_position

Description: Attempts to shift a given set of position coordinates one cell
             forward in a given direction. A wall in that direction will
             prevent this.

     Inputs: position  - Pointer to the character's coordinates.
             direction - Desired direction of movement.

    Outputs: Returns "true" if the character successfully moved.
******************************************************************************/
bool shift_position(GPoint *position, const int16_t direction)
{
  GPoint destination;

  // Get destination coordinates:
  switch (direction)
  {
    case NORTH:
      destination = GPoint(position->x, position->y - 1);
      break;
    case SOUTH:
      destination = GPoint(position->x, position->y + 1);
      break;
    case EAST:
      destination = GPoint(position->x + 1, position->y);
      break;
    default: // case WEST:
      destination = GPoint(position->x - 1, position->y);
      break;
  }

  // Check for a wall/obstacle:
  if (is_solid(destination))
  {
    return false;
  }

  // Move the character:
  *position = destination;

  return true;
}

/******************************************************************************
   Function: check_for_maze_completion

Description: Determines whether the current maze/level has been completed, then
             handles the transition to the next maze if applicable.

     Inputs: None.

    Outputs: Returns "true" if the maze has been completed.
******************************************************************************/
bool check_for_maze_completion(void)
{
  int32_t points_earned, max_time_bonus;

  if (get_cell_type(g_player->position) == EXIT)
  {
    // Compute points earned according to maze size and time elapsed:
    points_earned = (g_maze->width * g_maze->height) / 10 +
                    (((g_maze->width * g_maze->height) % 10 >= 5) ? 1 : 0);
    max_time_bonus = (g_maze->width * g_maze->height) / 2 +
                     (g_maze->width * g_maze->height) % 2;
    if (g_maze->seconds < max_time_bonus)
    {
      points_earned += max_time_bonus - g_maze->seconds;
    }
    if (g_player->points + points_earned >= MAX_POINTS ||
        g_player->points + points_earned < g_player->points)
    {
      g_player->points = MAX_POINTS;
      if (!g_player->achievement_unlocked[MAX_POINTS_ACHIEVEMENT])
	  {
        g_player->achievement_unlocked[MAX_POINTS_ACHIEVEMENT] = true;
        g_new_achievement_unlocked[MAX_POINTS_ACHIEVEMENT] = true;
      }
    }
    else
    {
      g_player->points += points_earned;
    }

    // Build a congratulatory message:
    switch(rand() % 30)
    {
      case 0:
        strcpy(g_message_str, "A-maze-ing!\n");
        break;
      case 1:
        strcpy(g_message_str, "Awesome!\n");
        break;
      case 2:
        strcpy(g_message_str, "Brilliant!\n");
        break;
      case 3:
        strcpy(g_message_str, "Congrats!\n");
        break;
      case 4:
        strcpy(g_message_str, "Excellent!\n");
        break;
      case 5:
        strcpy(g_message_str, "Fantastic!\n");
        break;
      case 6:
        strcpy(g_message_str, "Good job!\n");
        break;
      case 7:
        strcpy(g_message_str, "Great work!\n");
        break;
      case 8:
        strcpy(g_message_str, "Groovy!\n");
        break;
      case 9:
        strcpy(g_message_str, "Huzzah!\n");
        break;
      case 10:
        strcpy(g_message_str, "Hurrah!\n");
        break;
      case 11:
        strcpy(g_message_str, "Hooray!\n");
        break;
      case 12:
        strcpy(g_message_str, "Impressive!\n");
        break;
      case 13:
        strcpy(g_message_str, "Magnificent!\n");
        break;
      case 14:
        strcpy(g_message_str, "Marvelous!\n");
        break;
      case 15:
        strcpy(g_message_str, "Outstanding!\n");
        break;
      case 16:
        strcpy(g_message_str, "Peachy!\n");
        break;
      case 17:
        strcpy(g_message_str, "Phenomenal!\n");
        break;
      case 18:
        strcpy(g_message_str, "Spectacular!\n");
        break;
      case 19:
        strcpy(g_message_str, "Splendid!\n");
        break;
      case 20:
        strcpy(g_message_str, "Stellar!\n");
        break;
      case 21:
        strcpy(g_message_str, "Stupendous!\n");
        break;
      case 22:
        strcpy(g_message_str, "Superb!\n");
        break;
      case 23:
        strcpy(g_message_str, "Terrific!\n");
        break;
      case 24:
        strcpy(g_message_str, "Well done!\n");
        break;
      case 25:
        strcpy(g_message_str, "Wahoo!\n");
        break;
      case 26:
        strcpy(g_message_str, "Whoopee!\n");
        break;
      case 27:
        strcpy(g_message_str, "Wonderful!\n");
        break;
      case 28:
        strcpy(g_message_str, "Wowzers!\n");
        break;
      default: // case 29:
        strcpy(g_message_str, "Yippee!\n");
        break;
    }
    strcat(g_message_str, "\nTime: ");
    cat_time_onto_str(g_message_str, g_maze->seconds);
    strcat(g_message_str, "\nPoints: ");
    cat_int_onto_str(g_message_str, points_earned);
    show_message_box();

    // Update stats, check for a new best time, and check for achievements:
    if (g_player->level < MAX_LEVEL)
    {
      g_player->level++;
    }
    else
    {
      if(!g_player->achievement_unlocked[MAX_LEVEL_ACHIEVEMENT])
      {
        g_player->achievement_unlocked[MAX_LEVEL_ACHIEVEMENT] = true;
        g_new_achievement_unlocked[MAX_LEVEL_ACHIEVEMENT] = true;
      }
    }
    if (g_maze->seconds < g_player->best_time)
    {
      g_player->best_time = g_maze->seconds;
      g_new_best_time = g_player->best_time;
    }
    if (g_maze->seconds < 30 &&
        !g_player->achievement_unlocked[UNDER_THIRTY_SECONDS_ACHIEVEMENT])
    {
      g_player->achievement_unlocked[UNDER_THIRTY_SECONDS_ACHIEVEMENT] = true;
      g_new_achievement_unlocked[UNDER_THIRTY_SECONDS_ACHIEVEMENT] = true;
    }
    if (g_maze->seconds < 10 &&
        !g_player->achievement_unlocked[UNDER_TEN_SECONDS_ACHIEVEMENT])
    {
      g_player->achievement_unlocked[UNDER_TEN_SECONDS_ACHIEVEMENT] = true;
      g_new_achievement_unlocked[UNDER_TEN_SECONDS_ACHIEVEMENT] = true;
    }
    switch(g_player->level)
	{
      case 2:
        if(!g_player->achievement_unlocked[FIRST_LEVEL_ACHIEVEMENT])
        {
          g_player->achievement_unlocked[FIRST_LEVEL_ACHIEVEMENT] = true;
          g_new_achievement_unlocked[FIRST_LEVEL_ACHIEVEMENT] = true;
        }
        break;
      case 10:
        if(!g_player->achievement_unlocked[LEVEL_10_ACHIEVEMENT])
        {
          g_player->achievement_unlocked[LEVEL_10_ACHIEVEMENT] = true;
          g_new_achievement_unlocked[LEVEL_10_ACHIEVEMENT] = true;
        }
        break;
      case 50:
        if(!g_player->achievement_unlocked[LEVEL_50_ACHIEVEMENT])
        {
          g_player->achievement_unlocked[LEVEL_50_ACHIEVEMENT] = true;
          g_new_achievement_unlocked[LEVEL_50_ACHIEVEMENT] = true;
        }
        break;
      case 100:
        if(!g_player->achievement_unlocked[LEVEL_100_ACHIEVEMENT])
        {
          g_player->achievement_unlocked[LEVEL_100_ACHIEVEMENT] = true;
          g_new_achievement_unlocked[LEVEL_100_ACHIEVEMENT] = true;
        }
        break;
      case 500:
        if(!g_player->achievement_unlocked[LEVEL_500_ACHIEVEMENT])
        {
          g_player->achievement_unlocked[LEVEL_500_ACHIEVEMENT] = true;
          g_new_achievement_unlocked[LEVEL_500_ACHIEVEMENT] = true;
        }
        break;
      case 1000:
        if(!g_player->achievement_unlocked[LEVEL_1000_ACHIEVEMENT])
        {
          g_player->achievement_unlocked[LEVEL_1000_ACHIEVEMENT] = true;
          g_new_achievement_unlocked[LEVEL_1000_ACHIEVEMENT] = true;
        }
        break;
      case 5000:
        if(!g_player->achievement_unlocked[LEVEL_5000_ACHIEVEMENT])
        {
          g_player->achievement_unlocked[LEVEL_5000_ACHIEVEMENT] = true;
          g_new_achievement_unlocked[LEVEL_5000_ACHIEVEMENT] = true;
        }
        break;
      default:
        break;
	}

    // Save player progress and set up the next maze:
    //save_game_data();
    maze_init();

    return true;
  }

  return false;
}

/******************************************************************************
   Function: save_game_data

Description: Saves game data to the Pebble's persistent storage.

     Inputs: None.

    Outputs: Returns "true" if the player struct was saved successfully.
******************************************************************************/
bool save_game_data(void)
{
  int16_t num_bytes_stored = persist_write_data(STORAGE_KEY,
                                                g_player,
                                                sizeof(*g_player));

  return num_bytes_stored == sizeof(*g_player);
}

/******************************************************************************
   Function: load_game_data

Description: Loads game data from the Pebble's persistent storage. For any data
             not found, default data are generated.

     Inputs: None.

    Outputs: Returns "true" if all data sought were found.
******************************************************************************/
bool load_game_data(void)
{
  if (persist_exists(STORAGE_KEY))
  {
    persist_read_data(STORAGE_KEY, g_player, sizeof(player_t));

    return true;
  }
  player_init();

  return false;
}

/******************************************************************************
   Function: wall_coords_init

Description: Initializes the global "back_wall_coords" array so that it
             contains the top-left and bottom-right coordinates for every
             potential back wall location on the screen. (This establishes the
             field of view and sense of perspective while also facilitating
             convenient drawing of the 3D environment.)

     Inputs: None.

    Outputs: None.
******************************************************************************/
void wall_coords_init(void)
{
  int16_t i, j, wall_width;
  const float perspective_modifier = 2.0; // Helps determine FOV, etc.

  for (i = 0; i < MAX_VISIBILITY_DEPTH - 1; ++i)
  {
    for (j = 0; j < (STRAIGHT_AHEAD * 2) + 1; ++j)
    {
      g_back_wall_coords[i][j][TOP_LEFT] = GPoint(0, 0);
      g_back_wall_coords[i][j][BOTTOM_RIGHT] = GPoint(0, 0);
    }
  }
  for (i = 0; i < MAX_VISIBILITY_DEPTH - 1; ++i)
  {
    g_back_wall_coords[i][STRAIGHT_AHEAD][TOP_LEFT] =
      GPoint(FIRST_WALL_OFFSET - i * perspective_modifier,
             FIRST_WALL_OFFSET - i * perspective_modifier);
    if (i > 0)
    {
      g_back_wall_coords[i][STRAIGHT_AHEAD][TOP_LEFT].x +=
        g_back_wall_coords[i - 1][STRAIGHT_AHEAD][TOP_LEFT].x;
      g_back_wall_coords[i][STRAIGHT_AHEAD][TOP_LEFT].y +=
        g_back_wall_coords[i - 1][STRAIGHT_AHEAD][TOP_LEFT].y;
    }
    g_back_wall_coords[i][STRAIGHT_AHEAD][BOTTOM_RIGHT].x =
      SCREEN_WIDTH - g_back_wall_coords[i][STRAIGHT_AHEAD][TOP_LEFT].x;
    g_back_wall_coords[i][STRAIGHT_AHEAD][BOTTOM_RIGHT].y =
      GRAPHICS_FRAME_HEIGHT -
      g_back_wall_coords[i][STRAIGHT_AHEAD][TOP_LEFT].y;
    wall_width = g_back_wall_coords[i][STRAIGHT_AHEAD][BOTTOM_RIGHT].x -
                 g_back_wall_coords[i][STRAIGHT_AHEAD][TOP_LEFT].x;
    for (j = 1; j <= STRAIGHT_AHEAD; ++j)
    {
      g_back_wall_coords[i][STRAIGHT_AHEAD - j][TOP_LEFT] =
        g_back_wall_coords[i][STRAIGHT_AHEAD][TOP_LEFT];
      g_back_wall_coords[i][STRAIGHT_AHEAD - j][TOP_LEFT].x -= wall_width * j;
      g_back_wall_coords[i][STRAIGHT_AHEAD - j][BOTTOM_RIGHT] =
        g_back_wall_coords[i][STRAIGHT_AHEAD][BOTTOM_RIGHT];
      g_back_wall_coords[i][STRAIGHT_AHEAD - j][BOTTOM_RIGHT].x -= wall_width *
                                                                   j;
      g_back_wall_coords[i][STRAIGHT_AHEAD + j][TOP_LEFT] =
        g_back_wall_coords[i][STRAIGHT_AHEAD][TOP_LEFT];
      g_back_wall_coords[i][STRAIGHT_AHEAD + j][TOP_LEFT].x += wall_width * j;
      g_back_wall_coords[i][STRAIGHT_AHEAD + j][BOTTOM_RIGHT] =
        g_back_wall_coords[i][STRAIGHT_AHEAD][BOTTOM_RIGHT];
      g_back_wall_coords[i][STRAIGHT_AHEAD + j][BOTTOM_RIGHT].x += wall_width *
                                                                   j;
    }
  }
}

/******************************************************************************
   Function: maze_init

Description: Initializes the global maze struct by setting its width and height
             according to the current maze size value then setting entrance and
             exit points and procedurally carving a path between them. Also
             sets a starting direction, repositions the player, and sets the
             number of seconds spent in the maze to zero.

     Inputs: None.

    Outputs: None.
******************************************************************************/
void maze_init(void)
{
  int16_t i, j;
  GPoint exit, maze_carver_position;
  int16_t maze_carver_direction;

  // Determine width and height:
  g_maze->width = rand() % (MAX_MAZE_WIDTH - MIN_MAZE_WIDTH + 1) +
                  MIN_MAZE_WIDTH;
  g_maze->height = rand() % (MAX_MAZE_HEIGHT - MIN_MAZE_HEIGHT + 1) +
                   MIN_MAZE_HEIGHT;

  // Set all cells to "solid":
  for (i = 0; i < g_maze->width; ++i)
  {
    for (j = 0; j < g_maze->height; ++j)
    {
      g_maze->cells[i][j] = SOLID;
    }
  }

  // Now, set "entrance" and "exit" points and carve a path between them:
  switch (rand() % NUM_DIRECTIONS)
  {
    case NORTH:
      maze_carver_position = RANDOM_POINT_NORTH;
      exit = RANDOM_POINT_SOUTH;
      break;
    case SOUTH:
      maze_carver_position = RANDOM_POINT_SOUTH;
      exit = RANDOM_POINT_NORTH;
      break;
    case EAST:
      maze_carver_position = RANDOM_POINT_EAST;
      exit = RANDOM_POINT_WEST;
      break;
    default: // case WEST:
      maze_carver_position = RANDOM_POINT_WEST;
      exit = RANDOM_POINT_EAST;
      break;
  }
  g_maze->cells[maze_carver_position.x][maze_carver_position.y] = ENTRANCE;
  g_maze->entrance = maze_carver_position;
  g_maze->cells[exit.x][exit.y] = EXIT;
  maze_carver_direction = rand() % NUM_DIRECTIONS;
  while (!gpoint_equal(&maze_carver_position, &exit))
  {
    if (get_cell_type(maze_carver_position) != ENTRANCE)
    {
      g_maze->cells[maze_carver_position.x][maze_carver_position.y] = EMPTY;
    }
    switch(maze_carver_direction)
    {
      case NORTH:
        if (maze_carver_position.y > 0)
        {
          maze_carver_position.y--;
        }
        break;
      case SOUTH:
        if (maze_carver_position.y < g_maze->height - 1)
        {
          maze_carver_position.y++;
        }
        break;
      case EAST:
        if (maze_carver_position.x < g_maze->width - 1)
        {
          maze_carver_position.x++;
        }
        break;
      default: // case WEST:
        if (maze_carver_position.x > 0)
        {
          maze_carver_position.x--;
        }
        break;
    }
    if (rand() % NUM_DIRECTIONS == 0) // 25% chance of turning.
    {
      maze_carver_direction = rand() % NUM_DIRECTIONS;
    }
  }
  set_maze_starting_direction();
  reposition_player();
  g_maze->seconds = 0;
}

/******************************************************************************
   Function: set_maze_starting_direction

Description: Finds a viable starting direction (i.e., not facing a wall) for
             the current maze. (There should always be a viable direction, but
             if there isn't, the last direction checked will be selected.)

     Inputs: None.

    Outputs: Value representing the selected starting direction.
******************************************************************************/
int16_t set_maze_starting_direction(void)
{
  int16_t i;
  bool checked_direction[NUM_DIRECTIONS];

  for (i = 0; i < NUM_DIRECTIONS; ++i)
  {
    checked_direction[i] = false;
  }
  do
  {
    g_maze->starting_direction = rand() % NUM_DIRECTIONS;
    checked_direction[g_maze->starting_direction] = true;
  }while (is_solid(get_cell_farther_away(g_maze->entrance,
                                         g_maze->starting_direction,
                                         1)) &&
          !(checked_direction[NORTH] &&
            checked_direction[SOUTH] &&
            checked_direction[EAST]  &&
            checked_direction[WEST]));

  return g_maze->starting_direction;
}

/******************************************************************************
   Function: draw_scene

Description: Draws a (simplistic) 3D scene based on the player's current
             position, direction, and visibility depth.

     Inputs: layer - Pointer to the relevant layer.
             ctx   - Pointer to the relevant graphics context.

    Outputs: None.
******************************************************************************/
void draw_scene(Layer *layer, GContext *ctx)
{
  int16_t i, depth;
  GPoint cell_coords, cell_coords2;

  // First, draw a black background:
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx,
                     layer_get_bounds(layer),
                     NO_CORNER_RADIUS,
                     GCornerNone);

  // Next, draw the floor, ceiling, and walls of the maze:
  draw_floor_and_ceiling(ctx);
  for (depth = MAX_VISIBILITY_DEPTH - 1; depth >= 0; --depth)
  {
    // Draw the walls straight ahead at the current depth:
    cell_coords = get_cell_farther_away(g_player->position,
                                        g_player->direction,
                                        depth);
    if (out_of_bounds(cell_coords))
    {
      continue;
    }
    draw_cell_contents(ctx, cell_coords, depth, STRAIGHT_AHEAD);

    // Now, draw all cells to the left and right at the same depth:
    for (i = 1; i <= depth + 1; ++i)
    {
      cell_coords2 = get_cell_to_the_left(cell_coords,
                                          g_player->direction,
                                          i);
      draw_cell_contents(ctx, cell_coords2, depth, STRAIGHT_AHEAD - i);
      cell_coords2 = get_cell_to_the_right(cell_coords,
                                           g_player->direction,
                                           i);
      draw_cell_contents(ctx, cell_coords2, depth, STRAIGHT_AHEAD + i);
    }
  }

  // Finally, update the lower status bar:
  update_status_bar(ctx);
}

/******************************************************************************
   Function: draw_floor_and_ceiling

Description: Draws the floor and ceiling.

     Inputs: ctx - Pointer to the relevant graphics context.

    Outputs: None.
******************************************************************************/
void draw_floor_and_ceiling(GContext *ctx)
{
  int16_t x, y, max_y, shading_offset;

  x = 2;
  max_y = g_back_wall_coords[MAX_VISIBILITY_DEPTH - x]
                            [STRAIGHT_AHEAD]
                            [TOP_LEFT].y;
  while (max_y > GRAPHICS_FRAME_HEIGHT / 2 - MIN_WALL_HEIGHT / 2 &&
         x <= MAX_VISIBILITY_DEPTH)
  {
    max_y = g_back_wall_coords[MAX_VISIBILITY_DEPTH - ++x]
                              [STRAIGHT_AHEAD]
                              [TOP_LEFT].y;
  }
  graphics_context_set_stroke_color(ctx, GColorWhite);
  for (y = 0; y < max_y; ++y)
  {
    // Determine horizontal distance between points:
    shading_offset = 1 + y / MAX_VISIBILITY_DEPTH;
    if (y % MAX_VISIBILITY_DEPTH >= MAX_VISIBILITY_DEPTH / 2 + MAX_VISIBILITY_DEPTH % 2)
    {
      shading_offset++;
    }
    for (x = y % 2 ? 0 : (shading_offset / 2) + (shading_offset % 2);
         x < SCREEN_WIDTH;
         x += shading_offset)
    {
      // Draw a point on the ceiling:
      graphics_draw_pixel(ctx, GPoint(x, y));
      // Draw a point on the floor:
      graphics_draw_pixel(ctx, GPoint(x, GRAPHICS_FRAME_HEIGHT - y));
    }
  }
}

/******************************************************************************
   Function: draw_cell_contents

Description: Draws walls and other contents for a given cell.

     Inputs: ctx         - Pointer to the relevant graphics context.
             cell_coords - Coordinates of the cell of interest in "g_maze".
             depth       - Front-back visual depth of the cell of interest in
                           "g_back_wall_coords".
             position    - Left-right visual position of the cell of interest
                           in "g_back_wall_coords".

    Outputs: "True" unless the cell is located entirely off-screen.
******************************************************************************/
bool draw_cell_contents(GContext *ctx,
                        const GPoint cell_coords,
                        const int16_t depth,
                        const int16_t position)
{
  int16_t left, right, top, bottom, y_offset;
  GPoint cell_coords2;
  bool back_wall_drawn, left_wall_drawn, right_wall_drawn;

  if (is_solid(cell_coords)             ||
      depth < 0                         ||
      depth >= MAX_VISIBILITY_DEPTH - 1 ||
      position < 0                      ||
      position > STRAIGHT_AHEAD * 2)
  {
    return false;
  }

  // Back wall:
  left = g_back_wall_coords[depth][position][TOP_LEFT].x;
  right = g_back_wall_coords[depth][position][BOTTOM_RIGHT].x;
  top = g_back_wall_coords[depth][position][TOP_LEFT].y;
  bottom = g_back_wall_coords[depth][position][BOTTOM_RIGHT].y;
  if (bottom - top < MIN_WALL_HEIGHT)
  {
    return false;
  }
  back_wall_drawn = left_wall_drawn = right_wall_drawn = false;
  cell_coords2 = get_cell_farther_away(cell_coords,
                                       g_player->direction,
                                       1);
  if (is_solid(cell_coords2))
  {
    draw_wall(ctx,
              GPoint(left, top),
              GPoint(left, bottom),
              GPoint(right, top),
              GPoint(right, bottom));
    back_wall_drawn = true;
  }

  // Left wall:
  right = left;
  if (depth == 0)
  {
    left = 0;
    y_offset = top;
  }
  else
  {
    left = g_back_wall_coords[depth - 1][position][TOP_LEFT].x;
    y_offset = top - g_back_wall_coords[depth - 1][position][TOP_LEFT].y;
  }
  if (position <= STRAIGHT_AHEAD)
  {
    cell_coords2 = get_cell_to_the_left(cell_coords,
                                        g_player->direction,
                                        1);
    if (is_solid(cell_coords2))
    {
      draw_wall(ctx,
                GPoint(left, top - y_offset),
                GPoint(left, bottom + y_offset),
                GPoint(right, top),
                GPoint(right, bottom));
      left_wall_drawn = true;
    }
  }

  // Right wall:
  left = g_back_wall_coords[depth][position][BOTTOM_RIGHT].x;
  if (depth == 0)
  {
    right = SCREEN_WIDTH - 1;
  }
  else
  {
    right = g_back_wall_coords[depth - 1][position][BOTTOM_RIGHT].x;
  }
  if (position >= STRAIGHT_AHEAD)
  {
    cell_coords2 = get_cell_to_the_right(cell_coords,
                                         g_player->direction,
                                         1);
    if (is_solid(cell_coords2))
    {
      draw_wall(ctx,
                GPoint(left, top),
                GPoint(left, bottom),
                GPoint(right, top - y_offset),
                GPoint(right, bottom + y_offset));
      right_wall_drawn = true;
    }
  }

  // Draw vertical lines at corners:
  graphics_context_set_stroke_color(ctx, GColorBlack);
  cell_coords2 = get_cell_farther_away(cell_coords,
                                       g_player->direction,
                                       1);
  if ((back_wall_drawn && (left_wall_drawn ||
       !is_solid(get_cell_to_the_left(cell_coords2,
                                      g_player->direction,
                                      1)))) ||
      (left_wall_drawn &&
       !is_solid(get_cell_to_the_left(cell_coords2,
                                      g_player->direction,
                                      1))))
  {
    graphics_draw_line(ctx,
                       g_back_wall_coords[depth][position][0],
                       GPoint(g_back_wall_coords[depth][position][0].x,
                              g_back_wall_coords[depth][position][1].y));
  }
  if ((back_wall_drawn && (right_wall_drawn ||
       !is_solid(get_cell_to_the_right(cell_coords2,
                                       g_player->direction,
                                       1)))) ||
      (right_wall_drawn &&
       !is_solid(get_cell_to_the_right(cell_coords2,
                                       g_player->direction,
                                       1))))
  {
    graphics_draw_line(ctx,
                       g_back_wall_coords[depth][position][1],
                       GPoint(g_back_wall_coords[depth][position][1].x,
                              g_back_wall_coords[depth][position][0].y));
  }

  // Entrance/exit markers:
  if (get_cell_type(cell_coords) == ENTRANCE)
  {
    draw_entrance(ctx, depth, position);
  }
  else if (get_cell_type(cell_coords) == EXIT)
  {
    draw_exit(ctx, depth, position);
  }

  return true;
}

/******************************************************************************
   Function: draw_wall

Description: Draws a wall according to specifications.

     Inputs: ctx              - Pointer to the relevant graphics context.
             upper_left       - Coordinates for the wall's upper-left point.
             lower_left       - Coordinates for the wall's lower-left point.
             upper_right      - Coordinates for the wall's upper-right point.
             lower_right      - Coordinates for the wall's lower-right point.

    Outputs: "True" if the wall is successfully drawn on the screen (i.e., the
             wall isn't located entirely off-screen).
******************************************************************************/
bool draw_wall(GContext *ctx,
               const GPoint upper_left,
               const GPoint lower_left,
               const GPoint upper_right,
               const GPoint lower_right)
{
  int16_t i, j, dx, dy, shading_offset, half_shading_offset;
  float dy_over_dx;

  if (upper_left.x >= SCREEN_WIDTH || upper_right.x < 0)
  {
    return false;
  }

  // Use "dy" to handle slanted side walls (it's zero for back walls):
  dx = upper_right.x - upper_left.x;
  dy = upper_right.y - upper_left.y; // Negative for right-side walls.
  dy_over_dx = (float) dy / dx;      // Negative for right-side walls.
  for (i = upper_left.x; i <= upper_right.x && i < SCREEN_WIDTH; ++i)
  {
    // Calculate a new shading offset for each "x" value:
    shading_offset = 1 + ((upper_left.y + (i - upper_left.x) * dy_over_dx) /
                          MAX_VISIBILITY_DEPTH);

    // Round up, if applicable:
    if ((int16_t) (upper_left.y + (i - upper_left.x) * dy_over_dx) %
        MAX_VISIBILITY_DEPTH >= MAX_VISIBILITY_DEPTH / 2 + MAX_VISIBILITY_DEPTH % 2)
    {
      shading_offset++;
    }
    half_shading_offset = (shading_offset / 2) + (shading_offset % 2);

    // Now, draw points from top to bottom:
    for (j = upper_left.y + (i - upper_left.x) * dy_over_dx;
         j < lower_left.y - (i - upper_left.x) * dy_over_dx;
         ++j)
    {
      if ((j + (int16_t) ((i - upper_left.x) * dy_over_dx) +
          (i % 2 == 0 ? 0 : half_shading_offset)) % shading_offset == 0)
      {
        graphics_context_set_stroke_color(ctx, GColorWhite);
      }
      else
      {
        graphics_context_set_stroke_color(ctx, GColorBlack);
      }
      graphics_draw_pixel(ctx, GPoint(i, j));
    }
  }

  // Draw lines along the top and bottom of the wall:
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, upper_left, upper_right);
  graphics_draw_line(ctx, lower_left, lower_right);

  // Ad hoc solution to a minor visual issue (remove if no longer relevant):
  if (dy == 0 && upper_left.y == g_back_wall_coords[1][0][TOP_LEFT].y)
  {
    graphics_draw_line(ctx,
                       GPoint(lower_left.x, lower_left.y + 1),
                       GPoint(lower_right.x, lower_right.y + 1));
  }

  return true;
}

/******************************************************************************
   Function: draw_entrance

Description: Draws an entrance graphic on the ceiling of a given cell location.

     Inputs: ctx      - Pointer to the relevant graphics context.
             depth    - Front-back visual depth of the entrance cell in
                        "g_back_wall_coords".
             position - Left-right visual position of the entrance cell in
                        in "g_back_wall_coords".

    Outputs: "True" if the entrance is successfully drawn on the screen (i.e.,
             the entrance isn't located entirely off-screen).
******************************************************************************/
bool draw_entrance(GContext *ctx, const int16_t depth, const int16_t position)
{
  int16_t h_radius, v_radius; // Horizontal and vertical radii for an ellipse.

  h_radius = ELLIPSE_RADIUS_RATIO *
             (g_back_wall_coords[depth][position][1].x -
              g_back_wall_coords[depth][position][0].x);
  if (depth == 0)
  {
    v_radius = ELLIPSE_RADIUS_RATIO *
               g_back_wall_coords[depth][position][0].y;
  }
  else
  {
    v_radius = ELLIPSE_RADIUS_RATIO *
               (g_back_wall_coords[depth][position][0].y -
                g_back_wall_coords[depth - 1][position][0].y);
  }

  return fill_ellipse(ctx,
                      get_ceiling_center_point(depth, position),
                      h_radius,
                      v_radius,
                      GColorBlack);
}

/******************************************************************************
   Function: draw_exit

Description: Draws an exit graphic on the floor of a given cell location.

     Inputs: ctx      - Pointer to the relevant graphics context.
             depth    - Front-back visual depth of the exit cell in
                        "g_back_wall_coords".
             position - Left-right visual position of the exit cell in
                        in "g_back_wall_coords".

    Outputs: "True" if the exit is successfully drawn on the screen (i.e., the
             exit isn't located entirely off-screen).
******************************************************************************/
bool draw_exit(GContext *ctx, const int16_t depth, const int16_t position)
{
  int16_t h_radius, v_radius; // Horizontal and vertical radii for an ellipse.

  h_radius = ELLIPSE_RADIUS_RATIO *
             (g_back_wall_coords[depth][position][1].x -
              g_back_wall_coords[depth][position][0].x);
  if (depth == 0)
  {
    v_radius = ELLIPSE_RADIUS_RATIO *
               (GRAPHICS_FRAME_HEIGHT -
                g_back_wall_coords[depth][position][1].y);
  }
  else
  {
    v_radius = ELLIPSE_RADIUS_RATIO *
               (g_back_wall_coords[depth - 1][position][1].y -
                g_back_wall_coords[depth][position][1].y);
  }

  return fill_ellipse(ctx,
                      get_floor_center_point(depth, position),
                      h_radius,
                      v_radius,
                      GColorBlack);
}

/******************************************************************************
   Function: fill_ellipse

Description: Draws a filled ellipse according to given specifications.

     Inputs: ctx      - Pointer to the relevant graphics context.
             center   - Central coordinates of the ellipse (with respect to the
                        graphics frame).
             h_radius - Horizontal radius.
             v_radius - Vertical radius.
             color    - Desired color ("GColorBlack" or "GColorWhite").

    Outputs: Returns "true" if the ellipse is successfully drawn on the screen
             (i.e., the ellipse isn't located entirely off-screen).
******************************************************************************/
bool fill_ellipse(GContext *ctx,
                  const GPoint center,
                  const int16_t h_radius,
                  const int16_t v_radius,
                  const GColor color)
{
  int32_t theta;
  int16_t x_offset, y_offset;

  if (center.x + h_radius < 0 ||
      center.x - h_radius >= SCREEN_WIDTH ||
      center.y + v_radius < 0 ||
      center.y - v_radius >= GRAPHICS_FRAME_HEIGHT)
  {
    return false;
  }

  graphics_context_set_stroke_color(ctx, color);
  for (theta = 0; theta < NINETY_DEGREES; theta += DEFAULT_ROTATION_RATE)
  {
    x_offset = cos_lookup(theta) * h_radius / TRIG_MAX_RATIO;
    y_offset = sin_lookup(theta) * v_radius / TRIG_MAX_RATIO;
    graphics_draw_line(ctx,
                       GPoint(center.x - x_offset, center.y - y_offset),
                       GPoint(center.x + x_offset, center.y - y_offset));
    graphics_draw_line(ctx,
                       GPoint(center.x - x_offset, center.y + y_offset),
                       GPoint(center.x + x_offset, center.y + y_offset));
  }

  return true;
}

/******************************************************************************
   Function: show_message_box

Description: Updates and displays the message box.

     Inputs: None.

    Outputs: None.
******************************************************************************/
void show_message_box(void)
{
  text_layer_set_text(g_message_box_text_layer, g_message_str);
  window_stack_push(g_message_box_window, false /* not animated */);
}

/******************************************************************************
   Function: tick_handler

Description: Handles changes to the game world every second while in active
             gameplay.

     Inputs: tick_time     - Pointer to the relevant time struct.
             units_changed - Indicates which time unit changed.

    Outputs: None.
******************************************************************************/
static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  int16_t new_achievement_index; // To check for pending achievement messages.

  if (!g_game_paused)
  {
    g_maze->seconds++;
    if (g_maze->seconds > MAX_SECONDS)
    {
      g_maze->seconds = MAX_SECONDS;
      if (!g_player->achievement_unlocked[ONE_HOUR_ACHIEVEMENT])
	  {
        g_player->achievement_unlocked[ONE_HOUR_ACHIEVEMENT] = true;
        g_new_achievement_unlocked[ONE_HOUR_ACHIEVEMENT] = true;
        //save_game_data();
      }
    }
    layer_mark_dirty(window_get_root_layer(g_graphics_window));

    // Check for pending time/achievement messages:
    if (g_new_best_time > -1)
	{
      strcpy(g_message_str, "\nNew Best Time!\n");
      cat_time_onto_str(g_message_str, g_new_best_time);
      show_message_box();
      g_new_best_time = -1;
    }
    else if ((new_achievement_index = get_new_achievement_index()) > -1)
	{
      strcpy(g_message_str, "Achievement Unlocked!\n\n\"");
      strcat(g_message_str, get_achievement_name(new_achievement_index));
      strcat(g_message_str, "\"");
      show_message_box();
      g_new_achievement_unlocked[new_achievement_index] = false;
    }
  }
}

/******************************************************************************
   Function: app_focus_handler

Description: Handles MazeCrawler going out of, or coming back into, focus
             (e.g., when a notification window temporarily hides this app).

     Inputs: in_focus - "True" if MazeCrawler is now in focus.

    Outputs: None.
******************************************************************************/
void app_focus_handler(bool in_focus)
{
  if (!in_focus) // Some other app has taken the focus away from MazeCrawler.
  {
    g_game_paused = true;
  }
  else // MazeCrawler is now back in focus.
  {
    if (window_stack_get_top_window() == g_graphics_window)
    {
      g_game_paused = false;
    }
  }
}

/******************************************************************************
   Function: graphics_window_appear

Description: Called when the graphics window appears.

     Inputs: window - Pointer to the graphics window.

    Outputs: None.
******************************************************************************/
static void graphics_window_appear(Window *window)
{
  g_game_paused = false;
}

/******************************************************************************
   Function: graphics_window_disappear

Description: Called when the graphics window disappears.

     Inputs: window - Pointer to the graphics window.

    Outputs: None.
******************************************************************************/
static void graphics_window_disappear(Window *window)
{
  g_game_paused = true;
}

/******************************************************************************
   Function: graphics_window_load

Description: Handles the graphics window's loading process.

     Inputs: window - Pointer to the graphics window.

    Outputs: None.
******************************************************************************/
static void graphics_window_load(Window *window)
{
  window_set_click_config_provider(window,
                                   (ClickConfigProvider)
                                   graphics_click_config_provider);
  layer_set_update_proc(window_get_root_layer(window), draw_scene);

  // Initialize the status bar's text layers:
  g_level_text_layer = text_layer_create(LEVEL_TEXT_LAYER_FRAME);
  text_layer_set_background_color(g_level_text_layer, GColorClear);
  text_layer_set_text_color(g_level_text_layer, GColorWhite);
  text_layer_set_font(g_level_text_layer, STATUS_BAR_FONT);
  text_layer_set_text_alignment(g_level_text_layer, GTextAlignmentLeft);
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(g_level_text_layer));
  g_time_text_layer = text_layer_create(TIME_TEXT_LAYER_FRAME);
  text_layer_set_background_color(g_time_text_layer, GColorClear);
  text_layer_set_text_color(g_time_text_layer, GColorWhite);
  text_layer_set_font(g_time_text_layer, STATUS_BAR_FONT);
  text_layer_set_text_alignment(g_time_text_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window),
                  text_layer_get_layer(g_time_text_layer));

  // Subscribe to the tick timer:
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

/******************************************************************************
   Function: graphics_window_unload

Description: Handles the graphics window's unloading process.

     Inputs: window - Pointer to the graphics window.

    Outputs: None.
******************************************************************************/
static void graphics_window_unload(Window *window)
{
  tick_timer_service_unsubscribe();
  text_layer_destroy(g_level_text_layer);
  text_layer_destroy(g_time_text_layer);
}

/******************************************************************************
   Function: graphics_up_single_repeating_click

Description: The graphics window's single-click handler for the Pebble's "up"
             button. Moves the player one cell forward.

     Inputs: recognizer - The click recognizer.
             context    - Pointer to the associated context.

    Outputs: None.
******************************************************************************/
void graphics_up_single_repeating_click(ClickRecognizerRef recognizer,
                                        void *context)
{
  if (!g_game_paused)
  {
    move_player(g_player->direction);
  }
}

/******************************************************************************
   Function: graphics_up_multi_click

Description: The graphics window's multi-click handler for the "up" button.
             Turns the player to the left.

     Inputs: recognizer - The click recognizer.
             context    - Pointer to the associated context.

    Outputs: None.
******************************************************************************/
void graphics_up_multi_click(ClickRecognizerRef recognizer, void *context)
{
  if (!g_game_paused)
  {
    // Turn to the left:
    switch (g_player->direction)
    {
      case NORTH:
        g_player->direction = WEST;
        break;
      case WEST:
        g_player->direction = SOUTH;
        break;
      case SOUTH:
        g_player->direction = EAST;
        break;
      default: // case: EAST
        g_player->direction = NORTH;
        break;
    }
    update_compass();
    layer_mark_dirty(window_get_root_layer(g_graphics_window));
  }
}

/******************************************************************************
   Function: graphics_down_single_repeating_click

Description: The graphics window's single-click handler for the "down" button.
             Moves the player one cell backward.

     Inputs: recognizer - The click recognizer.
             context    - Pointer to the associated context.

    Outputs: None.
******************************************************************************/
void graphics_down_single_repeating_click(ClickRecognizerRef recognizer,
                                          void *context)
{
  if (!g_game_paused)
  {
    move_player(get_opposite_direction(g_player->direction));
  }
}

/******************************************************************************
   Function: graphics_down_multi_click

Description: The graphics window's multi-click handler for the "down" button.
             Turns the player to the right.

     Inputs: recognizer - The click recognizer.
             context    - Pointer to the associated context.

    Outputs: None.
******************************************************************************/
void graphics_down_multi_click(ClickRecognizerRef recognizer, void *context)
{
  if (!g_game_paused)
  {
    // Turn to the right:
    switch (g_player->direction)
    {
      case NORTH:
        g_player->direction = EAST;
        break;
      case EAST:
        g_player->direction = SOUTH;
        break;
      case SOUTH:
        g_player->direction = WEST;
        break;
      default: // case: WEST
        g_player->direction = NORTH;
        break;
    }
    update_compass();
    layer_mark_dirty(window_get_root_layer(g_graphics_window));
  }
}

/******************************************************************************
   Function: graphics_select_single_click

Description: The graphics window's single-click handler for the "select"
             button. Opens the in-game menu.

     Inputs: recognizer - The click recognizer.
             context    - Pointer to the associated context.

    Outputs: None.
******************************************************************************/
void graphics_select_single_click(ClickRecognizerRef recognizer, void *context)
{
  /*switch (g_player->level)
  {
    case 1:
      g_player->level = 9;
      break;
    case 9:
      g_player->level = 99;
      break;
    case 99:
      g_player->level = 999;
      break;
    case 999:
      g_player->level = 9999;
      break;
    default:
      g_player->level = 1;
      break;
  }*/
  menu_layer_set_selected_index(g_in_game_menu,
                                (MenuIndex) {0, 0},
                                MenuRowAlignTop,
                                false /* not animated */);
  window_stack_push(g_in_game_menu_window, false /* not animated */);
}

/******************************************************************************
   Function: graphics_click_config_provider

Description: Button-click configuration provider for the graphics window.

     Inputs: context - Pointer to the associated context.

    Outputs: None.
******************************************************************************/
void graphics_click_config_provider(void *context)
{
  // "Up" button:
  window_single_repeating_click_subscribe(BUTTON_ID_UP,
                                          CLICK_REPEAT_INTERVAL,
                                          graphics_up_single_repeating_click);
  window_multi_click_subscribe(BUTTON_ID_UP,
                               MULTI_CLICK_MIN,
                               MULTI_CLICK_MAX,
                               MULTI_CLICK_TIMEOUT,
                               LAST_CLICK_ONLY,
                               graphics_up_multi_click);

  // "Down" button:
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN,
                                          CLICK_REPEAT_INTERVAL,
                                          graphics_down_single_repeating_click);
  window_multi_click_subscribe(BUTTON_ID_DOWN,
                               MULTI_CLICK_MIN,
                               MULTI_CLICK_MAX,
                               MULTI_CLICK_TIMEOUT,
                               LAST_CLICK_ONLY,
                               graphics_down_multi_click);

  // "Select" button:
  window_single_click_subscribe(BUTTON_ID_SELECT,
                                graphics_select_single_click);
}

/******************************************************************************
   Function: message_box_select_single_click

Description: The message box window's single-click handler for the "select"
             button. Closes the message box, effectively resuming gameplay.

     Inputs: recognizer - The click recognizer.
             context    - Pointer to the associated context.

    Outputs: None.
******************************************************************************/
void message_box_select_single_click(ClickRecognizerRef recognizer,
                                     void *context)
{
  window_stack_pop(false /* not animated */);
}

/******************************************************************************
   Function: message_box_click_config_provider

Description: Button-click configurations for the message box window.

     Inputs: context - Pointer to the associated context.

    Outputs: None.
******************************************************************************/
void message_box_click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_SELECT,
                                message_box_select_single_click);
}

/******************************************************************************
   Function: narration_select_single_click

Description: The narration window's single-click handler for the "select"
             button. Either the next page of narration text will be displayed
             or the narration window will be closed.

     Inputs: recognizer - The click recognizer.
             context    - Pointer to the associated context.

    Outputs: None.
******************************************************************************/
void narration_select_single_click(ClickRecognizerRef recognizer,
                                   void *context)
{
  g_narration_page_num++;
  show_narration();
}

/******************************************************************************
   Function: narration_click_config_provider

Description: Button-click configurations for the narration window.

     Inputs: context - Pointer to the associated context.

    Outputs: None.
******************************************************************************/
void narration_click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_SELECT,
                                narration_select_single_click);
}

/******************************************************************************
   Function: menu_get_num_rows_callback

Description: Returns the number of rows in a given menu.

     Inputs: menu_layer    - Pointer to the menu of interest.
             section_index - Section number (all the menus in MazeCrawler have
                             only one section).
             data          - Pointer to additional data (not used).

    Outputs: The number of rows in the indicated menu.
******************************************************************************/
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer,
                                           uint16_t section_index,
                                           void *data)
{
  if (menu_layer == g_main_menu)
  {
    return MAIN_MENU_NUM_ROWS;
  }

  return IN_GAME_MENU_NUM_ROWS;
}

/******************************************************************************
   Function: main_menu_draw_row_callback

Description: Instructions for drawing each row of the main menu.

     Inputs: ctx        - Pointer to the associated context.
             cell_layer - Pointer to the layer of the cell to be drawn.
             cell_index - Pointer to the index struct of the cell to be drawn.
             data       - Pointer to additional data (not used).

    Outputs: None.
******************************************************************************/
static void main_menu_draw_row_callback(GContext* ctx,
                                        const Layer *cell_layer,
                                        MenuIndex *cell_index,
                                        void *data)
{
  switch (cell_index->row)
  {
    case 0:
      menu_cell_basic_draw(ctx,
                           cell_layer,
                           "Play",
                           "Enter the labyrinth!",
                           NULL);
      break;
    case 1:
      menu_cell_basic_draw(ctx,
                           cell_layer,
                           "Stats",
                           "Your accomplishments.",
                           NULL);
      break;
    case 2:
      menu_cell_basic_draw(ctx,
                           cell_layer,
                           "Controls",
                           "How to play.",
                           NULL);
      break;
    default:
      menu_cell_basic_draw(ctx,
                           cell_layer,
                           "About",
                           "Credits, etc.",
                           NULL);
      break;
  }
}

/******************************************************************************
   Function: main_menu_select_callback

Description: Called when a given cell of the main menu is selected.

     Inputs: menu_layer - Pointer to the menu layer.
             cell_index - Pointer to the index struct of the selected cell.
             data       - Pointer to additional data (not used).

    Outputs: None.
******************************************************************************/
void main_menu_select_callback(MenuLayer *menu_layer,
                               MenuIndex *cell_index,
                               void *data)
{
  switch (cell_index->row)
  {
    case 0: // Play
      window_stack_push(g_graphics_window, false /* not animated */);
      break;
    case 1: // Stats
      g_current_narration = STATS_NARRATION;
      show_narration();
      break;
    case 2: // Controls
      g_current_narration = CONTROLS_NARRATION;
      show_narration();
      break;
    default: // About
      g_current_narration = GAME_INFO_NARRATION;
      show_narration();
      break;
  }
}

/******************************************************************************
   Function: in_game_menu_draw_row_callback

Description: Instructions for drawing each row of the in-game menu.

     Inputs: ctx        - Pointer to the associated context.
             cell_layer - Pointer to the layer of the cell to be drawn.
             cell_index - Pointer to the index struct of the cell to be drawn.
             data       - Pointer to additional data (not used).

    Outputs: None.
******************************************************************************/
static void in_game_menu_draw_row_callback(GContext* ctx,
                                           const Layer *cell_layer,
                                           MenuIndex *cell_index,
                                           void *data)
{
  switch (cell_index->row)
  {
    case 0:
      menu_cell_basic_draw(ctx,
                           cell_layer,
                           "New Maze",
                           "Skip current maze.",
                           NULL);
      break;
    case 1:
      menu_cell_basic_draw(ctx,
                           cell_layer,
                           "Stats",
                           "Your accomplishments.",
                           NULL);
      break;
    case 2:
      menu_cell_basic_draw(ctx,
                           cell_layer,
                           "Controls",
                           "Learn how to play.",
                           NULL);
      break;
    default:
      menu_cell_basic_draw(ctx,
                           cell_layer,
                           "About",
                           "Credits, etc.",
                           NULL);
      break;
  }
}

/******************************************************************************
   Function: in_game_menu_select_callback

Description: Called when a given cell of the in-game menu is selected.

     Inputs: menu_layer - Pointer to the menu layer.
             cell_index - Pointer to the index struct of the selected cell.
             data       - Pointer to additional data (not used).

    Outputs: None.
******************************************************************************/
void in_game_menu_select_callback(MenuLayer *menu_layer,
                                  MenuIndex *cell_index,
                                  void *data)
{
  switch (cell_index->row)
  {
    case 0: // New Maze
      maze_init();
      window_stack_pop(false /* not animated */);
      break;
    case 1: // Stats
      g_current_narration = STATS_NARRATION;
      show_narration();
      break;
    case 2: // Controls
      g_current_narration = CONTROLS_NARRATION;
      show_narration();
      break;
    default: // About
      g_current_narration = GAME_INFO_NARRATION;
      show_narration();
      break;
  }
}

/******************************************************************************
   Function: get_achievement_name

Description: Given the integer ID of an achievement, returns that achievement's
             name as a string.

     Inputs: achievement_num - Value representing the achievement of interest.

    Outputs: Pointer to the name of the achievement.
******************************************************************************/
char *get_achievement_name(const int16_t achievement_num)
{
  static char name_str[ACHIEVEMENT_NAME_STR_LEN + 1];

  switch(achievement_num)
  {
    case FIRST_LEVEL_ACHIEVEMENT:
      strcpy(name_str, "Novice");
      break;
    case LEVEL_10_ACHIEVEMENT:
      strcpy(name_str, "Apprentice");
      break;
    case LEVEL_50_ACHIEVEMENT:
      strcpy(name_str, "Journeyman");
      break;
    case LEVEL_100_ACHIEVEMENT:
      strcpy(name_str, "Master");
      break;
    case LEVEL_500_ACHIEVEMENT:
      strcpy(name_str, "Dedicated");
      break;
    case LEVEL_1000_ACHIEVEMENT:
      strcpy(name_str, "Devoted");
      break;
    case LEVEL_5000_ACHIEVEMENT:
      strcpy(name_str, "Obsessed");
      break;
    case MAX_LEVEL_ACHIEVEMENT:
      strcpy(name_str, "Fanatical");
      break;
    case MAX_POINTS_ACHIEVEMENT:
      strcpy(name_str, "Addicted");
      break;
    case UNDER_THIRTY_SECONDS_ACHIEVEMENT:
      strcpy(name_str, "Speedy");
      break;
    case UNDER_TEN_SECONDS_ACHIEVEMENT:
      strcpy(name_str, "Super Speedy");
      break;
    case ONE_HOUR_ACHIEVEMENT:
      strcpy(name_str, "Fell Asleep");
      break;
    default:
      strcpy(name_str, "");
      break;
  }

  return name_str;
}

/******************************************************************************
   Function: get_achievement_description

Description: Given the integer ID of an achievement, returns that achievement's
             past-tense description as a string.

     Inputs: achievement_num - Value representing the achievement of interest.

    Outputs: Pointer to the description of the achievement.
******************************************************************************/
char *get_achievement_description(const int16_t achievement_num)
{
  static char description_str[ACHIEVEMENT_DESC_STR_LEN + 1];

  switch(achievement_num)
  {
    case FIRST_LEVEL_ACHIEVEMENT:
      strcpy(description_str, "Completed your first maze!");
      break;
    case LEVEL_10_ACHIEVEMENT:
      strcpy(description_str, "Reached level 10!");
      break;
    case LEVEL_50_ACHIEVEMENT:
      strcpy(description_str, "Reached level 50!");
      break;
    case LEVEL_100_ACHIEVEMENT:
      strcpy(description_str, "Reached level 100!");
      break;
    case LEVEL_500_ACHIEVEMENT:
      strcpy(description_str, "Reached level 500!");
      break;
    case LEVEL_1000_ACHIEVEMENT:
      strcpy(description_str, "Reached level 1000!");
      break;
    case LEVEL_5000_ACHIEVEMENT:
      strcpy(description_str, "Reached level 5000!");
      break;
    case MAX_LEVEL_ACHIEVEMENT:
      strcpy(description_str, "Completed level 9999!");
      break;
    case MAX_POINTS_ACHIEVEMENT:
      strcpy(description_str, "Reached the max. number of points!");
      break;
    case UNDER_THIRTY_SECONDS_ACHIEVEMENT:
      strcpy(description_str, "Completed a maze in < 30 seconds!");
      break;
    case UNDER_TEN_SECONDS_ACHIEVEMENT:
      strcpy(description_str, "Completed a maze in < 10 seconds!");
      break;
    case ONE_HOUR_ACHIEVEMENT:
      strcpy(description_str, "In a maze for one hour!");
      break;
    default:
      strcpy(description_str, "");
      break;
  }

  return description_str;
}

/******************************************************************************
   Function: get_num_achievements_unlocked

Description: Returns the number of achievements the player has unlocked.

     Inputs: None.

    Outputs: The number of achievements the player has unlocked.
******************************************************************************/
int16_t get_num_achievements_unlocked(void)
{
  int16_t i, count;

  for (i = 0, count = 0; i < NUM_ACHIEVEMENTS; ++i)
  {
    if (g_player->achievement_unlocked[i])
	{
      count++;
	}
  }

  return count;
}

/******************************************************************************
   Function: get_new_achievement_index

Description: Checks for new unlocked achievements, returning the index of the
             first one found. If none are found, -1 is returned instead.

     Inputs: None.

    Outputs: Index of the first new achievement found or -1 if none are found.
******************************************************************************/
int16_t get_new_achievement_index(void)
{
  int16_t i;

  for (i = 0; i < NUM_ACHIEVEMENTS; ++i)
  {
    if (g_new_achievement_unlocked[i])
    {
      return i;
    }
  }

  return -1;
}

/******************************************************************************
   Function: get_floor_center_point

Description: Returns the central point, with respect to the graphics layer, of
             the floor of the cell at a given visual depth and position.

     Inputs: depth    - Front-back visual depth in "g_back_wall_coords".
             position - Left-right visual position in "g_back_wall_coords".

    Outputs: GPoint coordinates of the floor's central point within the
             designated cell.
******************************************************************************/
GPoint get_floor_center_point(const int16_t depth, const int16_t position)
{
  int16_t x_midpoint1, x_midpoint2, x, y;

  x_midpoint1 = 0.5 * (g_back_wall_coords[depth][position][TOP_LEFT].x +
                       g_back_wall_coords[depth][position][BOTTOM_RIGHT].x);
  if (depth == 0)
  {
    if (position < STRAIGHT_AHEAD) // To the left of the player.
    {
      x_midpoint2 = -0.5 * SCREEN_WIDTH;
    }
    else if (position > STRAIGHT_AHEAD) // To the right of the player.
    {
      x_midpoint2 = 1.5 * SCREEN_WIDTH;
    }
    else // Directly under the player.
    {
      x_midpoint2 = x_midpoint1;
    }
    y = GRAPHICS_FRAME_HEIGHT;
  }
  else
  {
    x_midpoint2 = 0.5 *
      (g_back_wall_coords[depth - 1][position][TOP_LEFT].x +
       g_back_wall_coords[depth - 1][position][BOTTOM_RIGHT].x);
    y = 0.5 * (g_back_wall_coords[depth][position][BOTTOM_RIGHT].y +
               g_back_wall_coords[depth - 1][position][BOTTOM_RIGHT].y);
  }
  x = 0.5 * (x_midpoint1 + x_midpoint2);

  return GPoint(x, y);
}

/******************************************************************************
   Function: get_ceiling_center_point

Description: Returns the central point, with respect to the graphics layer, of
             the ceiling of the cell at a given visual depth and position.

     Inputs: depth    - Front-back visual depth in "g_back_wall_coords".
             position - Left-right visual position in "g_back_wall_coords".

    Outputs: GPoint coordinates of the ceiling's central point within the
             designated cell.
******************************************************************************/
GPoint get_ceiling_center_point(const int16_t depth, const int16_t position)
{
  GPoint floor_center = get_floor_center_point(depth, position);

  return GPoint(floor_center.x, GRAPHICS_FRAME_HEIGHT - floor_center.y);
}

/******************************************************************************
   Function: get_cell_farther_away

Description: Given a set of cell coordinates, returns new cell coordinates a
             given distance farther away in a given direction.

     Inputs: reference_point - Reference cell coordinates.
             direction       - Direction of interest.
             distance        - How far back we want to go.

    Outputs: Cell coordinates a given distance farther away from those passed
             in.
******************************************************************************/
GPoint get_cell_farther_away(const GPoint reference_point,
                             const int16_t direction,
                             const int16_t distance)
{
  switch(direction)
  {
    case NORTH:
      return GPoint(reference_point.x, reference_point.y - distance);
      break;
    case SOUTH:
      return GPoint(reference_point.x, reference_point.y + distance);
      break;
    case EAST:
      return GPoint(reference_point.x + distance, reference_point.y);
      break;
    default: // case WEST:
      return GPoint(reference_point.x - distance, reference_point.y);
      break;
  }
}

/******************************************************************************
   Function: get_cell_to_the_left

Description: Given a set of cell coordinates, returns new cell coordinates a
             given distance to the left with respect to a given perspective.

     Inputs: reference_point     - Reference cell coordinates.
             reference_direction - Direction from which to go left.
             distance            - How far to the left we want to go.

    Outputs: Cell coordinates a given distance to the left of those passed in.
******************************************************************************/
GPoint get_cell_to_the_left(const GPoint reference_point,
                            const int16_t reference_direction,
                            const int16_t distance)
{
  switch(reference_direction)
  {
    case NORTH:
      return GPoint(reference_point.x - distance, reference_point.y);
      break;
    case SOUTH:
      return GPoint(reference_point.x + distance, reference_point.y);
      break;
    case EAST:
      return GPoint(reference_point.x, reference_point.y - distance);
      break;
    default: // case WEST:
      return GPoint(reference_point.x, reference_point.y + distance);
      break;
  }
}

/******************************************************************************
   Function: get_cell_to_the_right

Description: Given a set of cell coordinates, returns new cell coordinates a
             given distance to the right with respect to a given perspective.

     Inputs: reference_point     - Reference cell coordinates.
             reference_direction - Direction from which to go right.
             distance            - How far to the right we want to go.

    Outputs: Cell coordinates a given distance to the right of those passed in.
******************************************************************************/
GPoint get_cell_to_the_right(const GPoint reference_point,
                             const int16_t reference_direction,
                             const int16_t distance)
{
  switch(reference_direction)
  {
    case NORTH:
      return GPoint(reference_point.x + distance, reference_point.y);
      break;
    case SOUTH:
      return GPoint(reference_point.x - distance, reference_point.y);
      break;
    case EAST:
      return GPoint(reference_point.x, reference_point.y + distance);
      break;
    default: // case WEST:
      return GPoint(reference_point.x, reference_point.y - distance);
      break;
  }
}

/******************************************************************************
   Function: get_cell_type

Description: Given a set of cell coordinates, returns the cell's type.

     Inputs: cell_coords - Coordinates of the cell of interest.

    Outputs: Integer representing the cell's type.
******************************************************************************/
int16_t get_cell_type(GPoint cell_coords)
{
  return g_maze->cells[cell_coords.x][cell_coords.y];
}

/******************************************************************************
   Function: out_of_bounds

Description: Determines whether a given cell lies outside the current maze
             boundaries.

     Inputs: cell_coords - Coordinates of the cell of interest.

    Outputs: Returns "true" if the cell is out of bounds.
******************************************************************************/
bool out_of_bounds(const GPoint cell_coords)
{
  return cell_coords.x < 0 ||
         cell_coords.x >= g_maze->width ||
         cell_coords.y < 0 ||
         cell_coords.y >= g_maze->height;
}

/******************************************************************************
   Function: is_solid

Description: Determines whether a given cell is "solid" (and is thus impassable
             and should have walls drawn around it). Out-of-bounds cells are
             considered solid.

     Inputs: cell_coords - Coordinates of the cell of interest.

    Outputs: Returns "true" if the cell is solid.
******************************************************************************/
bool is_solid(const GPoint cell_coords)
{
  return out_of_bounds(cell_coords) || get_cell_type(cell_coords) == SOLID;
}

/******************************************************************************
   Function: get_opposite_direction

Description: Returns the opposite of a given direction value (i.e., given the
             argument "NORTH", "SOUTH" will be returned).

     Inputs: direction - The direction whose opposite is desired.

    Outputs: Integer representing the opposite of the given direction.
******************************************************************************/
int16_t get_opposite_direction(const int16_t direction)
{
  switch(direction)
  {
    case NORTH:
      return SOUTH;
      break;
    case SOUTH:
      return NORTH;
      break;
    case EAST:
      return WEST;
      break;
    default: // case WEST:
      return EAST;
      break;
  }
}

/******************************************************************************
   Function: cat_int_onto_str

Description: Concatenates an integer value onto the end of a string. If the
             integer is negative, a minus sign will be included. If the
             absolute value of the integer is greater than MAX_LARGE_INT_VALUE, the
             integer will be assigned an absolute value of MAX_LARGE_INT_VALUE.

     Inputs: dest_str - Pointer to the destination string.
             integer  - Integer value to be converted to characters and
                        appended to the string.

    Outputs: None.
******************************************************************************/
void cat_int_onto_str(char *dest_str, int32_t integer)
{
  int16_t i, j;
  static char int_str[MAX_LARGE_INT_DIGITS + 1];
  bool negative = false;

  int_str[0] = '\0';
  if (integer < 0)
  {
    negative = true;
    integer *= -1;
  }
  if (integer > MAX_LARGE_INT_VALUE)
  {
    integer = MAX_LARGE_INT_VALUE;
  }
  if (integer == 0)
  {
    strcpy(int_str, "0");
  }
  else
  {
    for (i = 0; integer != 0; integer /= 10)
    {
      j = integer % 10;
      int_str[i++] = '0' + j;
    }
    int_str[i] = '\0';
  }

  i = strlen(dest_str) + strlen(int_str);
  if (negative)
  {
    ++i;
  }
  dest_str[i--] = '\0';
  j = 0;
  while (int_str[j] != '\0')
  {
    dest_str[i--] = int_str[j++];
  }
  if (negative)
  {
    dest_str[i--] = '-';
  }
}

/******************************************************************************
   Function: cat_time_onto_str

Description: Concatenates the amount of time spent in the current maze to the
             end of a given string in "MM:SS" format.

     Inputs: dest_str      - Pointer to the destination string.
             total_seconds - Total number of seconds in the time of interest.

    Outputs: Returns "true" if the concatenation is successful.
******************************************************************************/
void cat_time_onto_str(char *dest_str, int16_t total_seconds)
{
  int16_t minutes, remaining_seconds;

  if (total_seconds > MAX_SECONDS)
  {
    total_seconds = MAX_SECONDS;
  }
  minutes = total_seconds / 60;
  remaining_seconds = total_seconds % 60;
  if (minutes < 10)
  {
    cat_int_onto_str(dest_str, 0);
  }
  cat_int_onto_str(dest_str, minutes);
  strcat(dest_str, ":");
  if (remaining_seconds < 10)
  {
    cat_int_onto_str(dest_str, 0);
  }
  cat_int_onto_str(dest_str, remaining_seconds);
}

/******************************************************************************
   Function: init_narration

Description: Initializes the narration window.

     Inputs: None.

    Outputs: None.
******************************************************************************/
void init_narration(void)
{
  if (g_narration_window == NULL)
  {
    g_narration_window = window_create();
    window_set_background_color(g_narration_window, GColorBlack);
    window_set_click_config_provider(g_narration_window,
                                     narration_click_config_provider);
    g_narration_text_layer = text_layer_create(NARRATION_TEXT_LAYER_FRAME);
    text_layer_set_background_color(g_narration_text_layer, GColorBlack);
    text_layer_set_text_color(g_narration_text_layer, GColorWhite);
    text_layer_set_font(g_narration_text_layer, NARRATION_FONT);
    text_layer_set_text_alignment(g_narration_text_layer, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(g_narration_window),
                    text_layer_get_layer(g_narration_text_layer));
    g_narration_page_num = 0;
  }
}

/******************************************************************************
   Function: deinit_narration

Description: Deinitializes the narration window.

     Inputs: None.

    Outputs: None.
******************************************************************************/
void deinit_narration(void)
{
  if (g_narration_window != NULL)
  {
    text_layer_destroy(g_narration_text_layer);
    window_destroy(g_narration_window);
    g_narration_window = NULL;
  }
}

/******************************************************************************
   Function: init

Description: Initializes the MazeCrawler Pebble game.

     Inputs: None.

    Outputs: None.
******************************************************************************/
void init(void)
{
  int16_t i;

  g_game_paused = true;
  srand(time(0));

  // Graphics window initialization:
  g_graphics_window = window_create();
  window_set_background_color(g_graphics_window, GColorBlack);
  window_set_window_handlers(g_graphics_window, (WindowHandlers)
  {
    .load = graphics_window_load,
    .unload = graphics_window_unload,
    .appear = graphics_window_appear,
    .disappear = graphics_window_disappear,
  });

  // Main menu initialization:
  g_main_menu_window = window_create();
  g_main_menu = menu_layer_create(FULL_SCREEN_FRAME);
  menu_layer_set_callbacks(g_main_menu, NULL, (MenuLayerCallbacks)
  {
    .get_num_rows = menu_get_num_rows_callback,
    .draw_row = main_menu_draw_row_callback,
    .select_click = main_menu_select_callback,
  });
  menu_layer_set_click_config_onto_window(g_main_menu, g_main_menu_window);
  layer_add_child(window_get_root_layer(g_main_menu_window),
                  menu_layer_get_layer(g_main_menu));

  // In-game menu initialization:
  g_in_game_menu_window = window_create();
  g_in_game_menu = menu_layer_create(FULL_SCREEN_FRAME);
  menu_layer_set_callbacks(g_in_game_menu, NULL, (MenuLayerCallbacks)
  {
    .get_num_rows = menu_get_num_rows_callback,
    .draw_row = in_game_menu_draw_row_callback,
    .select_click = in_game_menu_select_callback,
  });
  menu_layer_set_click_config_onto_window(g_in_game_menu,
                                          g_in_game_menu_window);
  layer_add_child(window_get_root_layer(g_in_game_menu_window),
                  menu_layer_get_layer(g_in_game_menu));

  // Message box window intialization:
  g_message_box_window = window_create();
  window_set_background_color(g_message_box_window, GColorClear);
  window_set_click_config_provider(g_message_box_window,
                                   message_box_click_config_provider);
  g_message_box_text_layer = text_layer_create(MESSAGE_BOX_FRAME);
  text_layer_set_background_color(g_message_box_text_layer, GColorBlack);
  text_layer_set_text_color(g_message_box_text_layer, GColorWhite);
  text_layer_set_font(g_message_box_text_layer, MESSAGE_BOX_FONT);
  text_layer_set_text_alignment(g_message_box_text_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(g_message_box_window),
                  text_layer_get_layer(g_message_box_text_layer));

  // Focus service subscription:
  app_focus_service_subscribe(app_focus_handler);

  // Misc. variable initialization:
  g_narration_window = NULL;
  g_new_best_time = -1;
  for (i = 0; i < NUM_ACHIEVEMENTS; ++i)
  {
    g_new_achievement_unlocked[i] = false;
  }
  wall_coords_init();
  g_compass_path = gpath_create(&COMPASS_PATH_INFO);
  gpath_move_to(g_compass_path, GPoint(HALF_SCREEN_WIDTH,
                                       GRAPHICS_FRAME_HEIGHT +
                                         STATUS_BAR_HEIGHT / 2));
  g_player = malloc(sizeof(player_t));
  g_maze = malloc(sizeof(maze_t));

  // Present the main menu (and the intro narration, if applicable):
  window_stack_push(g_main_menu_window, true /* animated */);
  if (!load_game_data())
  {
    g_current_narration = INTRO_NARRATION;
    show_narration();
  }
  maze_init();
}

/******************************************************************************
   Function: deinit

Description: Deinitializes the MazeCrawler Pebble game.

     Inputs: None.

    Outputs: None.
******************************************************************************/
void deinit(void)
{
  save_game_data();
  deinit_narration();
  menu_layer_destroy(g_main_menu);
  window_destroy(g_main_menu_window);
  menu_layer_destroy(g_in_game_menu);
  window_destroy(g_in_game_menu_window);
  text_layer_destroy(g_message_box_text_layer);
  window_destroy(g_message_box_window);
  window_destroy(g_graphics_window);
  app_focus_service_unsubscribe();
  free(g_maze);
  free(g_player);
}

/******************************************************************************
   Function: main

Description: Main function for the MazeCrawler Pebble game.

     Inputs: None.

    Outputs: Number of errors encountered.
******************************************************************************/
int main(void)
{
  init();
  app_event_loop();
  deinit();

  return 0;
}
