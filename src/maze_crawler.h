/******************************************************************************
   Filename: maze_crawler.h

     Author: David C. Drake (http://davidcdrake.com)

Description: Header file for MazeCrawler, a first-person 3D maze-navigation
             game developed for the Pebble smartwatch (SDK 3). More information
             available online: http://davidcdrake.com/mazecrawler
******************************************************************************/

#ifndef MAZE_CRAWLER_H_
#define MAZE_CRAWLER_H_

#include <pebble.h>

/******************************************************************************
  Enumerations
******************************************************************************/

// Achievements:
enum {
  MAX_POINTS_ACHIEVEMENT,
  UNDER_THIRTY_SECONDS_ACHIEVEMENT,
  UNDER_TEN_SECONDS_ACHIEVEMENT,
  ONE_HOUR_ACHIEVEMENT,
  FIRST_LEVEL_ACHIEVEMENT,
  LEVEL_10_ACHIEVEMENT,
  LEVEL_50_ACHIEVEMENT,
  LEVEL_100_ACHIEVEMENT,
  LEVEL_500_ACHIEVEMENT,
  LEVEL_1000_ACHIEVEMENT,
  LEVEL_5000_ACHIEVEMENT,
  MAX_LEVEL_ACHIEVEMENT,
  NUM_ACHIEVEMENTS
};

// Narration types:
enum {
  CONTROLS_NARRATION,
  GAME_INFO_NARRATION,
  INTRO_NARRATION,
  STATS_NARRATION,
  NUM_NARRATION_TYPES
};

// Cell types:
enum {
  SOLID,
  EMPTY,
  ENTRANCE,
  EXIT,
  NUM_CELL_TYPES
};

// Directions:
enum {
  NORTH,
  SOUTH,
  EAST,
  WEST,
  NUM_DIRECTIONS
};

/******************************************************************************
  Other Constants
******************************************************************************/

#define MESSAGE_STR_LEN            50
#define LEVEL_STR_LEN              7
#define TIME_STR_LEN               5
#define ACHIEVEMENT_NAME_STR_LEN   15
#define ACHIEVEMENT_DESC_STR_LEN   50
#define NARRATION_STR_LEN          110
#define SCREEN_WIDTH               144
#define SCREEN_HEIGHT              168
#define HALF_SCREEN_WIDTH          (SCREEN_WIDTH / 2)
#define STATUS_BAR_HEIGHT          16 // Applies to top and bottom status bars.
#define STATUS_BAR_PADDING         4
#define FIRST_WALL_OFFSET          STATUS_BAR_HEIGHT
#define MIN_WALL_HEIGHT            STATUS_BAR_HEIGHT
#define GRAPHICS_FRAME_HEIGHT      (SCREEN_HEIGHT - 2 * STATUS_BAR_HEIGHT)
#define GRAPHICS_FRAME_WIDTH       SCREEN_WIDTH

