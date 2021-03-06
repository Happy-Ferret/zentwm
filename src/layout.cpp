#include "layout.h"
#include "config.h"

/*
    Layout handling
*/


layout* firstlayout = nullptr;

void add_layout(layout& layout){
    if (firstlayout == nullptr){
        firstlayout = &layout;
        firstlayout->next = firstlayout;
    }
    else {
        layout.next = firstlayout->next;
        firstlayout->next = &layout;
    }
}

layout* get_layout(){
    if (firstlayout == nullptr){
        firstlayout = &masterslavelayout;
        firstlayout->next = &masterslavelayout;
    }
    return firstlayout;
}



/*
    The actual layout functions
*/



/*

    Ëvenlayout

    This is a basic grid arrange function that tries to give each window an
    equal space.

*/

struct layout evenlayout {
    .name = "even",
    .layoutfunc = evenlayoutfunc,
    .next = nullptr,
};

void evenlayoutfunc(Workspace * workspace)
{
    // If there are no windows to arrange, stop
    if (workspace->count_tiling() == 0) return;

    // Initialize variables
    struct swc_rectangle geometry;
    struct swc_rectangle * workspace_geometry = &workspace->screen->swc->usable_geometry;

    // Number of rows and columns to use
    int num_columns = ceil(sqrt(workspace->count_tiling()));
    int num_rows = workspace->count_tiling() / num_columns + 1;

    // Get ready to count windows
    int window_index = 0;
    Window* window = workspace->get_window(0);

    // Iterate over columns
    for (int column_index = 0; column_index < num_columns; ++column_index)
    {
        // Get column geometry
        geometry.x = workspace_geometry->x + border_width + padding
            + workspace_geometry->width * column_index / num_columns;
        geometry.width = workspace_geometry->width / num_columns
            - 2 * border_width - 2*padding;

        // Even out the amount of rows if the window count doesnt have a even sqrt
        if (column_index == workspace->count_tiling() % num_columns)
            --num_rows;

        // Iterate over rows in column
        for (int row_index = 0; row_index < num_rows; ++row_index)
        {
            // Get row geometry
            geometry.y = workspace_geometry->y + border_width + padding
                + workspace_geometry->height * row_index / num_rows;
            geometry.height = workspace_geometry->height / num_rows
                - 2 * border_width - 2*padding;

            // Get current window to apply position and size to
            window = workspace->get_window(window_index);
            // Apply window position and size
            swc_window_set_geometry(window->swc, &geometry);
            window_index++;
        }
    }
}

/*

    Master-slave layout

    An layout with one window being the master has one half of the left side of the workspace,
    while the others are rowed on the right side of the workspace

*/

struct layout masterslavelayout {
    .name = "masterslave",
    .layoutfunc = masterslavelayoutfunc,
    .next = nullptr,
};

void masterslavelayoutfunc(Workspace * workspace)
{
    // If there are no windows to arrange, stop
    if (workspace->count_tiling() == 0) return;

    // Initialize variables
    struct swc_rectangle geometry;
    struct swc_rectangle * workspace_geometry = &workspace->screen->swc->usable_geometry;

    Window* window = workspace->get_window(0);

    // If there is only one window, make it reserve the full workspace
    if (workspace->count_tiling() == 1){
        geometry.x = workspace_geometry->x + border_width + padding;
        geometry.y = workspace_geometry->y + border_width + padding;
        geometry.width = workspace_geometry->width - 2*border_width - 2*padding;
        geometry.height = workspace_geometry->height - 2*border_width - 2*padding;
        swc_window_set_geometry(window->swc, &geometry);
    }
    else {
		// Master window on the left side
        geometry.x = workspace_geometry->x + border_width + padding;
        geometry.width = workspace_geometry->width/2 - 2*padding - 2*border_width;
        geometry.y = workspace_geometry->y + border_width + padding;
        geometry.height = workspace_geometry->height - 2*padding - 2*border_width;
        // Apply size to master window
        swc_window_set_geometry(window->swc, &geometry);

        // The other windows on rows on the right side
        geometry.x = workspace_geometry->x + workspace_geometry->width/2 + padding + border_width;

        // Calculate row count
        int num_rows = workspace->count_tiling() - 1;
		int row_index;
        // Loop over each window in each row
        for (int window_index = 1; window_index < workspace->count_tiling(); window_index++)
        {
            // Get window and row
			window = workspace->get_window(window_index);
			row_index = window_index - 1;
            // Set dimensions
            geometry.y = workspace_geometry->y + border_width + padding
                + workspace_geometry->height * row_index / num_rows;
            geometry.height = workspace_geometry->height / num_rows
                - 2 * border_width - 2*padding;
            // Apply size to window
            swc_window_set_geometry(window->swc, &geometry);
        }
    }
}

/*

    Full Layout

    Makes the focused window fullscreen, while the others are hidden

*/

struct layout fulllayout {
    .name = "full",
    .layoutfunc = fulllayoutfunc,
    .next = nullptr,
};

void fulllayoutfunc(Workspace* workspace)
{
    // If there are no windows to arrange, abort
    if (workspace->count_tiling() == 0) return;

    // Get available screen realestate
    struct swc_rectangle * geometry = &workspace->screen->swc->usable_geometry;
    // Get window
    Window* window = workspace->focused_window;

    if (window){
        // Set focused windows geometry to the full screen realestate
        swc_window_set_geometry(window->swc, geometry);
        // Hide all windows except for the focused one
        workspace->hide_all();
        swc_window_show(window->swc);
    }
}
