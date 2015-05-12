/*
 *  utils.cpp - utils functions
 *
 *  Copyright (C) 2011-2014 Intel Corporation
 *    Author: Xu Guangxin <guangxin.xu@intel.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "utils.h"

#include "common/common_def.h"
#include "common/log.h"

#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <va/va.h>

namespace YamiMediaCodec{

uint32_t guessFourcc(const char* fileName)
{
    static const char *possibleFourcc[] = {
            "I420", "NV12", "YV12",
            "YUY2", "UYVY",
            "RGBX", "BGRX", "XRGB", "XBGR"
        };
    const char* extension = strrchr(fileName, '.');
    if (extension) {
        extension++;
        for (int i = 0; i < N_ELEMENTS(possibleFourcc); i++) {
            const char* fourcc = possibleFourcc[i];
            if (!strcasecmp(fourcc, extension)) {
                uint32_t ret = VA_FOURCC(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);
                DEBUG_FOURCC("guessed input fourcc:", ret);
                return ret;
            }
        }
    }

    return VA_FOURCC_I420;
}

bool getPlaneResolution(uint32_t fourcc, uint32_t pixelWidth, uint32_t pixelHeight, uint32_t byteWidth[3], uint32_t byteHeight[3],  uint32_t& planes)
{
    int w = pixelWidth;
    int h = pixelHeight;
    uint32_t* width = byteWidth;
    uint32_t* height = byteHeight;
    switch (fourcc) {
        case VA_FOURCC_NV12:
        case VA_FOURCC_I420:
        case VA_FOURCC_YV12:{
            width[0] = w;
            height[0] = h;
            if (fourcc == VA_FOURCC_NV12) {
                width[1]  = w + (w & 1);
                height[1] = (h + 1) >> 1;
                planes = 2;
            } else {
                width[1] = width[2] = (w + 1) >> 1;
                height[1] = height[2] = (h + 1) >> 1;
                planes = 3;
            }
            break;
        }
        case VA_FOURCC_YUY2:
        case VA_FOURCC_UYVY: {
            width[0] = w * 2;
            height[0] = h;
            planes = 1;
            break;
        }
        case VA_FOURCC_RGBX:
        case VA_FOURCC_RGBA:
        case VA_FOURCC_BGRX:
        case VA_FOURCC_BGRA: {
            width[0] = w * 4;
            height[0] = h;
            planes = 1;
            break;
        }
        default: {
            assert(0 && "do not support this format");
            planes = 0;
            return false;
        }
    }
    return true;
}

/// return system clock in ms
uint64_t getSystemTime()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL))
        return 0;
    return tv.tv_usec/1000+tv.tv_sec*1000;
}

void CalcFps::setAnchor()
{
    m_timeStart = getSystemTime();
}

float CalcFps::fps(uint32_t frameCount)
{
    if (!m_timeStart) {
        fprintf(stderr, "anchor point isn't set, please call setAnchor first\n");
        return 0.0f;
    }

    uint64_t sysTime = getSystemTime() - m_timeStart;
    float fps = frameCount*1000.0/sysTime;
    fprintf(stdout, "rendered frame count: %d in %ld ms; fps=%.2f\n",
            frameCount, sysTime, fps);

    return fps;
}

bool fillFrameRawData(VideoFrameRawData* frame, uint32_t fourcc, uint32_t width, uint32_t height, uint8_t* data)
{
    memset(frame, 0, sizeof(*frame));
    uint32_t planes;
    uint32_t w[3], h[3];
    if (!getPlaneResolution(fourcc, width, height, w, h, planes))
        return false;
    frame->fourcc = fourcc;
    frame->width = width;
    frame->height = height;
    frame->handle = reinterpret_cast<intptr_t>(data);
    frame->memoryType = VIDEO_DATA_MEMORY_TYPE_RAW_POINTER;

    uint32_t offset = 0;
    for (int i = 0; i < planes; i++) {
        frame->pitch[i] = w[i];
        frame->offset[i] = offset;
        offset += w[i] * h[i];
    }
    return true;
}

};
