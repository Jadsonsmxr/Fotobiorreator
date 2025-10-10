#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

static void event_handler_cb_home_screen_obj0(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 25, 0, e);
    }
}

void create_screen_home_screen() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.home_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 800, 82);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff004579), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff06c500), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    lv_obj_set_pos(obj, 10, -18);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_image_set_src(obj, &img_fuea);
                }
                {
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    lv_obj_set_pos(obj, 133, -14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_image_set_src(obj, &img_fapeam);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj2 = obj;
                    lv_obj_set_pos(obj, 255, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Fotobiorreator");
                }
                {
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    lv_obj_set_pos(obj, 532, -18);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_image_set_src(obj, &img_logo_cba);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj3 = obj;
            lv_obj_set_pos(obj, 30, 104);
            lv_obj_set_size(obj, 740, 353);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_opa(obj, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 14, 14);
                    lv_obj_set_size(obj, 234, 200);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj4 = obj;
                            lv_obj_set_pos(obj, 79, 87);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "32");
                        }
                        {
                            lv_obj_t *obj = lv_arc_create(parent_obj);
                            objects.obj5 = obj;
                            lv_obj_set_pos(obj, 20, 27);
                            lv_obj_set_size(obj, 150, 150);
                            lv_arc_set_value(obj, 75);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                            lv_obj_set_style_arc_rounded(obj, true, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_width(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_color(obj, lv_color_hex(0xff4b4b4b), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_width(obj, 8, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_color(obj, lv_color_hex(0xff00d82a), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj6 = obj;
                            lv_obj_set_pos(obj, 30, -13);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Temperatura");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj7 = obj;
                            lv_obj_set_pos(obj, 86, 133);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "°C");
                        }
                        {
                            lv_obj_t *obj = lv_image_create(parent_obj);
                            lv_obj_set_pos(obj, 75, 47);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_image_set_src(obj, &img_temp);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 232, 14);
                    lv_obj_set_size(obj, 234, 200);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_image_create(parent_obj);
                            lv_obj_set_pos(obj, 75, 47);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_image_set_src(obj, &img_ph1);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj8 = obj;
                            lv_obj_set_pos(obj, 76, 87);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "7,5");
                        }
                        {
                            lv_obj_t *obj = lv_arc_create(parent_obj);
                            objects.obj9 = obj;
                            lv_obj_set_pos(obj, 20, 27);
                            lv_obj_set_size(obj, 150, 150);
                            lv_arc_set_value(obj, 51);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                            lv_obj_set_style_arc_rounded(obj, true, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_width(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_color(obj, lv_color_hex(0xff4b4b4b), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_width(obj, 8, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_color(obj, lv_color_hex(0xff00d82a), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj10 = obj;
                            lv_obj_set_pos(obj, 80, -13);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "pH");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 449, 14);
                    lv_obj_set_size(obj, 234, 200);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_image_create(parent_obj);
                            lv_obj_set_pos(obj, 76, 47);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_image_set_src(obj, &img_co2);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj11 = obj;
                            lv_obj_set_pos(obj, 71, 87);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "362");
                        }
                        {
                            lv_obj_t *obj = lv_arc_create(parent_obj);
                            objects.obj12 = obj;
                            lv_obj_set_pos(obj, 20, 27);
                            lv_obj_set_size(obj, 150, 150);
                            lv_arc_set_value(obj, 85);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                            lv_obj_set_style_arc_rounded(obj, true, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_width(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_color(obj, lv_color_hex(0xff4b4b4b), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_width(obj, 8, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_color(obj, lv_color_hex(0xff00d82a), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj13 = obj;
                            lv_obj_set_pos(obj, 74, -13);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "CO");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj14 = obj;
                            lv_obj_set_pos(obj, 76, 133);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "ppm");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj15 = obj;
                            lv_obj_set_pos(obj, 106, -6);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "2");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 673, 16);
            lv_obj_set_size(obj, 80, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_home_screen_obj0, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff005b1a), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj16 = obj;
            lv_obj_set_pos(obj, 117, 411);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Manaus - AM");
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 80, 402);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_localiza__o);
        }
    }
    
    tick_screen_home_screen();
}

void delete_screen_home_screen() {
    lv_obj_delete(objects.home_screen);
    objects.home_screen = 0;
    objects.obj1 = 0;
    objects.obj2 = 0;
    objects.obj3 = 0;
    objects.obj4 = 0;
    objects.obj5 = 0;
    objects.obj6 = 0;
    objects.obj7 = 0;
    objects.obj8 = 0;
    objects.obj9 = 0;
    objects.obj10 = 0;
    objects.obj11 = 0;
    objects.obj12 = 0;
    objects.obj13 = 0;
    objects.obj14 = 0;
    objects.obj15 = 0;
    objects.obj0 = 0;
    objects.obj16 = 0;
    deletePageFlowState(0);
}

void tick_screen_home_screen() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
}

void create_screen_history_screen() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.history_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_chart_create(parent_obj);
            lv_obj_set_pos(obj, 298, 166);
            lv_obj_set_size(obj, 180, 100);
        }
    }
    
    tick_screen_history_screen();
}

void delete_screen_history_screen() {
    lv_obj_delete(objects.history_screen);
    objects.history_screen = 0;
    deletePageFlowState(1);
}

void tick_screen_history_screen() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
}

void create_screen_config_screen() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.config_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    
    tick_screen_config_screen();
}

void delete_screen_config_screen() {
    lv_obj_delete(objects.config_screen);
    objects.config_screen = 0;
    deletePageFlowState(2);
}

void tick_screen_config_screen() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
}


static const char *screen_names[] = { "Home_Screen", "History_Screen", "Config_Screen" };
static const char *object_names[] = { "home_screen", "history_screen", "config_screen", "obj0", "obj1", "obj2", "obj3", "obj4", "obj5", "obj6", "obj7", "obj8", "obj9", "obj10", "obj11", "obj12", "obj13", "obj14", "obj15", "obj16" };


typedef void (*create_screen_func_t)();
create_screen_func_t create_screen_funcs[] = {
    create_screen_home_screen,
    create_screen_history_screen,
    create_screen_config_screen,
};
void create_screen(int screen_index) {
    create_screen_funcs[screen_index]();
}
void create_screen_by_id(enum ScreensEnum screenId) {
    create_screen_funcs[screenId - 1]();
}

typedef void (*delete_screen_func_t)();
delete_screen_func_t delete_screen_funcs[] = {
    delete_screen_home_screen,
    delete_screen_history_screen,
    delete_screen_config_screen,
};
void delete_screen(int screen_index) {
    delete_screen_funcs[screen_index]();
}
void delete_screen_by_id(enum ScreensEnum screenId) {
    delete_screen_funcs[screenId - 1]();
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_home_screen,
    tick_screen_history_screen,
    tick_screen_config_screen,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    
    eez_flow_set_create_screen_func(create_screen);
    eez_flow_set_delete_screen_func(delete_screen);
    
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_home_screen();
    create_screen_history_screen();
    create_screen_config_screen();
}
