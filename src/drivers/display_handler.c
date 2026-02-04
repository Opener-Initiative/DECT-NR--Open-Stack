#include "display_handler.h"
#include <zephyr/device.h>
#if DT_HAS_CHOSEN(zephyr_display)
#include <zephyr/drivers/display.h>
#endif
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include <zephyr/kernel.h>
#include <string.h>

LOG_MODULE_REGISTER(display, LOG_LEVEL_INF);

static const struct device *display_dev;
static lv_obj_t *label_upper;
static lv_obj_t *label_center;
static lv_obj_t *label_lower;

struct display_msg {
    char upper[64];
    char center[64];
    char lower[64];
    uint8_t flags;
};

#define DISP_FLAG_UPPER  (1U << 0)
#define DISP_FLAG_CENTER (1U << 1)
#define DISP_FLAG_LOWER  (1U << 2)

K_MSGQ_DEFINE(display_msgq, sizeof(struct display_msg), 8, 4);

static void display_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    struct display_msg msg;
    while (1) {
        if (k_msgq_get(&display_msgq, &msg, K_FOREVER) == 0) {
            #if DT_HAS_CHOSEN(zephyr_display)
            if ((msg.flags & DISP_FLAG_UPPER) && label_upper) {
                lv_label_set_text(label_upper, msg.upper);
            }
            if ((msg.flags & DISP_FLAG_CENTER) && label_center) {
                /* center may be empty string to clear it */
                lv_label_set_text(label_center, msg.center);
                lv_obj_align(label_center, LV_ALIGN_CENTER, 0, 0);
            }
            if ((msg.flags & DISP_FLAG_LOWER) && label_lower) {
                lv_label_set_text(label_lower, msg.lower);
            }
            lv_task_handler();
            #else
            LOG_INF("Display queued upper: %s", msg.upper);
            LOG_INF("Display queued center: %s", msg.center);
            LOG_INF("Display queued lower: %s", msg.lower);
            #endif
        }
    }
}

/* Increase display thread stack in case LVGL and display driver need more
 * temporary stack during formatting/flush operations when a real display is
 * connected. If this still crashes, try higher values or investigate
 * per-driver stack usage. */
/* Increase display thread stack to handle LVGL/driver stack usage when a
 * real SSD1306 is connected; helps rule out stack overflow during init.
 */
K_THREAD_DEFINE(display_thread_id, 2048, display_thread, NULL, NULL, NULL, 6, 0, 0);

int display_init(void)
{
    #if DT_HAS_CHOSEN(zephyr_display)
    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!display_dev) {
        LOG_WRN("No display found in Device Tree - running without display");
        return -ENODEV;
    }
    if (!device_is_ready(display_dev)) {
        LOG_WRN("Display not ready - running without display");
        return -EIO;
    }
    lv_obj_clean(lv_scr_act());

    label_upper = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label_upper, LV_LABEL_LONG_WRAP);
    lv_obj_align(label_upper, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(label_upper, "");

    label_center = lv_label_create(lv_scr_act());
    lv_label_set_text(label_center, "DECT NR+");
    lv_obj_align(label_center, LV_ALIGN_CENTER, 0, 0);

    label_lower = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label_lower, LV_LABEL_LONG_WRAP);
    lv_obj_align(label_lower, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_label_set_text(label_lower, "");

    display_blanking_off(display_dev);
    lv_task_handler(); // Process and render the content
    LOG_INF("Display initialized successfully");
    #else
    LOG_INF("System configured without display");
    #endif
    return 0;
}

void display_update_set_upper_text(const char *txt)
{
    struct display_msg msg = {0};
    if (!txt) txt = "";
    strncpy(msg.upper, txt, sizeof(msg.upper)-1);
    msg.upper[sizeof(msg.upper)-1] = '\0';
    /* indicate we want to update upper; also explicitly clear center */
    msg.flags = DISP_FLAG_UPPER | DISP_FLAG_CENTER;
    msg.center[0] = '\0';
    int ret = k_msgq_put(&display_msgq, &msg, K_NO_WAIT);
    if (ret != 0) {
        LOG_WRN("Display queue full (upper), dropping");
    }
}

void display_update_set_lower_text(const char *txt)
{
    struct display_msg msg = {0};
    if (!txt) txt = "";
    strncpy(msg.lower, txt, sizeof(msg.lower)-1);
    msg.lower[sizeof(msg.lower)-1] = '\0';
    /* indicate we want to update lower; also explicitly clear center */
    msg.flags = DISP_FLAG_LOWER | DISP_FLAG_CENTER;
    msg.center[0] = '\0';
    int ret = k_msgq_put(&display_msgq, &msg, K_NO_WAIT);
    if (ret != 0) {
        LOG_WRN("Display queue full (lower), dropping");
    }
}

void display_update_text(const char *txt)
{
    struct display_msg msg = {0};
    if (!txt) txt = "";
    /* center update: set center and explicitly clear upper/lower */
    strncpy(msg.center, txt, sizeof(msg.center)-1);
    msg.center[sizeof(msg.center)-1] = '\0';
    msg.upper[0] = '\0';
    msg.lower[0] = '\0';
    msg.flags = DISP_FLAG_CENTER | DISP_FLAG_UPPER | DISP_FLAG_LOWER;
    int ret = k_msgq_put(&display_msgq, &msg, K_NO_WAIT);
    if (ret != 0) {
        LOG_WRN("Display queue full (center), dropping");
    }
    #if !DT_HAS_CHOSEN(zephyr_display)
    LOG_INF("Display text: %s", txt);
    #endif
}

void display_process(void)
{
    #if DT_HAS_CHOSEN(zephyr_display)
    lv_task_handler();
    #endif
}
