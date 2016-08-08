
// Author: Christian Vallentin <mail@vallentinsource.com>
// Website: http://vallentinsource.com
// Repository: https://github.com/MrVallentin/LoadBMP
//
// Date Created: January 03, 2014
// Last Modified: August 08, 2016
//
// Version: 1.0.0

// Copyright (c) 2014-2016 Christian Vallentin <mail@vallentinsource.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.

// Include loadbmp.h as following
// to create the implementation file.
//
// #define LOADBMP_IMPLEMENTATION
// #include "loadbmp.h"

#ifndef LOADBMP_H
#define LOADBMP_H

#ifdef __cplusplus
extern "C" {
#endif


// Errors
#define LOADBMP_NO_ERROR 0

#define LOADBMP_OUT_OF_MEMORY 1

#define LOADBMP_FILE_NOT_FOUND 2
#define LOADBMP_FILE_OPERATION 3

#define LOADBMP_INVALID_SIGNATURE 4
#define LOADBMP_INVALID_BITS_PER_PIXEL 5


// Components
#define LOADBMP_RGB  3
#define LOADBMP_RGBA 4


#ifdef LOADBMP_IMPLEMENTATION
#	define LOADBMP_API
#else
#	define LOADBMP_API extern
#endif


// LoadBMP uses raw buffers and support RGB and RGBA formats.
// The order is RGBRGBRGB... or RGBARGBARGBA..., from top left
// to bottom right, without any padding.


LOADBMP_API unsigned int loadbmp_encode_file(
	const char *filename, const unsigned char *pixels, unsigned int width, unsigned int height, unsigned int components);

LOADBMP_API unsigned int loadbmp_decode_file(
	const char *filename, unsigned char **pixels, unsigned int *width, unsigned int *height, unsigned int components);


#ifdef LOADBMP_IMPLEMENTATION

// Disable Microsoft Visual C++ compiler security warnings for fopen, strcpy, etc being unsafe
#if defined(_MSC_VER) && (_MSC_VER >= 1310)
#	pragma warning(disable: 4996)
#endif

#include <stdlib.h> /* malloc() */
#include <string.h> /* memset() */
#include <stdio.h> /* fopen(), fwrite(), fread(), fclose() */


LOADBMP_API unsigned int loadbmp_encode_file(
	const char *filename, const unsigned char *pixels, unsigned int width, unsigned int height, unsigned int components)
{
	FILE *f = fopen(filename, "wb");

	if (!f)
		return LOADBMP_FILE_OPERATION;

	unsigned char bmp_file_header[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0 };
	unsigned char bmp_info_header[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0 };
	unsigned char bmp_pad[3] = { 0, 0, 0 };

	const unsigned int size = 54 + width * height * 3; // 3 as the BMP format uses 3 channels (red, green, blue and NO alpha)

	bmp_file_header[2] = (unsigned char)(size);
	bmp_file_header[3] = (unsigned char)(size >> 8);
	bmp_file_header[4] = (unsigned char)(size >> 16);
	bmp_file_header[5] = (unsigned char)(size >> 24);

	bmp_info_header[4] = (unsigned char)(width);
	bmp_info_header[5] = (unsigned char)(width >> 8);
	bmp_info_header[6] = (unsigned char)(width >> 16);
	bmp_info_header[7] = (unsigned char)(width >> 24);

	bmp_info_header[8] = (unsigned char)(height);
	bmp_info_header[9] = (unsigned char)(height >> 8);
	bmp_info_header[10] = (unsigned char)(height >> 16);
	bmp_info_header[11] = (unsigned char)(height >> 24);

	fwrite(bmp_file_header, 1, 14, f);
	fwrite(bmp_info_header, 1, 40, f);

	unsigned int x, y, index;

	for (y = (height - 1); y != -1; y--)
	{
		for (x = 0; x < width; x++)
		{
			index = (x + y * width) * components;

			fwrite(pixels + index + 2, sizeof(unsigned char), 1, f);
			fwrite(pixels + index + 1, sizeof(unsigned char), 1, f);
			fwrite(pixels + index + 0, sizeof(unsigned char), 1, f);
		}

		fwrite(bmp_pad, 1, ((4 - (width * 3) % 4) % 4), f);
	}

	fclose(f);

	return LOADBMP_NO_ERROR;
}


LOADBMP_API unsigned int loadbmp_decode_file(
	const char *filename, unsigned char **pixels, unsigned int *width, unsigned int *height, unsigned int components)
{
	FILE *f = fopen(filename, "rb");

	if (!f)
		return LOADBMP_FILE_NOT_FOUND;

	unsigned char bmp_file_header[14];
	unsigned char bmp_info_header[40];
	unsigned char bmp_pad[3];

	memset(bmp_file_header, 0, sizeof(bmp_file_header));
	memset(bmp_info_header, 0, sizeof(bmp_info_header));

	fread(bmp_file_header, sizeof(bmp_file_header), 1, f);
	fread(bmp_info_header, sizeof(bmp_info_header), 1, f);

	if ((bmp_file_header[0] != 'B') || (bmp_file_header[1] != 'M'))
	{
		fclose(f);
		return LOADBMP_INVALID_SIGNATURE;
	}

	if ((bmp_info_header[14] != 24) && (bmp_info_header[14] != 32))
	{
		fclose(f);
		return LOADBMP_INVALID_BITS_PER_PIXEL;
	}

	const unsigned int w = (bmp_info_header[4] + (bmp_info_header[5] << 8) + (bmp_info_header[6] << 16) + (bmp_info_header[7] << 24));
	const unsigned int h = (bmp_info_header[8] + (bmp_info_header[9] << 8) + (bmp_info_header[10] << 16) + (bmp_info_header[11] << 24));

	unsigned char *p = NULL;

	if ((w > 0) && (h > 0))
	{
		p = (unsigned char*)malloc(w * h * components);

		if (!p)
		{
			fclose(f);
			return LOADBMP_OUT_OF_MEMORY;
		}

		unsigned int x, y, index;

		for (y = (h - 1); y != -1; y--)
		{
			for (x = 0; x < w; x++)
			{
				index = (x + y * w) * components;

				fread(p + index + 2, sizeof(unsigned char), 1, f);
				fread(p + index + 1, sizeof(unsigned char), 1, f);
				fread(p + index + 0, sizeof(unsigned char), 1, f);

				if (components == LOADBMP_RGBA)
					p[index + 3] = 255;
			}

			fread(bmp_pad, 1, ((4 - (w * 3) % 4) % 4), f);
		}
	}

	(*width) = w;
	(*height) = h;
	(*pixels) = p;

	fclose(f);

	return LOADBMP_NO_ERROR;
}


#endif

#ifdef __cplusplus
}
#endif

#endif