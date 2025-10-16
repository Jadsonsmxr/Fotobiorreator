#include "ui/screens.h"   // inclui o arquivo gerado pelo EEZ Studio
#include <lvgl.h>
#include <cstdio>

void ui_custom_update_values(float temp, float ph, float co2) {
    char buffer[16];

    // Atualiza temperatura
    sprintf(buffer, "%.0f", temp);
    lv_label_set_text(objects.obj4, buffer);
    lv_arc_set_value(objects.obj5, (int)(temp));  // opcional: controla arco

    // Atualiza pH
    sprintf(buffer, "%.1f", ph);
    lv_label_set_text(objects.obj8, buffer);
    lv_arc_set_value(objects.obj9, (int)(ph * 10));  // exemplo simples

    // Atualiza CO₂
    sprintf(buffer, "%.0f", co2);
    lv_label_set_text(objects.obj11, buffer);
    lv_arc_set_value(objects.obj12, (int)(co2 / 10));  // ajusta conforme escala
}