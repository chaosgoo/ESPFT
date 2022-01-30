#include <Arduino.h>
#include <TFT_eSPI.h>
extern "C"
{
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H
}
typedef uint16_t st7789_color_t;

#define st7789_rgb_to_color(r, g, b)                                        \
  ((((st7789_color_t)(r) >> 3) << 11) | (((st7789_color_t)(g) >> 2) << 5) | \
   ((st7789_color_t)(b) >> 3))

inline void __attribute__((always_inline))
st7789_color_to_rgb(st7789_color_t color, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *b = (color << 3);
  color >>= 5;
  color <<= 2;
  *g = color;
  color >>= 8;
  *r = color << 3;
}
uint8_t st7789_dither_table[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

inline st7789_color_t __attribute__((always_inline))
st7789_rgb_to_color_dither(uint8_t r, uint8_t g, uint8_t b, uint16_t x,
                           uint16_t y)
{
  const uint8_t pos = ((y << 8) + (y << 3) + x) & 0xff;
  uint8_t rand_b = st7789_dither_table[pos];
  const uint8_t rand_r = rand_b & 0x07;
  rand_b >>= 3;
  const uint8_t rand_g = rand_b & 0x03;
  rand_b >>= 2;

  if (r < 249)
  {
    r = r + rand_r;
  }
  if (g < 253)
  {
    g = g + rand_g;
  }
  if (b < 249)
  {
    b = b + rand_b;
  }
  return st7789_rgb_to_color(r, g, b);
}

// extern const uint8_t ttf_start[] asm("_binary_src_Lite_ttf_start");
// extern const uint8_t ttf_end[] asm("_binary_src_Lite_ttf_end");

extern const uint8_t ttf_start[] asm("_binary_src_Lite2_ttf_start");
extern const uint8_t ttf_end[] asm("_binary_src_Lite2_ttf_end");

TFT_eSPI tft = TFT_eSPI();
static FT_Library library;
FT_Error error;
static FT_Face face;
void initFreeType()
{
  error = FT_Init_FreeType(&library);
  printf("FT_Init_FreeType(&library):%d\n", error);
}

void loadCharacter(char character, int pixel_height)
{
  error =
      FT_New_Memory_Face(library, ttf_start, ttf_end - ttf_start - 1, 0, &face);
  printf(
      "FT_New_Memory_Face(library, ttf_start, ttf_end - ttf_start - 1, 0, "
      "&face):%d\n",
      error);
  FT_Select_Charmap(face, FT_ENCODING_UNICODE);

  error = FT_Set_Pixel_Sizes(face,          /* handle to face object */
                             0,             /* pixel_width           */
                             pixel_height); /* pixel_height          */
  printf("FT_Set_Pixel_Sizes(face, 0,  %d):%d\n", pixel_height, error);
  FT_UInt glyph_index = FT_Get_Char_Index(face, (uint32_t)character);
  printf("glyph_index:%d\n", glyph_index);
  error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
  printf("FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT):%d\n", error);
  error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
  printf("FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL):%d\n", error);
  FT_Bitmap bitmap;
  FT_Bitmap_New(&bitmap);
  FT_Bitmap_Convert(library, &face->glyph->bitmap, &bitmap, 1);
  size_t pos = 0;
  for (size_t y = 0; y < bitmap.rows; ++y)
  {
    for (size_t x = 0; x < bitmap.width; ++x)
    {
      int color =
          bitmap.buffer[y * face->glyph->bitmap.pitch + x];
      // bitmap[pos >> 2] |= ((color >> 6) << ((pos & 0x03) << 1));
      printf("%d", color);
      if ((++pos % bitmap.pitch) == 0)
      {
        printf("\n");
      }
    }
  }
  tft.pushImage(0, 0, bitmap.width, bitmap.rows,
                bitmap.buffer);
}

void setup()
{
  Serial.begin(115200);
  tft.init();
  tft.initDMA(true);
  tft.fillScreen(TFT_RED);
  initFreeType();
  Serial.printf("Done.\n");
}
int ph = 32;
int flag = 1;
void loop()
{
  tft.fillScreen(TFT_BLACK);
  ph += flag;
  loadCharacter('a', ph);
  Serial.printf("loadCharacter Done.\n");
  uint8_t *bitmap;
  bitmap = (uint8_t *)malloc(face->glyph->bitmap.width *
                             face->glyph->bitmap.rows * 2);
  memset(bitmap, 0, face->glyph->bitmap.width * face->glyph->bitmap.rows * 2);

  // size_t pos = 0;
  // for (size_t y = 0; y < face->glyph->bitmap.rows; ++y)
  // {
  //   for (size_t x = 0; x < face->glyph->bitmap.width; ++x)
  //   {
  //     uint8_t color =
  //         face->glyph->bitmap.buffer[y * face->glyph->bitmap.pitch + x];
  //     bitmap[pos >> 2] |= ((color >> 6) << ((pos & 0x03) << 1));
  //     Serial.printf("%d", color);
  //     if ((++pos % face->glyph->bitmap.pitch) == 0)
  //     {
  //       Serial.println();
  //     }
  //   }
  // }
  // Serial.printf("w:%d,h:%d,pixel_mode:%d,num_grays:%d, pitch:%d\n",
  //               face->glyph->bitmap.width, face->glyph->bitmap.rows,
  //               face->glyph->bitmap.pixel_mode, face->glyph->bitmap.num_grays,
  //               face->glyph->bitmap.pitch);
  // tft.pushImage(0, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows,
  //               face->glyph->bitmap.buffer);

  delay(1000);
  free(bitmap);
  FT_Done_Face(face);
  if (ph > 120)
  {
    flag = -1;
  }
  if (ph < 14)
  {
    flag = 1;
  }
}