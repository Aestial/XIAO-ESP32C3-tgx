/**
 * @brief tgx library example : displaying a 3D mesh... 
 * 
 */

#include <TFT_eSPI.h>

#define USE_TFT_ESPI_LIBRARY
#include "lv_xiao_round_screen.h"

#include <tgx.h>

#include "naruto.h"

using namespace tgx;

#define SLX 240
#define SLY 240

// tft already declared in lv_xiao_round_screen.h
// TFT_eSPI tft = TFT_eSPI();

uint16_t fb[SLX * SLY]; //Framebuffer to draw onto.

// Second framebuffer used by eSPI_TFT for DMA update
uint16_t* fb2; // allocated via malloc

// the z-buffer in 16 bits precision.
uint16_t* zbuf;

// the image that encapsulate framebuffer fb
Image<RGB565> imgfb(fb, SLX, SLY);

// only load the shaders we need
const Shader LOADED_SHADERS = SHADER_PERSPECTIVE | SHADER_ZBUFFER | SHADER_FLAT | SHADER_GOURAUD | SHADER_NOTEXTURE | SHADER_TEXTURE_NEAREST | SHADER_TEXTURE_WRAP_POW2;

Renderer3D<RGB565, LOADED_SHADERS, uint16_t> renderer;



void setup() 
{
  Serial.begin(115200);

  // allocate second framebuffer
  fb2 = (uint16_t*)malloc(SLX * SLY * sizeof(uint16_t));
  while (fb2 == nullptr)
  {
    Serial.println("Error: cannot allocate memory for fb2");
    delay(1000);
  }

  
  // allocate the zbuffer
  zbuf = (uint16_t*)malloc(SLX * SLY * sizeof(uint16_t));
  while(zbuf == nullptr)
  {
    Serial.println("Error: cannot allocate memory for zbuff");
    delay(1000);
  }

  // initialize the screen driver
  tft.init();
  tft.setRotation(0);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}