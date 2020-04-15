// Little 'bomberman' clone
// player vs computer
// arrow keys to move the player
// space key to place a bomb or restart the game when the game is over
// CToy is not a game framework,
// this game is rendered in software (pixel by pixel)
// and all the game data is included in the code itself,
// see this as an exercise in C, not as how to make a game ;)

#include <ctoy.h>
#include <limits.h>
#include <m_path_finding.h>

// map types
#define MAP_GROUND 0
#define MAP_ROCK 1
#define MAP_BRICK 2
#define MAP_BONUS 3

#define SPR_W 5 // sprite width
#define X 1 // to help visuale sprite arrays:

char sprite_rock[SPR_W*SPR_W] = {
   X, X, X, X, 0,
   X, 0, 0, X, 0,
   X, 0, X, X, 0,
   X, X, X, X, 0,
   0, 0, 0, 0, 0
};

char sprite_brick[SPR_W*SPR_W] = {
   X, X, X, X, X,
   0, X, 0, X, 0,
   X, X, X, X, X,
   0, 0, X, 0, 0,
   X, X, X, X, X
};

char sprite_bonus[SPR_W*SPR_W] = {
   X, X, X, X, X,
   X, 0, 0, 0, X,
   X, 0, X, 0, X,
   X, 0, 0, 0, X,
   X, X, X, X, X
};

char sprite_player[SPR_W*SPR_W] = {
   0, 0, X, 0, 0,
   0, X, X, X, X,
   X, 0, X, 0, 0,
   0, 0, X, X, 0,
   0, X, 0, 0, X
};

char sprite_robot[SPR_W*SPR_W] = {
   X, 0, 0, 0, X,
   0, X, X, X, 0,
   0, X, 0, X, 0,
   X, X, X, X, X,
   X, 0, 0, 0, X
};

char sprite_bomb[SPR_W*SPR_W] = {
   0, X, X, X, 0,
   X, 0, X, X, X,
   X, X, X, X, X,
   X, X, X, X, X,
   0, X, X, X, 0
};

char sprite_flame[SPR_W*SPR_W] = {
   0, X, 0, X, 0,
   X, X, X, X, X,
   0, X, X, X, 0,
   X, X, X, X, X,
   0, X, 0, X, 0
};

// color 'palette' (sRGB color space)
char ground_color[3] = {70, 120, 90};
char rock_color[3] = {60, 60, 70};
char rock_back_color[3] = {30, 30, 50};
char brick_color[3] = {90, 70, 60};
char brick_back_color[3] = {130, 120, 100};
char bonus_color[3] = {200, 100, 50};
char player_color[3] = {0, 255, 255};
char robot_color[3] = {255, 255, 0};
char bomb_color[3] = {0, 0, 0};
char flame_color[3] = {255, 128, 0};

// game data
float forget_pr = 0.1; // reduce game difficulty (0 is very hard)
float bonus_pr = 0.03; // bonus probability
int bomb_delay = 120; // about 2 seconds (assuming 60 fps)
int player_delay = 6; // limit the player's speed (key repeat delay)
int robot_delay = 9; // a bit slower than the player

struct flame
{
   int x, y; // map coordinates
   int left, right; // flame row
   int top, bottom; // flame column
   int life;
};

struct bomb
{
   int x, y;
   int radius;
   int tick;
   struct flame flame;
};

struct character
{
   int x, y;
   struct bomb bomb;
};

struct scene
{
   struct m_image map;
   struct m_image background;
   struct m_image frame_buffer;
   struct character player;
   struct character robot;
   unsigned int *robot_motion_map;
};

struct scene game = {0};

// function to draw a 2 colors sprite on top of an image 
void draw_sprite(struct m_image *dest, char *sprite, int px, int py, char *back_color, char *front_color)
{
   int y, x;

   // we parse all rows of the sprite (top to bottom)
   for (y = 0; y < SPR_W; y++) {

      // find destination pixel according to the sprite position px, py
      // we assume the destination image is composed of 3 component bytes
      char *dest_pixel = ((char *)dest->data) + ((py + y) * dest->width + px) * 3;

      // we parse all columns of the sprite (left to right)
      for (x = 0; x < SPR_W; x++) {
         
         if ((*sprite) == 0) { // sprite marked as zero
            if (back_color) {
               dest_pixel[0] = back_color[0];
               dest_pixel[1] = back_color[1];
               dest_pixel[2] = back_color[2];
            }
         }
         else { // sprite not marked as zero (X)
            if (front_color) {
               dest_pixel[0] = front_color[0];
               dest_pixel[1] = front_color[1];
               dest_pixel[2] = front_color[2];
            }
         }

         sprite++;
         dest_pixel += 3;
      }
   }
}

