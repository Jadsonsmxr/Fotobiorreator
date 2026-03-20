#include "images.h"

const ext_img_desc_t images[10] = {
    { "arc", &img_arc },
    { "arc_temperature", &img_arc_temperature },
    { "arc_ph", &img_arc_ph },
    { "img_fuea", &img_img_fuea },
    { "img_fapeam", &img_img_fapeam },
    { "img_cba", &img_img_cba },
    { "img_co2", &img_img_co2 },
    { "img_ph1", &img_img_ph1 },
    { "img_temp", &img_img_temp },
    { "img_localiza__o", &img_img_localiza__o },
};