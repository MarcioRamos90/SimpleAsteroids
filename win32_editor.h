#if !define(SPACE_PROGRAM_H)

struct win32_screen_buffer
{
    u32 Width;
    u32 Height;
    u32 Pitch;
    u32 BytesPerPixel;

    BITMAPINFO Info;
    void* Memory;
};

#define SPACE_PROGRAM_H
#endif
