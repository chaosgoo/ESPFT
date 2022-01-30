#include "Display.h"
void DisplayInit()
{
    tft.init();
    tft.initDMA(true);
    tft.fillScreen(TFT_RED);
}