/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "sdl_ssd1331.h"
#include "sdl_oled_basic.h"
#include "sdl_graphics.h"

static int s_activeColumn = 0;
static int s_activePage = 0;
static int s_columnStart = 0;
static int s_columnEnd = 127;
static int s_pageStart = 0;
static int s_pageEnd = 7;

static int sdl_ssd1331_detect(uint8_t data)
{
    return (data == 0xBE);
}

static uint8_t s_verticalMode = 1;

static void sdl_ssd1331_commands(uint8_t data)
{
    switch (s_commandId)
    {
        case 0xA0:
            if (s_cmdArgIndex == 0)
            {
                s_verticalMode = data & 0x01;
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0x15:
            switch (s_cmdArgIndex)
            {
                case 0:
                     s_columnStart = data;
                     s_activeColumn = data;
                     break;
                case 1:
                     s_columnEnd = data;
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0x75:
            switch (s_cmdArgIndex)
            {
                case 0:
                     s_activePage = data;
                     s_pageStart = data;
                     break;
                case 1:
                     s_pageEnd = data;
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        default:
            s_commandId = SSD_COMMAND_NONE;
            break;
    }
}


void sdl_ssd1331_data(uint8_t data)
{
    int y = s_activePage;
    int x = s_activeColumn;
    sdl_put_pixel(x, y, data);

    if (s_verticalMode)
    {
        s_activePage++;
        if (s_activePage > s_pageEnd)
        {
            s_activePage = s_pageStart;
            s_activeColumn++;
            if (s_activeColumn > s_columnEnd)
            {
                s_activeColumn = s_columnStart;
            }
        }
    }
    else
    {
        s_activeColumn++;
        if (s_activeColumn > s_columnEnd)
        {
            s_activeColumn = s_columnStart;
            s_activePage++;
            if (s_activePage > s_pageEnd)
            {
                s_activePage = s_pageStart;
            }
        }
    }
}

sdl_oled_info sdl_ssd1331 =
{
    .width = 96,
    .height = 64,
    .bpp = 8,
    .pixfmt = SDL_PIXELFORMAT_RGB332,
    .dataMode = SDMS_AUTO,
    .detect = sdl_ssd1331_detect,
    .run_cmd = sdl_ssd1331_commands,
    .run_data = sdl_ssd1331_data,
};