void generate_map(struct m_image *map)
{
   int w = 19; // map width
   int h = 19; // map height
   char *data;
   int i, x, y;

   // create the map as a 1 byte component image
   m_image_create(map, M_UBYTE, w, h, 1);
   data = (char *)map->data;

   // clear the map
   memset(data, MAP_GROUND, map->size);

   // mark destructible blocks
   // we generate random coordinates and mark as BRICK
   for (i = 0; i < 300; i++) {

      // x, y random coordinates
      x = m_rand() % map->width;
      y = m_rand() % map->height;

      // we preserve the top left and bottom right corners (for the player and bot)
      if ((x >= 3 || y >= 3) && ((x + 3) < w || (y + 3) < h))
         data[y * w + x] = (m_randf() < bonus_pr) ? MAP_BONUS : MAP_BRICK;
   }

   // mark permanent blocks:
   // we parse every odd rows and columns and mark as ROCK
   for (y = 2; y < h-2; y+=2)
   for (x = 2; x < w-2; x+=2)
      data[y * w + x] = MAP_ROCK;

   // mark top border
   for (x = 0; x < w; x++)
      data[x] = MAP_ROCK;

   // mark left border
   for (y = 0; y < h; y++)
      data[y * w] = MAP_ROCK;

   // mark right border
   for (y = 0; y < h; y++)
      data[y * w + (w-1)] = MAP_ROCK;

   // mark bottom border
   for (x = 0; x < w; x++)
      data[(h-1) * w + x] = MAP_ROCK;
}

void update_robot_motion_map(struct scene *scn)
{
   struct m_pf_point *stack = malloc(scn->map.size * sizeof(struct m_pf_point));
   int i;

   // init from the map (GROUND = 0, WALL = UINT_MAX)
   for (i = 0; i < scn->map.size; i++)
      scn->robot_motion_map[i] = (((char *)scn->map.data)[i] == MAP_GROUND) ? 0 : UINT_MAX;

   // player
   scn->robot_motion_map[scn->player.y * scn->map.width + scn->player.x] = UINT_MAX;

   // player bomb
   scn->robot_motion_map[scn->player.bomb.y * scn->map.width + scn->player.bomb.x] = UINT_MAX;
   
   // robot bomb
   if (scn->robot.bomb.tick && (scn->robot.bomb.x != scn->robot.x || scn->robot.bomb.y != scn->robot.y))
      scn->robot_motion_map[scn->robot.bomb.y * scn->map.width + scn->robot.bomb.x] = UINT_MAX;

   // flood fill from the robot position
   m_pf_floodfill(
      scn->robot_motion_map,
      scn->map.width, scn->map.height,
      scn->robot.x, scn->robot.y,
      stack
   );

   free(stack);
}

void update_background(struct scene *scn, int x, int y)
{
   char *map_pixel = (char *)scn->map.data;

   // we draw the sprite according to the map pixel value
   switch (map_pixel[y * scn->map.width + x]) {
      case MAP_GROUND:
         draw_sprite(&scn->background, sprite_rock, x*SPR_W, y*SPR_W, ground_color, ground_color);
         break;
      case MAP_ROCK:
         draw_sprite(&scn->background, sprite_rock, x*SPR_W, y*SPR_W, rock_back_color, rock_color);
         break;
      case MAP_BRICK:
         draw_sprite(&scn->background, sprite_brick, x*SPR_W, y*SPR_W, brick_back_color, brick_color);
         break;
      case MAP_BONUS:
         draw_sprite(&scn->background, sprite_bonus, x*SPR_W, y*SPR_W, brick_color, bonus_color);
         break;
   }  
}

