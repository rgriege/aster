#define VIOLET_IMPLEMENTATION
#include "violet/all.h"

#define W 600
#define H 600
#define MAX_ASTEROIDS 16

struct asteroid
{
	v2f pos;
	v2f vel;
	s32 r;
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

int main(int argc, char *const argv[])
{
	gui_t *gui;
	b32 quit = false;
	struct asteroid asteroids[MAX_ASTEROIDS] = {0};
	u32 asteroid_timer = 0;

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

		if (key_down(gui, KB_Q))
			quit = true;

		gui_end_frame_ex(gui, 16, 1000, 30000);
	}

	return 0;
}
