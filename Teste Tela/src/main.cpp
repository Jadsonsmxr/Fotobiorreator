#include <Arduino.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <TAMC_GT911.h>
#include <touch.h>
#include <ui/ui.h>  // Arquivos gerados pelo EEZ Studio

// ----------------------
// Configuração do display
// ----------------------
#define RST_PIN  2
#define GFX_BL   DF_GFX_BL

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    14 /* R0 */, 21 /* R1 */, 47 /* R2 */, 48 /* R3 */, 45 /* R4 */,
    9  /* G0 */, 46 /* G1 */, 3  /* G2 */, 8  /* G3 */, 16 /* G4 */, 1 /* G5 */,
    15 /* B0 */, 7  /* B1 */, 6  /* B2 */, 5  /* B3 */, 4 /* B4 */,
    0 /* hsync_polarity */, 210 /* hsync_front_porch */, 30 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
    0 /* vsync_polarity */, 22 /* vsync_front_porch */, 13 /* vsync_pulse_width */, 10 /* vsync_back_porch */,
    1 /* pclk_active_neg */, 16000000 /* prefer_speed */
);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    800 /* width */, 480 /* height */, rgbpanel
);

// ----------------------
// Buffers LVGL (em PSRAM)
// ----------------------
#define LV_BUFFER_LINES 50
#define LV_BUFFER_SIZE (800 * LV_BUFFER_LINES)

static lv_color_t *buf1 = nullptr;
static lv_color_t *buf2 = nullptr;

// ----------------------
// Função de flush (LVGL 9.3.0)
// ----------------------
static void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);

    lv_display_flush_ready(disp);
}


static uint32_t screenWidth;
static uint32_t screenHeight;

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
      Serial.print( "Data x " );
      Serial.println( data->point.x );
      Serial.print( "Data y " );
      Serial.println( data->point.y );
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
}

unsigned long lastTick = 0;

void update_UI() {
    unsigned long now = millis();
    unsigned long diff = now - lastTick;

    // Limita o incremento a no máximo 5ms por chamada
    // Evita picos grandes se o loop atrasar
    if(diff > 5) diff = 5;

    lv_tick_inc(diff);  // Atualiza LVGL com o tempo decorrido
    lastTick = now;

    lv_timer_handler(); // Atualiza a UI
}



// ----------------------
// Setup
// ----------------------
void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando...");

    // Reset físico do painel
    pinMode(RST_PIN, OUTPUT);
    digitalWrite(RST_PIN, LOW);
    delay(50);
    digitalWrite(RST_PIN, HIGH);
    delay(50);

    // Inicializa display físico
    gfx->begin();
    gfx->fillScreen(BLACK);
    Serial.println("Display iniciado!");

    // ----------------------
    // Inicializa LVGL
    // ----------------------
    lv_init();




    // Init touch device
    pinMode(TOUCH_GT911_RST, OUTPUT);
    digitalWrite(TOUCH_GT911_RST, LOW);
    delay(10);
    digitalWrite(TOUCH_GT911_RST, HIGH);
    delay(10);
    touch_init();

    // Aloca buffers na PSRAM
    buf1 = (lv_color_t *)heap_caps_malloc(LV_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    buf2 = (lv_color_t *)heap_caps_malloc(LV_BUFFER_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);

    if (!buf1 || !buf2)
    {
        Serial.println("ERRO: Falha ao alocar buffers LVGL na PSRAM!");
        while (1)
        {
            delay(1000);
        }
    }

    // Cria display LVGL
    lv_display_t *disp = lv_display_create(800, 480);

     // Registra função de flush
    lv_display_set_flush_cb(disp, my_disp_flush);

    // Define buffers e modo de renderização parcial
    lv_display_set_buffers(disp, buf1, buf2, LV_BUFFER_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);

   


    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);






    // ----------------------
    // Inicializa interface do EEZ Studio
    // ----------------------
    ui_init();
    Serial.println("LVGL e UI inicializados!");

    Serial.println("Setup concluído!");
}

// ----------------------
// Loop principal
// ----------------------
void loop()
{
 update_UI();
 delay(3);
}
