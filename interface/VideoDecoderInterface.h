/*
 *  VideoDecoderInterface.h- basic va decoder for video
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


#ifndef VIDEO_DECODER_INTERFACE_H_
#define VIDEO_DECODER_INTERFACE_H_
// config.h should NOT be included in header file, especially for the header file used by external

#include "VideoDecoderDefs.h"

namespace YamiMediaCodec {
/**
 * \class IVideoDecoder
 * \brief Abstract video decoding interface of libyami
 *
 * it is the interface with client
 */
class IVideoDecoder {
public:
    virtual ~IVideoDecoder() {}
    /** \brief configure decoder before decode the first frame
    * @param[in] buffer     stream information to initialize decoder
    * if the configuration in @param buffer doesn't match the information in coming stream,
    * future #decode will return DECODE_FORMAT_CHANGE (#VIDEO_DECODE_STATUS), it will trigger
    * a reconfiguration from client. (yami will not reconfigure decoder internally to avoid mismatch between client and libyami)
    */
    virtual Decode_Status start(VideoConfigBuffer *buffer) = 0;
    /// \brief reset decoder with new configuration before decoding a new stream
    /// @param[in] buffer     new stream information to reset decoder. cached data (input data or decoded video frames) are discarded.
    virtual Decode_Status reset(VideoConfigBuffer *buffer) = 0;
    /// stop decoding and destroy sw/hw decoder context.
    virtual void stop(void) = 0;
    /// discard cached data (input data or decoded video frames), it is usually required during seek
    virtual void flush(void) = 0;
    /// continue decoding with new data in @param[in] buffer; send empty data (buffer.data=NULL, buffer.size=0) to indicate EOS
    virtual Decode_Status decode(VideoDecodeBuffer *buffer) = 0;
    /**
     * \brief return one frame to client for display; obsolete API. please use getOutput(XID, ...) or getOutput(VideoFrameRawData, ...) instead.
     * NULL will be returned if no available frame for rendering
     * @param[in] draining drain out all possible frames or not. it is set to true upon EOS.
     * @return a #VideoRenderBuffer to be rendered by client
     */
    virtual const VideoRenderBuffer* getOutput(bool draining = false) = 0;
    /**
     * \brief  render one available video frame to draw
     * @param[in] draw a X11 drawable, Pixmap or Window ID. we do not use Drawable/XID type from X11/X.h because interface should be unique unconditionally
     * @param[in/out] timeStamp, time stamp of current rendering frame (it is passed from client before)
     * @param[in] drawX/drawY/drawWidth/drawHeight specify a rect to render on the draw
     * @param[in] drawX horizontal offset to render on the draw
     * @param[in] drawY vertical offset to render on the draw
     * @param[in] width rendering rect width on the draw
     * @param[in] height render rect height on the draw
     * @param[in] frameX/frameY/frameWidth/frameHeight specify a portion rect for rendering, default means the whole surface to render
     * @param[in] frameX horizontal offset of the video frame to render
     * @param[in] frameY vertical offset of the video frame to render
     * @param[in] frameWidth rect width of the video frame to render
     * @param[in] frameHeight rect height of the video frame to render
     *
     * default value of frameX/frameY/frameWidth/frameHeight means rendering the whole video frame(surface)
     *
     * @return DECODE_SUCCESS for success
     * @return RENDER_NO_AVAILABLE_FRAME when no available frame to render
     * @return RENDER_FAIL when driver fail to do vaPutSurface
     * @return RENDER_INVALID_PARAMETER
     */
    virtual Decode_Status getOutput(unsigned long draw, int64_t *timeStamp
        , int drawX, int drawY, int drawWidth, int drawHeight, bool draining = false
        , int frameX = -1, int frameY = -1, int frameWidth = -1, int frameHeight = -1) = 0;
    /**
     * \brief export one frame to client buffer;
     * there are four type to export one frame (VideoDataMemoryType); after rendering, client return the buffer back by renderDone(VideoFrameRawData*);
     * RENDER_NO_AVAILABLE_FRAME will be returned if no available frame for rendering.
     * if frame->fourcc or/and frame->width/height are set, color conversion or/and resize is done as well.
     * @param[in] draining drain out all possible frames or not. it is set to true upon EOS.
     */
    virtual Decode_Status getOutput(VideoFrameRawData* frame, bool draining = false) = 0;

    /**
    * in chromeos v4l2vda, it creates all EGLImage from output frames at initialization time, not create/destroy each frame texture on the fly.
    * @param[in] frames the exported handles (and attributes) for all output frames.
    * @param[in/out] frameCount. the size of input frames when it is not zero. return the internal output pool size when it is zero.
    * make sure to recycle (renderDone()) these frames.
    */
    virtual Decode_Status populateOutputHandles(VideoFrameRawData *frames, unsigned int &frameCount) = 0;

    /** \brief retrieve updated stream information after decoder has parsed the video stream.
    * client usually calls it when libyami return DECODE_FORMAT_CHANGE in decode().
    */
    virtual const VideoFormatInfo* getFormatInfo(void) = 0;
    /** \brief client recycles buffer back to libyami after the buffer has been rendered.
    *
    * <pre>
    * it is used by current omx client in async rendering mode.
    * if rendering target is passed in getOutput(), then yami can does the rendering directly; this function becomes unnecessary.
    * however, this API is still useful when we export video frame directly for EGLImage (dma_buf).
    * </pre>
    */
    virtual void renderDone(const VideoRenderBuffer* buffer) = 0;

    /** \brief client recycles video frame (retrieve by getOutput) back to libyami after the buffer has been rendered.
    */
    virtual void renderDone(VideoFrameRawData* frame) = 0;

    /// set native display
    virtual void  setNativeDisplay( NativeDisplay * display = NULL) = 0;

    /// lockable is set to false when seek begins and reset to true after seek is done
    /// EOS also set lockable to false
    virtual void releaseLock(bool lockable=false) = 0;

    /// obsolete, make all cached video frame output-able, it can be done by getOutput(draining=true) as well
    virtual void flushOutport(void) = 0;
    /// not interest for now, may be used by Android to accept external video frame memory from gralloc
    virtual void  enableNativeBuffers(void) = 0;
    /// not interest for now, may be used by Android to accept external video frame memory from gralloc
    virtual Decode_Status  getClientNativeWindowBuffer(void *bufferHeader, void *nativeBufferHandle) = 0;
    /// not interest for now, may be used by Android to accept external video frame memory from gralloc
    virtual Decode_Status flagNativeBuffer(void * pBuffer) = 0;
};
}
#endif                          /* VIDEO_DECODER_INTERFACE_H_ */
