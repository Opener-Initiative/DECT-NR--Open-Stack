#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <stddef.h>
#include <stdint.h>

//// Initialize the display and LVGL
int display_init(void);

/// @brief Update the text displayed on the screen
/// @param txt Pointer to the text string to display
void display_update_text(const char *txt);

/// @brief Update the upper text line on the display
/// @param txt Pointer to the text string to display
void display_update_set_upper_text(const char *txt);

/// @brief Update the lower text line on the display
/// @param txt Pointer to the text string to display
void display_update_set_lower_text(const char *txt);

/* Inline convenience wrappers */
static inline void display_update_upper_text(const char *txt) {
	display_update_set_upper_text(txt);
}
static inline void display_update_lower_text(const char *txt) {
	display_update_set_lower_text(txt);
}

/// @brief Process the display updates
void display_process(void);

#endif
