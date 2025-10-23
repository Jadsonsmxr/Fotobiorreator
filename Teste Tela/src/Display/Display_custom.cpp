#include "ui/screens.h"   // inclui o arquivo gerado pelo EEZ Studio
#include <lvgl.h>
#include <cstdio>
#include <cmath> //para usar isnan()

// Variáveis para armazenar os últimos valores válidos
float lastTemp = 0.0;
float lastPh   = 7.0;
float lastCo2  = 0.0;

static lv_chart_series_t *serTemp = nullptr;
static lv_chart_series_t *serPh   = nullptr;
static lv_chart_series_t *serCo2  = nullptr;
//pensar em criar alguma condição para mudar entre tipos de gráficos (linha, barra, etc)
void ui_chartline_layout_update() 
{
    lv_chart_set_type(objects.chart_temp, LV_CHART_TYPE_LINE);
    lv_chart_set_type(objects.chart_ph, LV_CHART_TYPE_LINE);
    lv_chart_set_type(objects.chart_co2, LV_CHART_TYPE_LINE);

    // Adiciona as séries apenas uma vez e armazena em variáveis de arquivo
    if (!serTemp) serTemp = lv_chart_add_series(objects.chart_temp, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    if (!serPh) serPh = lv_chart_add_series(objects.chart_ph,   lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    if (!serCo2) serCo2 = lv_chart_add_series(objects.chart_co2,  lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
};

    


void ui_custom_update_values(float temp, float ph, float co2) {
    char buffer[16];
    ui_chartline_layout_update();
    // --- Temperatura ---
    if (!std::isnan(temp)) lastTemp = temp;
    sprintf(buffer, "%.0f", lastTemp);
    lv_label_set_text(objects.data_temperatura, buffer);
    lv_arc_set_value(objects.arc_temperatura, (int)(lastTemp));
    lv_chart_set_next_value(objects.chart_temp, serTemp, lastTemp);
     // Atualiza o gráfico de temperatura

    // --- pH ---
    if (!std::isnan(ph)) lastPh = ph;
    sprintf(buffer, "%.1f", lastPh);
    lv_label_set_text(objects.data_ph, buffer);
    lv_arc_set_value(objects.arc_ph, (int)(lastPh));

    // --- CO₂ ---
    if (!std::isnan(co2)) lastCo2 = co2;
    sprintf(buffer, "%.0f", lastCo2);
    lv_label_set_text(objects.data_co2, buffer);
    lv_arc_set_value(objects.arc_co2, (int)(lastCo2));

    lv_chart_refresh(objects.chart_temp);
    lv_chart_refresh(objects.chart_ph);
    lv_chart_refresh(objects.chart_co2);
}

void transitions_screens() {
    lv_obj_add_event_cb(objects.button_general, [](lv_event_t * e) {
        lv_obj_t *dest = (lv_obj_t *)lv_event_get_user_data(e);
        lv_obj_t *current = lv_scr_act(); // pega tela atual

        if(current != dest) { // só troca se for diferente
            lv_scr_load_anim(dest, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, true);
        }
    }, LV_EVENT_CLICKED, objects.general_screen);
}
