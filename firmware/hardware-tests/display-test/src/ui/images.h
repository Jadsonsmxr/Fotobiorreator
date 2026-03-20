#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_arc;
extern const lv_img_dsc_t img_arc_temperature;
extern const lv_img_dsc_t img_arc_ph;
extern const lv_img_dsc_t img_img_fuea;
extern const lv_img_dsc_t img_img_fapeam;
extern const lv_img_dsc_t img_img_cba;
extern const lv_img_dsc_t img_img_co2;
extern const lv_img_dsc_t img_img_ph1;
extern const lv_img_dsc_t img_img_temp;
extern const lv_img_dsc_t img_img_localiza__o;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[10];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/