#ifdef PBL_COLOR
#define FULL_SCREEN_FRAME          GRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - STATUS_BAR_HEIGHT)
#define GRAPHICS_FRAME             GRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, GRAPHICS_FRAME_HEIGHT)
#define LEVEL_TEXT_LAYER_FRAME     GRect(STATUS_BAR_PADDING, STATUS_BAR_HEIGHT + GRAPHICS_FRAME_HEIGHT, HALF_SCREEN_WIDTH, STATUS_BAR_HEIGHT)
#define TIME_TEXT_LAYER_FRAME      GRect(HALF_SCREEN_WIDTH, STATUS_BAR_HEIGHT + GRAPHICS_FRAME_HEIGHT, HALF_SCREEN_WIDTH - STATUS_BAR_PADDING, STATUS_BAR_HEIGHT)
#define MESSAGE_BOX_FRAME          GRect(10, STATUS_BAR_HEIGHT + 15, SCREEN_WIDTH - 20, GRAPHICS_FRAME_HEIGHT - 30)
#define NARRATION_TEXT_LAYER_FRAME GRect(2, STATUS_BAR_HEIGHT, SCREEN_WIDTH - 4, SCREEN_HEIGHT)
#else
#define FULL_SCREEN_FRAME          GRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - STATUS_BAR_HEIGHT)
#define GRAPHICS_FRAME             GRect(0, 0, SCREEN_WIDTH, GRAPHICS_FRAME_HEIGHT)
#define LEVEL_TEXT_LAYER_FRAME     GRect(STATUS_BAR_PADDING, GRAPHICS_FRAME_HEIGHT, HALF_SCREEN_WIDTH, STATUS_BAR_HEIGHT)
#define TIME_TEXT_LAYER_FRAME      GRect(HALF_SCREEN_WIDTH, GRAPHICS_FRAME_HEIGHT, HALF_SCREEN_WIDTH - STATUS_BAR_PADDING, STATUS_BAR_HEIGHT)
#define MESSAGE_BOX_FRAME          GRect(10, 15, SCREEN_WIDTH - 20, GRAPHICS_FRAME_HEIGHT - 30)
#define NARRATION_TEXT_LAYER_FRAME GRect(2, 0, SCREEN_WIDTH - 4, SCREEN_HEIGHT)
#endif

#define STATUS_BAR_FONT            fonts_get_system_font(FONT_KEY_GOTHIC_14)
#define MESSAGE_BOX_FONT           fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD)
#define NARRATION_FONT             fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD)
#define NO_CORNER_RADIUS           0
#define COMPASS_RADIUS             5
#define MIN_MAZE_WIDTH             10 // Min. no. of cells per side.
#define MIN_MAZE_HEIGHT            MIN_MAZE_WIDTH
#define MAX_MAZE_WIDTH             20 // Max. no. of cells per side.
#define MAX_MAZE_HEIGHT            MAX_MAZE_WIDTH
#define MAX_VISIBILITY_DEPTH       6 // Helps determine no. of cells visible in a given line of sight.
#define STRAIGHT_AHEAD             (MAX_VISIBILITY_DEPTH - 1) // Index value for "g_back_wall_coords".
#define TOP_LEFT                   0 // Index value for "g_back_wall_coords".
#define BOTTOM_RIGHT               1 // Index value for "g_back_wall_coords".
#define RANDOM_POINT_NORTH         GPoint(rand() % g_maze->width, rand() % (g_maze->height / 4))
#define RANDOM_POINT_SOUTH         GPoint(rand() % g_maze->width, g_maze->height - (1 + rand() % (g_maze->height / 4)))
#define RANDOM_POINT_EAST          GPoint(g_maze->width - (1 + rand() % (g_maze->width / 4)), rand() % g_maze->height)
#define RANDOM_POINT_WEST          GPoint(rand() % (g_maze->width / 4), rand() % g_maze->height)
#define NINETY_DEGREES             (TRIG_MAX_ANGLE / 4)
#define DEFAULT_ROTATION_RATE      (TRIG_MAX_ANGLE / 30) // 12 degrees per rotation event
#define ELLIPSE_RADIUS_RATIO       0.4
#define CLICK_REPEAT_INTERVAL      300 // milliseconds
#define MULTI_CLICK_MIN            2
#define MULTI_CLICK_MAX            2 // We only care about double-clicks.
#define MULTI_CLICK_TIMEOUT        0
#define LAST_CLICK_ONLY            true
#define MAX_SMALL_INT_VALUE        9999
#define MAX_SMALL_INT_DIGITS       4
#define MAX_LARGE_INT_VALUE        999999999
#define MAX_LARGE_INT_DIGITS       9
#define MAX_LEVEL                  MAX_SMALL_INT_VALUE
#define MAX_POINTS                 MAX_LARGE_INT_VALUE
#define MAX_SECONDS                3599 // 59:59 (just under an hour)
#define MAIN_MENU_NUM_ROWS         4
#define IN_GAME_MENU_NUM_ROWS      4
#define INTRO_NARRATION_NUM_PAGES  4
#define STORAGE_KEY                8417
#define ANIMATED                   true
#define NOT_ANIMATED               false