void generate_background(struct scene *scn)
{
   char *map_pixel = (char *)scn->map.data;
   int w = scn->map.width;
   int h = scn->map.height;
   int x, y;

   // create the background as a 3 byte component image (sRGB)
   // we multiply the map size by the sprite width to find the background size
   m_image_create(&scn->background, M_UBYTE, w*SPR_W, h*SPR_W, 3);

   // we parse all pixels of the map
   for (y = 0; y < h; y++)
   for (x = 0; x < w; x++) {
      update_background(scn, x, y);
   }

   // robot motion map
   if (scn->robot_motion_map) free(scn->robot_motion_map);
   scn->robot_motion_map = malloc(scn->map.size * sizeof(unsigned int));
}

void start_game(struct scene *scn)
{
   generate_map(&scn->map);
   generate_background(scn);
   scn->player.x = 1;
   scn->player.y = 1;
   scn->robot.x = scn->map.width-2;
   scn->robot.y = scn->map.height-2;
   scn->player.bomb.tick = 0;
   scn->player.bomb.flame.life = 0;
   scn->player.bomb.radius = 1;
   scn->robot.bomb.radius = 1;
   scn->robot.bomb.tick = 0;
   scn->robot.bomb.flame.life = 0;
}

void draw_bomb(struct scene *scn, struct bomb *bomb)
{
   if (bomb->tick > 0) {

      // we use an offset to animate the bomb
      // we divide tick by ten to control the speed of the movement
      // modulo 2 to limit the movement to 1 pixel up and down
      int anim_offset = (bomb->tick / 10) % 2;

      draw_sprite(
         &scn->frame_buffer, sprite_bomb,
         (bomb->x * SPR_W),
         (bomb->y * SPR_W) - anim_offset,
         NULL,
         bomb_color
      );
   }

   if (bomb->flame.life > 0) {

      // similarily to the bomb we use an offset to animate the flame
      // modulo 3 minus 1 gives us a tree pixel wide movement
      int anim_offset_x = ((bomb->flame.life + 0) / 2) % 3 - 1;
      int anim_offset_y = ((bomb->flame.life + 1) / 2) % 3 - 1;

      int left   = bomb->flame.x - bomb->flame.left;
      int right  = bomb->flame.x + bomb->flame.right;
      int top    = bomb->flame.y - bomb->flame.top;
      int bottom = bomb->flame.y + bomb->flame.bottom;
      int x, y;

      // flame row
      for (x = left; x <= right; x++)
         draw_sprite(
            &scn->frame_buffer, sprite_flame,
            (x * SPR_W) + anim_offset_x,
            (bomb->flame.y * SPR_W),
            NULL,
            flame_color
         );

      // flame column
      for (y = top; y <= bottom; y++)
         draw_sprite(
            &scn->frame_buffer, sprite_flame,
            (bomb->flame.x * SPR_W),
            (y * SPR_W) + anim_offset_y,
            NULL,
            flame_color
         );
   }
}

void draw_game(struct scene *scn)
{
   m_image_copy(&scn->frame_buffer, &scn->background);

   draw_bomb(scn, &game.player.bomb);
   draw_bomb(scn, &game.robot.bomb);

   draw_sprite(
      &scn->frame_buffer, sprite_player,
      scn->player.x*SPR_W,
      scn->player.y*SPR_W,
      NULL,
      player_color
   );

   draw_sprite(
      &scn->frame_buffer, sprite_robot,
      scn->robot.x*SPR_W,
      scn->robot.y*SPR_W,
      NULL,
      robot_color
   );
}

char scan_flame(struct scene *scn, int x, int y)
{
   char *map_pixel = ((char *)scn->map.data) + (y * scn->map.width + x);

   // early exit if we hit a rock
   if ((*map_pixel) == MAP_ROCK)
      return (*map_pixel);

   // if we hit a destructible, we remove it and update the background
   if ((*map_pixel) != MAP_GROUND) {
      char tmp = (*map_pixel);
      (*map_pixel) = MAP_GROUND;
      update_background(scn, x, y);
      return tmp;
   }

   return (*map_pixel);
}

