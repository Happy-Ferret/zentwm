#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <string>
#include <vector>

class Workspace;

#include "screen.h"

// Commands
void switch_workspace(void * data, uint32_t time, uint32_t value, uint32_t state);
void focus_next(void * data, uint32_t time, uint32_t value, uint32_t state);
void next_layout(void * data, uint32_t time, uint32_t value, uint32_t state);


class Workspace {
private:
	struct layout* currentlayout;
	std::vector<Window*> windows_tiling;
	std::vector<Window*> windows_floating;
public:
	std::string name;
	Screen* screen;
	Window* focused_window;

	Workspace(std::string, Screen* screen);
	// Window handling
	void add_window(Window* window);
	void remove_window(Window* window);
	void show_all();
	void hide_all();
	Window* focus_next();

	// Window access
	int get_index(Window* window);
	Window* get_window(int index);
	int count();
	int count_tiling();

	// Layout handling
	void arrange();
	void next_layout();
};

#endif
