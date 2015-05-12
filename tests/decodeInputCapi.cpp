/*
 *  decodeInputCapi.cpp - decode test input capi
 *
 *  Copyright (C) 2011-2014 Intel Corporation
 *    Author: Xin Tang<xin.t.tang@intel.com>
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

#include "decodeInputCapi.h"
#include "decodeinput.h"

using namespace YamiMediaCodec;

DecodeInputHandler createDecodeInput(char *fileName)
{
    return DecodeInput::create(fileName);
}

const char * getMimeType(DecodeInputHandler input)
{
    if(input)
        return ((DecodeInput*)input)->getMimeType();
    else
        return NULL;
}

bool decodeInputIsEOS(DecodeInputHandler input)
{
    if(input)
        return ((DecodeInput*)input)->isEOS();
}

bool getNextDecodeUnit(DecodeInputHandler input, VideoDecodeBuffer *inputbuffer)
{
    if(input)
        return ((DecodeInput*)input)->getNextDecodeUnit(*inputbuffer);
    else
        return false;
}

void releaseDecodeInput(DecodeInputHandler input)
{
    if(input)
        delete (DecodeInput*)input;
}
