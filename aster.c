#define VIOLET_IMPLEMENTATION
#include "violet/all.h"

#define W 600
#define H 600
#define MAX_ASTEROIDS 16
#define PLAYER_SIZE 20.f
#define PLAYER_SPEED 80.f
#define PLAYER_ROT_SPEED fPI

struct asteroid
{
	v2f pos;
	v2f vel;
	s32 r;
};

struct player
{
	enum
	{
		PLAYER_FREE,
		PLAYER_FLYING,
		PLAYER_LANDED,
	} state;
	v2f pos;
	v2f dir;
	v2f vel;
	const struct asteroid *base;
};

static
void reset_player(struct player *player)
{
	player->state = PLAYER_FREE;
	player->pos.x = W/2;
	player->pos.y = H/2;
	player->dir   = g_v2f_up;
	player->vel   = g_v2f_zero;
	player->base  = NULL;
}
static
void generate_asteroid(struct asteroid *a)
{
	const r32 speed = 20.f + rand() % 10;
	a->r = 30 + rand() % 80;
	switch (rand() % 4) {
	default:
	case 0:
		a->pos.x = -a->r;
		a->pos.y = rand() % H;
		a->vel = v2f_scale(g_v2f_right, speed);
	break;
	case 1:
		a->pos.x = W+a->r;
		a->pos.y = rand() % H;
		a->vel = v2f_scale(g_v2f_left, speed);
	break;
	case 2:
		a->pos.x = rand() % W;
		a->pos.y = -a->r;
		a->vel = v2f_scale(g_v2f_up, speed);
	break;
	case 3:
		a->pos.x = rand() % W;
		a->pos.y = H+a->r;
		a->vel = v2f_scale(g_v2f_down, speed);
	break;
	}
}

static
b32 entity_off_screen(v2f pos, r32 r)
{
	return pos.x + 1.f < -r
	    || pos.x - r - 1.f > W
	    || pos.y + 1.f < -r
	    || pos.y - r - 1.f > H;
}

static
void update_player_rotation(gui_t *gui, struct player *player, r32 dt)
{
	if (key_down(gui, KB_A))
		player->dir = v2f_rot(player->dir, PLAYER_ROT_SPEED*dt);
	else if (key_down(gui, KB_D))
		player->dir = v2f_rot(player->dir, -PLAYER_ROT_SPEED*dt);
}

static
void update_player_position_on_asteroid(gui_t *gui, struct player *player)
{
	const struct asteroid *a = player->base;
	player->pos = v2f_fmadd(a->pos, player->dir, a->r);
}

static
void render_player(gui_t *gui, const struct player *player)
{
	const v2f lperp = v2f_lperp(player->dir);
	const v2f tri[3] = {
		v2f_fmadd(player->pos, lperp, PLAYER_SIZE/2),
		v2f_fmadd(player->pos, lperp, -PLAYER_SIZE/2),
		v2f_fmadd(player->pos, player->dir, PLAYER_SIZE/2),
	};
	gui_polyf(gui, B2PC(tri), g_red, g_white);
}

int main(int argc, char *const argv[])
{
	gui_t *gui;
	b32 quit = false;
	struct asteroid asteroids[MAX_ASTEROIDS] = {0};
	u32 asteroid_timer = 0;
	struct player player = {0};

	gui = gui_create(0, 0, W, H, "aster", WINDOW_CENTERED);
	if (!gui)
		return 1;

	srand(time(NULL));
	reset_player(&player);

	while (!quit && gui_begin_frame(gui)) {
		const u32 milli = gui_frame_time_milli(gui);
		const r32 dt = (r32)milli / 1000.f;

		if (asteroid_timer == 0) {
			for (u32 i = 0; i < MAX_ASTEROIDS; ++i) {
				struct asteroid *a = &asteroids[i];
				if (a->r == 0) {
					generate_asteroid(a);
					break;
				}
			}
			asteroid_timer = 4000 + rand() % 2000;
		} else if (asteroid_timer < milli) {
			asteroid_timer = 0;
		} else {
			asteroid_timer -= milli;
		}

		for (u32 i = 0; i < MAX_ASTEROIDS; ++i) {
			struct asteroid *a = &asteroids[i];
			if (a->r > 0) {
				a->pos = v2f_fmadd(a->pos, a->vel, dt);
				gui_circ(gui, a->pos.x, a->pos.y, a->r, g_nocolor, g_white);
				if (entity_off_screen(a->pos, a->r))
					a->r = 0;
			}
		}

		switch (player.state) {
		case PLAYER_FREE:
			update_player_rotation(gui, &player, dt);
			if (key_down(gui, KB_SPACE)) {
				player.vel = v2f_scale(player.dir, PLAYER_SPEED);
				player.state = PLAYER_FLYING;
			}
		break;
		case PLAYER_FLYING:
			update_player_rotation(gui, &player, dt);
			player.pos = v2f_fmadd(player.pos, player.vel, dt);
			for (u32 i = 0; i < MAX_ASTEROIDS; ++i) {
				struct asteroid *a = &asteroids[i];
				if (a->r > 0 && v2f_dist(player.pos, a->pos) < a->r) {
					player.state = PLAYER_LANDED;
					player.vel = g_v2f_zero;
					player.base = a;
					player.dir = v2f_dir(a->pos, player.pos);
					update_player_position_on_asteroid(gui, &player);
				}
			}
		break;
		case PLAYER_LANDED:
			update_player_rotation(gui, &player, dt);
			update_player_position_on_asteroid(gui, &player);
			if (key_down(gui, KB_SPACE)) {
				player.vel = v2f_scale(player.dir, PLAYER_SPEED);
				player.state = PLAYER_FLYING;
				player.base = NULL;
			}
		break;
		}
		if (entity_off_screen(player.pos, 0)) {
			for (u32 i = 0; i < MAX_ASTEROIDS; ++i)
				asteroids[i].r = 0;
			asteroid_timer = 0;
			reset_player(&player);
		}

		render_player(gui, &player);

		if (key_down(gui, KB_Q))
			quit = true;

		gui_end_frame_ex(gui, 16, 1000, 30000);
	}

	return 0;
}
