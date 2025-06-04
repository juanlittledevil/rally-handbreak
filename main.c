/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"

#define DEADZONE_MIN 1230
#define ADC_MAX 2100
#define DEBUG_PRINT false

typedef struct {
    uint16_t z;
} __attribute__((packed)) joystick_report_t;

static uint32_t blink_interval_ms = 250;

void led_blinking_task(void);
void hid_task(void);

/*------------- MAIN -------------*/
int main(void)
{
    stdio_init_all();
    board_init();
    tud_init(BOARD_TUD_RHPORT);

    adc_init();
    adc_gpio_init(26);      // A0 (GP26)
    adc_select_input(0);

    while (1)
    {
        tud_task(); // tinyusb device task
        led_blinking_task();
        hid_task();
    }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

void tud_mount_cb(void)   { blink_interval_ms = 1000; }
void tud_umount_cb(void)  { blink_interval_ms = 250; }
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    blink_interval_ms = 2500;
}
void tud_resume_cb(void)
{
    blink_interval_ms = tud_mounted() ? 1000 : 250;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

void send_joystick_report(uint16_t value)
{
    static joystick_report_t report;
    report.z = value;

    if (tud_hid_ready()) {
        tud_hid_report(0, &report, sizeof(report));
    }
}

void hid_task(void)
{
    static uint32_t start_ms = 0;
    const uint32_t interval_ms = 5;

    if (board_millis() - start_ms < interval_ms) return;
    start_ms += interval_ms;

    uint16_t raw = adc_read();
    uint16_t clamped = raw;

    // Apply dead zone and clamp
    if (clamped < DEADZONE_MIN) clamped = DEADZONE_MIN;
    if (clamped > ADC_MAX) clamped = ADC_MAX;

    // Scale to 0-32767, which is half the range for the joystick report
    uint16_t scaled = (uint32_t)(clamped - DEADZONE_MIN) * 32767 / (ADC_MAX - DEADZONE_MIN);
    if (DEBUG_PRINT) printf("ADC raw: %u, clamped: %u, scaled: %u\n", raw, clamped, scaled); // Debug print

    send_joystick_report(scaled);
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
    static uint32_t start_ms = 0;
    static bool led_state = false;

    // blink is disabled
    if (!blink_interval_ms) return;

    // Blink every interval ms
    if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = 1 - led_state; // toggle
}

//--------------------------------------------------------------------+
// TinyUSB HID callbacks (minimal for joystick)
//--------------------------------------------------------------------+

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    (void) instance; (void) report_id; (void) report_type; (void) buffer; (void) reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) instance; (void) report_id; (void) report_type; (void) buffer; (void) bufsize;
}