void explode_bomb(struct scene *scn, struct bomb *bomb)
{
   struct flame *flame = &bomb->flame;
   int px = bomb->x;
   int py = bomb->y;
   int radius = bomb->radius;
   int x, y;

   // we are going to scan the map on four directions
   // to detect collisions, destructibles etc
   // in the radius capacity of the bomb
   // hitting a bonus will increase the bomb radius

   // left
   for (x = px; x >= (px - radius); x--) {
      char map = scan_flame(scn, x, py);
      if (map != MAP_ROCK) flame->left = (px - x);
      if (map == MAP_BONUS) bomb->radius++;
      if (map != MAP_GROUND) break;
   }

   // right
   for (x = px; x <= (px + radius); x++) {
      char map = scan_flame(scn, x, py);
      if (map != MAP_ROCK) flame->right = (x - px);
      if (map == MAP_BONUS) bomb->radius++;
      if (map != MAP_GROUND) break;
   }

   // top
   for (y = py; y >= (py - radius); y--) {
      char map = scan_flame(scn, px, y);
      if (map != MAP_ROCK) flame->top = (py - y);
      if (map == MAP_BONUS) bomb->radius++;
      if (map != MAP_GROUND) break;
   }

   // bottom
   for (y = py; y <= (py + radius); y++) {
      char map = scan_flame(scn, px, y);
      if (map != MAP_ROCK) flame->bottom = (y - py);
      if (map == MAP_BONUS) bomb->radius++;
      if (map != MAP_GROUND) break;
   }

   flame->x = px;
   flame->y = py;
   flame->life = 30;
}

void update_bomb(struct scene *scn, struct bomb *bomb)
{
   if (bomb->tick > 0) {
      if (bomb->tick == 1) explode_bomb(scn, bomb);
      bomb->tick--;
   }

   if (bomb->flame.life > 0)
      bomb->flame.life--;
}

void put_bomb(struct character *character)
{
   character->bomb.tick = bomb_delay;
   character->bomb.x = character->x;
   character->bomb.y = character->y;
}

int in_flame_range(struct bomb *bomb, int x, int y)
{
   if (bomb->flame.life > 0) {

      int left   = bomb->flame.x - bomb->flame.left;
      int right  = bomb->flame.x + bomb->flame.right;
      int top    = bomb->flame.y - bomb->flame.top;
      int bottom = bomb->flame.y + bomb->flame.bottom;

      return (x == bomb->flame.x && y >= top  && y <= bottom) ||
             (y == bomb->flame.y && x >= left && x <= right);
   }

   return 0;
}

int in_bomb_range(struct bomb *bomb, int x, int y)
{
   if (bomb->tick > 0) {
      // we assume the bomb radius is infinite (unknown yet)
      return (x == bomb->x || y == bomb->y);
   }
   return in_flame_range(bomb, x, y);
}

void move_robot_toward_target(struct scene *scn, int x, int y)
{
   int i;

   if (scn->robot.x == x && scn->robot.y == y) // already there
      return;
  
   // check if the robot is already one step away from the target
   if (scn->robot_motion_map[y * scn->map.width + x] == 1) {
      scn->robot.x = x;
      scn->robot.y = y;
      return;
   }

   // otherwise trace the path back from the target to the robot
   for (i = 0; i < scn->map.size; i++) {

      unsigned int smin = UINT_MAX;
      int j, xmin, ymin;

      // check all 4 directions to find the smallest score
      // and so the next path coordinates
      for (j = 0; j < 4; j++) {
         static int dir[4][2] = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};
         int xj = x + dir[j][0];
         int yj = y + dir[j][1];
         unsigned int sj = scn->robot_motion_map[yj * scn->map.width + xj];

         if (sj < smin) {
            smin = sj;
            xmin = xj;
            ymin = yj;
         }
      }

      x = xmin;
      y = ymin;

      // when we reach the score 1, we are one step away from the robot
      // that's where we want the robot to go next (if there are no flames)
      if (smin == 1) {
         if (!in_flame_range(&scn->robot.bomb,  x, y) &&
            (!in_flame_range(&scn->player.bomb, x, y) || m_randf() < forget_pr)) {
            scn->robot.x = x;
            scn->robot.y = y;
         }
         break;
      }
   }  
}

