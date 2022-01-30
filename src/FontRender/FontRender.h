#if !defined(FONT_RENDER_H_)
#define FONT_RENDER_H_
#include <stdint.h>
extern "C"
{
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H
#include FT_SIZES_H
}

#define FREETYPE_CACHE_SIZE (16 * 1024)
typedef struct FontInfo
{
    FT_Byte *file_base;
    FT_Long file_size;
    FT_Long face_index;
    FT_Size font_width;
    FT_Size font_weight;
} FontInfo_t;

class FontRender
{
private:
    static FT_Error font_face_requester(FTC_FaceID face_id, FT_Library library_is,
                                        FT_Pointer req_data, FT_Face *aface);
    /* data */
public:
    FontInfo fontData;
    FT_Library library;
    FT_Error error;
    FT_Face face;
    FTC_Manager cache_manager;
    FTC_CMapCache cmap_cache;
    FTC_SBitCache sbit_cache;
    FTC_SBit sbit;
    FTC_ImageTypeRec font_type;
    FontRender(FontInfo fontInfo);
    FontRender();
    const uint8_t *getCharBitmap(FT_Long unicode);
    FT_Bool setFontWeight(FT_Size weight);
    FT_Bool setFontWidth(FT_Size widht);
    FT_Bool setFont(FT_Byte *file_base,
                    FT_Long file_size,
                    FT_Long face_index);
    FT_Bool setFont(FontInfo fontInfo);
    ~FontRender();
};

#endif // FONT_RENDER_H_
