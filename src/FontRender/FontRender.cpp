#include "FontRender.h"
FontRender::FontRender() {}
FontRender::FontRender(FontInfo fontInfo)
{
    fontData = fontInfo;
    FT_Error error = FT_Init_FreeType(&library);
    if (error)
    {
        printf("FT_Init_FreeType error(%d)", error);
        return;
    }

    error = FT_New_Memory_Face(library, fontData.file_base, fontData.file_size, fontData.face_index, &face);

    FT_Set_Pixel_Sizes(face, 0, (FT_UInt)fontInfo.font_width);

    if (error)
    {
        printf("FT_New_Memory_Face error:%d\n", error);
    }
    error = FTC_Manager_New(library, 0, 0, FREETYPE_CACHE_SIZE,
                            font_face_requester, face, &cache_manager);
    if (error)
    {
        printf("FTC_Manager_New error(%d)", error);
        return;
    }
    font_type.face_id = (FTC_FaceID)1;
    font_type.width = (FT_UInt)(fontData.font_width);
    font_type.height = (FT_UInt)(fontData.font_width);
    font_type.flags = FT_LOAD_DEFAULT;

    error = FTC_CMapCache_New(cache_manager, &cmap_cache);
    if (error)
    {
        printf("FTC_CMapCache_New error(%d)", error);
        return;
    }
    error = FTC_SBitCache_New(cache_manager, &sbit_cache);
    if (error)
    {
        printf("FTC_SBitCache_New error(%d)", error);
        return;
    }
    printf("FontRender Created (%p)\n", this);

    FT_UInt glyph_idx;
    FTC_ScalerRec current_ic;
    FTC_Scaler selected_ic;
    printf("FTC_CMapCache_Lookup\n");
    glyph_idx = FTC_CMapCache_Lookup(cmap_cache, (FTC_FaceID)1, -1, 'A');
    if (glyph_idx)
    {
        selected_ic = &current_ic;
    }
    printf("FTC_SBitCache_Lookup\n");
    if (FTC_SBitCache_LookupScaler(sbit_cache, selected_ic, FT_LOAD_DEFAULT,
                                   glyph_idx, &sbit, (FTC_Node *)NULL))
    {
        printf("error");
    }


}

FontRender::~FontRender()
{
    FTC_Manager_Done(cache_manager);
    FT_Done_FreeType(library);
}

FT_Error FontRender::font_face_requester(FTC_FaceID face_id, FT_Library library_is,
                                         FT_Pointer req_data, FT_Face *aface)
{
    printf("font_face_requester charmap_index:%p\n", aface);

    FT_UNUSED(face_id);
    FT_UNUSED(library_is);

    *aface = (FT_Face)req_data;
    return FT_Err_Ok;
}

const uint8_t *FontRender::getCharBitmap(FT_Long unicode)
{
    FT_UInt charmap_index = FT_Get_Charmap_Index(face->charmap);
    // FTC_ImageTypeRec desc_sbit_type;
    FT_UInt glyph_index =
        FTC_CMapCache_Lookup(cmap_cache, font_type.face_id, charmap_index, unicode);
    printf("FTC_CMapCache_Lookup Done\n");
    // FT_Error error = FTC_SBitCache_Lookup(sbit_cache, &desc_sbit_type,
    //                                       glyph_index, &sbit, NULL);
    if (error)
    {
        printf("FTC_SBitCache_Lookup error:%d\n", error);
    }
    return (const uint8_t *)sbit->buffer;
}
FT_Bool FontRender::setFontWeight(FT_Size weight)
{
}
FT_Bool FontRender::setFontWidth(FT_Size widht)
{
}
FT_Bool FontRender::setFont(FT_Byte *file_base,
                            FT_Long file_size,
                            FT_Long face_index)
{
    fontData.file_base = file_base;
    fontData.file_size = file_size;
    fontData.face_index = face_index;

    FTC_Manager_Done(cache_manager);
}
FT_Bool FontRender::setFont(FontInfo fontInfo)
{
    fontData = fontInfo;
    return 1;
}