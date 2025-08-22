#pragma once
#include <M5Stack.h>
#include <Wire.h>
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"

/* --- MLX90640 Thermal array 32x24 ---
 * Prefix: MLX90640
 * >MLX90640.Get_Image()
 */

#define MLX90640_ADDRESS 0x33
#define TA_SHIFT 8

// PaHUB port used for this device
extern int MLX90640_port;

// Temperature range used for color mapping
#define MINTEMP 24
#define MAXTEMP 35

static paramsMLX90640 mlx90640;
static uint8_t MLX90640_image[768];
static float MLX90640_pixels[768];
static float MLX90640_pixels_flipped[768];
// Frame counter to append to IMG header
static uint32_t frameCount = 0;

// Color palette copied from MIT licensed example file (C) 2025 M5Stack Technology CO LTD.
// For details and license see:
// https://github.com/m5stack/M5Stack/blob/master/examples/Unit/THERMAL_MLX90640/THERMAL_MLX90640.ino

static const uint16_t camColors[] = {
    0x480F,0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,0x3810,0x3010,0x3010,
    0x3010,0x2810,0x2810,0x2810,0x2810,0x2010,0x2010,0x2010,0x1810,0x1810,0x1811,
    0x1811,0x1011,0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,0x0011,0x0011,
    0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,0x00B2,0x00B2,0x00D2,0x00F2,
    0x00F2,0x0112,0x0132,0x0152,0x0152,0x0172,0x0192,0x0192,0x01B2,0x01D2,0x01F3,
    0x01F3,0x0213,0x0233,0x0253,0x0253,0x0273,0x0293,0x02B3,0x02D3,0x02D3,0x02F3,
    0x0313,0x0333,0x0333,0x0353,0x0373,0x0394,0x03B4,0x03D4,0x03D4,0x03F4,0x0414,
    0x0434,0x0454,0x0474,0x0474,0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,
    0x0554,0x0554,0x0574,0x0574,0x0573,0x0573,0x0573,0x0572,0x0572,0x0572,0x0571,
    0x0591,0x0591,0x0590,0x0590,0x058F,0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,
    0x05AD,0x05AD,0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,0x05C9,
    0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,0x05E6,0x05E6,0x05E5,0x05E5,
    0x0604,0x0604,0x0604,0x0603,0x0603,0x0602,0x0602,0x0601,0x0621,0x0621,0x0620,
    0x0620,0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,0x1E40,0x1E40,0x2640,
    0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,0x3E60,0x3E60,0x4660,0x4660,0x4E60,
    0x4E80,0x5680,0x5680,0x5E80,0x5E80,0x6680,0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,
    0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,0x8EC0,0x96C0,0x96C0,0x9EC0,0x9EC0,0xA6C0,
    0xAEC0,0xAEC0,0xB6E0,0xB6E0,0xBEE0,0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,
    0xD700,0xDF00,0xDEE0,0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,0xE600,
    0xE5E0,0xE5C0,0xE5A0,0xE580,0xE560,0xE540,0xE520,0xE500,0xE4E0,0xE4C0,0xE4A0,
    0xE480,0xE460,0xEC40,0xEC20,0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,
    0xEB20,0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,0xF200,0xF1E0,
    0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,0xF0E0,0xF0C0,0xF0A0,0xF080,0xF060,
    0xF040,0xF020,0xF800
};

bool MLX90640_init()
{
   
    Wire.begin();
    Wire.setClock(450000);
    uint16_t eeMLX90640[832];
    if (MLX90640_DumpEE(MLX90640_ADDRESS, eeMLX90640) != 0) {
        MLX90640_active = false;
        return false;
    }
    MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    MLX90640_SetRefreshRate(MLX90640_ADDRESS, 0x05);
    MLX90640_active = true;
    return true;
}

static bool MLX90640_capture()
{
    for (byte x = 0 ; x < 2 ; x++)
    {
        uint16_t mlxFrame[834];
        MLX90640_GetFrameData(MLX90640_ADDRESS, mlxFrame);
        float Ta = MLX90640_GetTa(mlxFrame, &mlx90640);
        float tr = Ta - TA_SHIFT;
        float emissivity = 0.95;
        MLX90640_CalculateTo(mlxFrame, &mlx90640, emissivity, tr, MLX90640_pixels);
        int mode_ = MLX90640_GetCurMode(MLX90640_ADDRESS);
        MLX90640_BadPixelsCorrection(mlx90640.brokenPixels, MLX90640_pixels, mode_, &mlx90640);
    }
    for (int x = 0 ; x < 768 ; x++)
    {
        if (x % 32 == 0)
        {
            for (int j = 0 + x, k = 31 + x; j < 32 + x ; j++, k--)
            {
                MLX90640_pixels_flipped[j] = MLX90640_pixels[k];
            }
        }
    }
    for (int i = 0; i < 768; i++)
    {
        float val = MLX90640_pixels_flipped[i];
        int colorTemp;
        if (val >= MAXTEMP) colorTemp = MAXTEMP;
        else if (val <= MINTEMP) colorTemp = MINTEMP;
        else colorTemp = val;
        uint8_t colorIndex = map(colorTemp, MINTEMP, MAXTEMP, 0, 255);
        MLX90640_image[i] = constrain(colorIndex, 0, 255);
    }
    return true;
}

static void MLX90640_draw_image(int offx, int offy)
{
    for (int y = 0; y < 24; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            uint8_t idx = MLX90640_image[y * 32 + x];
            M5.Lcd.fillRect(offx + 2 * x, offy + 2 * y, 2, 2, camColors[idx]);
        }
    }
}

static void MLX90640_draw_white_image(int offx, int offy)
{
            M5.Lcd.fillRect(offx, offy , 64, 48, ez.screen.background()); // clear the image area);     
}

String MLX90640()
{
    
    if (PaHUB_active && MLX90640_port >= 0) selectPaHUBChannel(MLX90640_port);

    if (!MLX90640_active) MLX90640_init();
    if (!MLX90640_active) {
        LastError("MLX90640 not found");
        return "-";
    }

    if (command == "GET_IMAGE")
    {
        if (!MLX90640_capture()) {
            LastError("Error reading image");
            return "-";
        }
        // Increment frame counter
        frameCount++;
        for (int i = 0; i < 768; i++)
        {
            write2serial(MLX90640_image[i]);
        }
        return String(frameCount);
    }

    LastError("No valid command found");
    return "-";
}