static const GPathInfo COMPASS_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{-3, -3},
                         {3, -3},
                         {0, 6},
                         {-3, -3}}
};

static const char *const g_narration_strings[3][4] = {
  {
    "       CONTROLS\nForward: \"Up\"\nBack: \"Down\"\nLeft: \"Up\" x 2\nRight: \"Down\" x 2",
    "More information available online:\n\ndavidcdrake.com/\n           mazecrawler",
    "",
    "",
  },
  {
    "MazeCrawler was designed and programmed by David C. Drake:\n\ndavidcdrake.com",
    "Thanks for playing! And special thanks to Team Pebble for creating these wonderful, fun, and useful devices!",
    "",
    "",
  },
  {
    "You have fallen into a vast network of mazes. Each maze has an exit...",
    "...but each exit leads down to yet another, deeper level of the labyrinth.",
    "Will you ever escape, or are you doomed to roam these halls to the end of your days?",
    "You know not, yet here you are, brave explorer, and you must try!",
  },
};

static const char *const g_achievement_names[] = {
  "Addicted",
  "Speedy",
  "Super Speedy",
  "Fell Asleep",
  "Novice",
  "Apprentice",
  "Journeyman",
  "Master",
  "Dedicated",
  "Devoted",
  "Obsessed",
  "Fanatical",
};

static const char *const g_achievement_descriptions[] = {
  "Reached the max. number of points!",
  "Completed a maze in < 30 seconds!",
  "Completed a maze in < 10 seconds!",
  "In a maze for one hour!",
  "Completed your first maze!",
  "Reached level 10!",
  "Reached level 50!",
  "Reached level 100!",
  "Reached level 500!",
  "Reached level 1000!",
  "Reached level 5000!",
  "Completed level 9999!",
};

/******************************************************************************
  Structures
******************************************************************************/

typedef struct Maze
{
  int16_t cells[MAX_MAZE_WIDTH][MAX_MAZE_HEIGHT],
          width,
          height,
          seconds,
          starting_direction;
  GPoint entrance;
} __attribute__((__packed__)) maze_t;

typedef struct PlayerCharacter
{
  GPoint position;
  int16_t direction,
          level,
          best_time; // in seconds
  int32_t points;
  bool achievement_unlocked[NUM_ACHIEVEMENTS];
} __attribute__((__packed__)) player_t;

/******************************************************************************
  Global Variables
******************************************************************************/

Window *g_main_menu_window,
       *g_in_game_menu_window,
       *g_narration_window,
       *g_graphics_window,
       *g_message_box_window;
MenuLayer *g_main_menu,
          *g_in_game_menu;
ScrollLayer *g_scroll_layer;
TextLayer *g_level_text_layer,
          *g_time_text_layer,
          *g_message_box_text_layer,
          *g_narration_text_layer;
char g_message_str[MESSAGE_STR_LEN + 1],
     g_narration_str[NARRATION_STR_LEN + 1];
maze_t *g_maze;
player_t *g_player;
GPoint g_back_wall_coords[MAX_VISIBILITY_DEPTH - 1]
                         [(STRAIGHT_AHEAD * 2) + 1]
                         [2];
int16_t g_new_best_time,
        g_current_narration,
        g_narration_page_num;
bool g_game_paused,
     g_new_achievement_unlocked[NUM_ACHIEVEMENTS];
GPath *g_compass_path;
GColor g_current_background_color;

#ifdef PBL_COLOR
#define NUM_BACKGROUND_COLORS 10
GColor g_background_colors[NUM_BACKGROUND_COLORS];
static StatusBarLayer *g_main_menu_status_bar,
                      *g_in_game_menu_status_bar,
                      *g_narration_status_bar,
                      *g_graphics_status_bar;
