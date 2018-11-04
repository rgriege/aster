#define VIOLET_IMPLEMENTATION
#include "violet/all.h"

#define W 600
#define H 600
#define MAX_ASTEROIDS 16
#define PLAYER_SIZE 20.f
#define PLAYER_ROT_SPEED fPI

struct asteroid
{
	v2f pos;
	v2f vel;
	s32 r;
};

struct player
{
	v2f pos;
	v2f dir;
};

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
	struct player player = { .pos = { .x=W/2, .y=H/2 }, .dir = g_v2f_up };

	gui = gui_create(0, 0, W, H, "aster", WINDOW_CENTERED);
	if (!gui)
		return 1;

	srand(time(NULL));

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
				if (   a->pos.x + 1.f < -a->r
				    || a->pos.x - a->r - 1.f > W
				    || a->pos.y + 1.f < -a->r
				    || a->pos.y - a->r - 1.f > H)
					a->r = 0;
			}
		}

		if (key_down(gui, KB_A))
			player.dir = v2f_rot(player.dir, PLAYER_ROT_SPEED*dt);
		else if (key_down(gui, KB_D))
			player.dir = v2f_rot(player.dir, -PLAYER_ROT_SPEED*dt);
		render_player(gui, &player);

		if (key_down(gui, KB_Q))
			quit = true;

		gui_end_frame_ex(gui, 16, 1000, 30000);
	}

	return 0;
}
