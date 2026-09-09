#include <Arduino.h>
#include <lvgl.h>

#include "lovyanGfxSetup.h"
#include "ui/ui.h"

#define TFT_HOR_RES SCREEN_WIDTH
#define TFT_VER_RES SCREEN_HEIGHT

#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

uint32_t draw_buf[DRAW_BUF_SIZE / 4];

LGFX tft;


// --------------------------------------------------
// LVGL Display
// --------------------------------------------------

void my_disp_flush(
    lv_display_t *disp,
    const lv_area_t *area,
    uint8_t *px_map)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    tft.startWrite();

    tft.setAddrWindow(
        area->x1,
        area->y1,
        w,
        h
    );

    tft.writePixels(
        (lgfx::rgb565_t *)px_map,
        w * h
    );

    tft.endWrite();

    lv_disp_flush_ready(disp);
}


// --------------------------------------------------
// Touch
// --------------------------------------------------

void my_touchpad_read(
    lv_indev_t *indev,
    lv_indev_data_t *data)
{
    uint16_t touchX;
    uint16_t touchY;

    bool touched = tft.getTouch(&touchX, &touchY);

    if (!touched)
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    else
    {
        data->state = LV_INDEV_STATE_PRESSED;

        data->point.x = touchX;
        data->point.y = touchY;
    }
}


// --------------------------------------------------
// SETUP
// --------------------------------------------------

void setup()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("================================");
    Serial.println("Starting Fresh LVGL Application");
    Serial.println("================================");


    // Initialize display
    tft.begin();

    tft.setRotation(0);

    tft.setBrightness(255);


    // Initialize LVGL
    lv_init();

    lv_tick_set_cb((lv_tick_get_cb_t)millis);


    // Create LVGL display
    lv_display_t *disp =
        lv_display_create(
            TFT_HOR_RES,
            TFT_VER_RES
        );


    // Display callback
    lv_display_set_flush_cb(
        disp,
        my_disp_flush
    );


    // Display buffer
    lv_display_set_buffers(
        disp,
        draw_buf,
        NULL,
        sizeof(draw_buf),
        LV_DISPLAY_RENDER_MODE_PARTIAL
    );


    // Create touch input device
    lv_indev_t *indev =
        lv_indev_create();

    lv_indev_set_type(
        indev,
        LV_INDEV_TYPE_POINTER
    );

    lv_indev_set_read_cb(
        indev,
        my_touchpad_read
    );


    // Initialize EEZ Studio UI
    ui_init();


    Serial.println("LVGL initialized successfully.");
}


// --------------------------------------------------
// LOOP
// --------------------------------------------------

void loop()
{
    lv_timer_handler();

    delay(5);
}