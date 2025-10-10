#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *home_screen;
    lv_obj_t *history_screen;
    lv_obj_t *config_screen;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_HOME_SCREEN = 1,
    SCREEN_ID_HISTORY_SCREEN = 2,
    SCREEN_ID_CONFIG_SCREEN = 3,
};

void create_screen_home_screen();
void delete_screen_home_screen();
void tick_screen_home_screen();

void create_screen_history_screen();
void delete_screen_history_screen();
void tick_screen_history_screen();

void create_screen_config_screen();
void delete_screen_config_screen();
void tick_screen_config_screen();

void create_screen_by_id(enum ScreensEnum screenId);
void delete_screen_by_id(enum ScreensEnum screenId);
void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/