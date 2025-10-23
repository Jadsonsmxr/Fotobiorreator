#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *home_screen;
    lv_obj_t *general_screen;
    lv_obj_t *charts_screen;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *data_temperatura;
    lv_obj_t *arc_temperatura;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *data_ph;
    lv_obj_t *arc_ph;
    lv_obj_t *obj6;
    lv_obj_t *data_co2;
    lv_obj_t *arc_co2;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *button_general;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *button_home;
    lv_obj_t *button_charts;
    lv_obj_t *button_configs;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *tab_total;
    lv_obj_t *tab_temperatura;
    lv_obj_t *chart_temp;
    lv_obj_t *chart_ph;
    lv_obj_t *chart_co2;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_HOME_SCREEN = 1,
    SCREEN_ID_GENERAL_SCREEN = 2,
    SCREEN_ID_CHARTS_SCREEN = 3,
};

void create_screen_home_screen();
void delete_screen_home_screen();
void tick_screen_home_screen();

void create_screen_general_screen();
void delete_screen_general_screen();
void tick_screen_general_screen();

void create_screen_charts_screen();
void delete_screen_charts_screen();
void tick_screen_charts_screen();

void create_screen_by_id(enum ScreensEnum screenId);
void delete_screen_by_id(enum ScreensEnum screenId);
void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/