void update_robot(struct scene *scn)
{
   int target_x, target_y, target_d = INT_MAX;  
   int w = scn->map.width;
   int h = scn->map.height;
   int x, y;

   update_robot_motion_map(scn);

   // let's find the closest position to the player the robot can go
   for (y = 0; y < h; y++)
   for (x = 0; x < w; x++) {

      // we look at all possible pixels of the motion map
      unsigned int score = scn->robot_motion_map[y * w + x];
      if (score > 0 && score < UINT_MAX) { // was visited by flood-fill

         // if we are not in range of the bombs
         if ((!in_bomb_range(&scn->robot.bomb,  x, y)) &&
             (!in_bomb_range(&scn->player.bomb, x, y) || m_randf() < forget_pr)) {

            // find the manhattan distance to the player
            int d = M_ABS(x - scn->player.x) + M_ABS(y - scn->player.y);
            if (d < target_d) { // only keep the shortest distance
               target_d = d;
               target_x = x;
               target_y = y;
            }
         }
      }
   }

   if (target_d < INT_MAX) {

      // blocked: try to put a bomb
      if (target_x == scn->robot.x && target_y == scn->robot.y) {

         // little disavantage for the robot,
         // it has to wait for the flame to be over
         if (scn->robot.bomb.tick == 0 && scn->robot.bomb.flame.life == 0)
            put_bomb(&scn->robot);
      }
      // move towards the target
      else {
         move_robot_toward_target(scn, target_x, target_y);
      }  
   }
}

int try_move(int key, int dx, int dy, int key_t)
{
   if (ctoy_key_pressed(key) && key_t > player_delay) {

      int x = (game.player.x + dx);
      int y = (game.player.y + dy);
      char map_pixel = ((char *)game.map.data)[y * game.map.width + x];

      // robot
      if (x == game.robot.x && y == game.robot.y)
         return 0;

      // player bomb
      if (game.player.bomb.tick > 0) {
         if (x == game.player.bomb.x && y == game.player.bomb.y)
            return 0;
      }

      // robot bomb
      if (game.robot.bomb.tick > 0) {
         if (x == game.robot.bomb.x && y == game.robot.bomb.y)
            return 0;
      }

      if (map_pixel == MAP_GROUND)
         return 1;
   }

   return 0;
}

void ctoy_begin()
{
   start_game(&game);
}

void ctoy_end()
{
   m_image_destroy(&game.map);
   m_image_destroy(&game.background);
   m_image_destroy(&game.frame_buffer);
   if (game.robot_motion_map) free(game.robot_motion_map);
}

void ctoy_main_loop()
{
   // timer used to control key repeat
   static int key_t = 0;
   
   // player game over
   if (in_flame_range(&game.player.bomb, game.player.x, game.player.y) ||
      in_flame_range(&game.robot.bomb,  game.player.x, game.player.y)) {

      // we freeze the game until space is pressed
      if (ctoy_key_press(CTOY_KEY_SPACE)) start_game(&game);
      return;
   }

   // robot game over
   if (in_flame_range(&game.player.bomb, game.robot.x, game.robot.y) ||
      in_flame_range(&game.robot.bomb,  game.robot.x, game.robot.y)) {

      // we freeze the game until space is pressed
      if (ctoy_key_press(CTOY_KEY_SPACE)) start_game(&game);
      return;
   }

   // update bombs
   update_bomb(&game, &game.player.bomb);
   update_bomb(&game, &game.robot.bomb);

   // update robot
   if ((ctoy_t() % robot_delay) == 0)
      update_robot(&game);

   // update player
   if (try_move(CTOY_KEY_RIGHT, 1, 0, key_t)) {
      game.player.x++;
      key_t = 0;
   }
   else if (try_move(CTOY_KEY_LEFT, -1, 0, key_t)) {
      game.player.x--;
      key_t = 0;
   }
   else if (try_move(CTOY_KEY_DOWN, 0, 1, key_t)) {
      game.player.y++;
      key_t = 0;
   }
   else if (try_move(CTOY_KEY_UP, 0, -1, key_t)) {
      game.player.y--;
      key_t = 0;
   }

   if (game.player.bomb.tick == 0) {
      if (ctoy_key_press(CTOY_KEY_SPACE))
         put_bomb(&game.player);
   }

   // draw
   draw_game(&game);
   ctoy_swap_buffer(&game.frame_buffer);

   // update key timer
   key_t++;
}