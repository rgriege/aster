#define VIOLET_IMPLEMENTATION
#include "violet/all.h"

#define W 600
#define H 600

int main(int argc, char *const argv[])
{
	gui_t *gui;
	b32 quit = false;

	gui = gui_create(0, 0, W, H, "aster", WINDOW_CENTERED);
	if (!gui)
		return 1;

	while (!quit && gui_begin_frame(gui)) {

		if (key_down(gui, KB_Q))
			quit = true;

		gui_end_frame_ex(gui, 16, 1000, 30000);
	}

	return 0;
}
