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
  Serial.println("Starting setup...");

  pinMode(TOUCH_INT, INPUT_PULLUP);
  Wire.begin();

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

  tft.initDMA();
  tft.startWrite();
  
  // setup the 3D renderer.
  renderer.setViewportSize(SLX,SLY);
  renderer.setOffset(0, 0);
  renderer.setImage(&imgfb); // set the image to draw onto (ie the screen framebuffer)
  renderer.setZbuffer(zbuf); // set the z buffer for depth testing
  renderer.setPerspective(45, ((float)SLX) / SLY, 1.0f, 100.0f); // set the perspective projection matrix.
  renderer.setMaterial(RGBf(0.85f, 0.55f, 0.25f), 0.2f, 0.7f, 0.8, 64); // bronze color with a lot of specular reflection.
  renderer.setCulling(1);
  renderer.setTextureQuality(SHADER_TEXTURE_NEAREST);
  renderer.setTextureWrappingMode(SHADER_TEXTURE_WRAP_POW2);

  Serial.println("Setup complete!");
}

tgx::fMat4 moveModel(int &);

int loopnumber = 0;
int prev_loopnumber = -1;

/** Main loop */
void loop() {
  Serial.println("Loop step...");

  // compute the model position
  fMat4 M = moveModel(loopnumber);
  renderer.setModelMatrix(M);

  // draw the 3D mesh
  imgfb.fillScreen(RGB565_Navy);  // clear the framebuffer (black background)
  renderer.clearZbuffer();        // clear the z-buffer

  // choose the shader to use
  switch (loopnumber % 4)
  {
    case 0: renderer.setShaders(SHADER_GOURAUD | SHADER_TEXTURE);
            renderer.drawMesh(&naruto_1, false);
            break;

    case 1: renderer.drawWireFrameMesh(&naruto_1,true);
            break;

    case 2: renderer.setShaders(SHADER_FLAT);
            renderer.drawMesh(&naruto_1, false);
            break;

    case 3: renderer.setShaders(SHADER_GOURAUD);
            renderer.drawMesh(&naruto_1, false);
            break;
  }

  if (prev_loopnumber != loopnumber)
  {
    prev_loopnumber = loopnumber;
    tft.fillRect(0, 300, 240, 20, TFT_BLACK);
    tft.setCursor(5,305);
    switch(loopnumber % 4)
    {
      case 0: tft.print("Gouraud shading / texturing"); break;
      case 1: tft.print("Wireframe"); break;
      case 2: tft.print("Flat shading"); break;
      case 3: tft.print("Gouraud shading"); break;
    }
  }

  // Upload the framebuffer to the screen (async, via DMA)
  tft.dmaWait();
  tft.pushImageDMA((tft.width() - SLX) / 2, (tft.height() - SLY) / 2, SLX, SLY, fb, fb2);
}

tgx::fMat4 moveModel(int &loopnumber)
{
  Serial.println("Moving model.");

  const float end1 = 6000;
  const float end2 = 2000;
  const float end3 = 6000;
  const float end4 = 2000;

  int tot = (int)(end1 + end2 + end3 + end4);
  int m = millis();

  loopnumber = m / tot;
  float t = m % tot;

  const float dilat = 9; //scale model
  const float roty = 360 * (t / 4000); // rotate 1 turn every 4 seconds
  float tz, ty;
  if (t < end1)
  { // far away
    tz = -25;
    ty = 0;
  }
  else 
  {
    t -= end1;
    if (t < end2)
    { // zooming in
      t /= end2;
      tz = -25 + 18 * t;
      ty = -6.5f * t;
    }
    else 
    {
      t -= end2;
      if (t < end3)
      { // close up
        tz = -7;
        ty = -6.5f;
      }
      else 
      { // zooming out
        t -= end3;
        t /= end4;
        tz = -7 -18 * t;
        ty = -6.5 + 6.5 * t;
      }
    }
  }

  fMat4 M;
  M.setScale({ dilat, dilat, dilat }); // scale the model
  M.multRotate(-roty, { 0,1,0 }); // rotate around y
  M.multTranslate({ 0, ty, tz }); // translate
  return M;
}