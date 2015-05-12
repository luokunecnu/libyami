/*
 *  vaapidecoder_host.cpp - create specific type of video decoder
 *
 *  Copyright (C) 2013 Intel Corporation
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

#include "common/log.h"
#include "interface/VideoDecoderHost.h"
#if __BUILD_H264_DECODER__
#include "vaapidecoder_h264.h"
#endif
#if __BUILD_VP8_DECODER__
#include "vaapidecoder_vp8.h"
#endif
#if __BUILD_VP9_DECODER__
#include "vaapidecoder_vp9.h"
#endif
#if __BUILD_JPEG_DECODER__
#include "vaapidecoder_jpeg.h"
#endif
#if __BUILD_FAKE_DECODER__
#include "vaapidecoder_fake.h"
#endif
#include "vaapi/vaapi_host.h"
#include <string.h>

using namespace YamiMediaCodec;
DEFINE_CLASS_FACTORY(Decoder)
static const DecoderEntry g_decoderEntries[] = {
#if __BUILD_H264_DECODER__
    DEFINE_DECODER_ENTRY(YAMI_MIME_AVC, H264),
    DEFINE_DECODER_ENTRY(YAMI_MIME_H264, H264),
#endif
#if __BUILD_JPEG_DECODER__
    DEFINE_DECODER_ENTRY(YAMI_MIME_JPEG, Jpeg),
#endif
#if __BUILD_VP8_DECODER__
    DEFINE_DECODER_ENTRY(YAMI_MIME_VP8, VP8),
#endif
#if __BUILD_VP9_DECODER__
    DEFINE_DECODER_ENTRY(YAMI_MIME_VP9, VP9)
#endif
};

#ifndef N_ELEMENTS
#define N_ELEMENTS(array) (sizeof(array)/sizeof(array[0]))
#endif
extern "C" {
IVideoDecoder *createVideoDecoder(const char *mimeType)
{
    yamiTraceInit();
    if (!mimeType) {
        ERROR("NULL mime type.");
        return NULL;
    }
#if __BUILD_FAKE_DECODER__
    return new VaapiDecoderFake(1920, 1080);
#endif
    INFO("mimeType: %s\n", mimeType);
    for (int i = 0; i < N_ELEMENTS(g_decoderEntries); i++) {
        const DecoderEntry *e = g_decoderEntries + i;
        if (strcasecmp(e->mime, mimeType) == 0)
            return e->create();
    }
    ERROR("Failed to create %s", mimeType);
    return NULL;
}

void releaseVideoDecoder(IVideoDecoder * p)
{
    delete p;
}

bool preSandboxInitDecoder()
{
    // TODO, for hybrid Decoder uses mediasdk, does the prework here
    return true;
}
} // extern "C"