#endif

/******************************************************************************
  Function Declarations
******************************************************************************/

void show_narration(void);
void show_window(Window *const window);
void init_player(void);
void reposition_player(void);
bool move_player(const int16_t direction);
bool shift_position(GPoint *const position, const int16_t direction);
bool check_for_maze_completion(void);
bool save_game_data(void);
bool load_game_data(void);
void init_wall_coords(void);
void init_maze(void);
int16_t set_maze_starting_direction(void);
void draw_scene(Layer *layer, GContext *ctx);
void draw_floor_and_ceiling(GContext *ctx);
bool draw_cell_contents(GContext *ctx,
                        const GPoint cell_coords,
                        const int16_t depth,
                        const int16_t position);
bool draw_wall(GContext *ctx,
               const GPoint upper_left,
               const GPoint lower_left,
               const GPoint upper_right,
               const GPoint lower_right);
bool draw_entrance(GContext *ctx, const int16_t depth, const int16_t position);
bool draw_exit(GContext *ctx, const int16_t depth, const int16_t position);
bool fill_ellipse(GContext *ctx,
                  const GPoint center,
                  const int16_t h_radius,
                  const int16_t v_radius,
                  const GColor color);
void update_status_bar(GContext *ctx);
void update_compass(void);
void show_message_box(void);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
void app_focus_handler(const bool in_focus);
static void graphics_window_appear(Window *window);
static void graphics_window_disappear(Window *window);
static void graphics_window_load(Window *window);
static void graphics_window_unload(Window *window);
void graphics_up_single_repeating_click(ClickRecognizerRef recognizer,
                                        void *context);
void graphics_up_multi_click(ClickRecognizerRef recognizer, void *context);
void graphics_down_single_repeating_click(ClickRecognizerRef recognizer,
                                          void *context);
void graphics_down_multi_click(ClickRecognizerRef recognizer, void *context);
void graphics_select_single_click(ClickRecognizerRef recognizer,
                                  void *context);
void graphics_click_config_provider(void *context);
void message_box_select_single_click(ClickRecognizerRef recognizer,
                                     void *context);
void message_box_click_config_provider(void *context);
void narration_single_click(ClickRecognizerRef recognizer, void *context);
void narration_click_config_provider(void *context);
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer,
                                           uint16_t section_index,
                                           void *data);
static void main_menu_draw_row_callback(GContext* ctx,
                                        const Layer *cell_layer,
                                        MenuIndex *cell_index,
                                        void *data);
void main_menu_select_callback(MenuLayer *menu_layer,
                               MenuIndex *cell_index,
                               void *data);
static void in_game_menu_draw_row_callback(GContext* ctx,
                                           const Layer *cell_layer,
                                           MenuIndex *cell_index,
                                           void *data);
void in_game_menu_select_callback(MenuLayer *menu_layer,
                                  MenuIndex *cell_index,
                                  void *data);
int16_t get_num_achievements_unlocked(void);
int16_t get_new_achievement_index(void);
GPoint get_floor_center_point(const int16_t depth, const int16_t position);
GPoint get_ceiling_center_point(const int16_t depth, const int16_t position);
GPoint get_cell_farther_away(const GPoint reference_point,
                             const int16_t direction,
                             const int16_t distance);
GPoint get_cell_to_the_left(const GPoint reference_point,
                            const int16_t reference_direction,
                            const int16_t distance);
GPoint get_cell_to_the_right(const GPoint reference_point,
                             const int16_t reference_direction,
                             const int16_t distance);
int16_t get_cell_type(GPoint cell_coords);
bool out_of_bounds(const GPoint cell_coords);
bool is_solid(const GPoint cell_coords);
int16_t get_opposite_direction(const int16_t direction);
void strcat_time(char *const dest_str, int16_t seconds);
void init_narration(void);
void deinit_narration(void);
void init(void);
void deinit(void);
int main(void);

#endif // MAZE_CRAWLER_H_
