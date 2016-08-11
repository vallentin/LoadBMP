
# LoadBMP [![Build Status][LoadBMPBuildStatus]][LoadBMPCI] ![Release][LoadBMPVersionBadge] ![License][LoadBMPLicenseBadge]

[LoadBMP][LoadBMP] is a single-header C library
for loading and saving [BMP][BMP] image files.

[LoadBMP][LoadBMP] has no no dependencies besides the standard C libraries
`stdlib.h`, `string.h` and `stdio.h`.


## Examples

[LoadBMP][LoadBMP] uses raw buffers and support RGB (`LOADBMP_RGB`) and RGBA (`LOADBMP_RGBA`) formats.
The order is RGBRGBRGB... or RGBARGBARGBA..., from top left
to bottom right, without any padding.

The format ultimately only affects the size and layout of the pixel buffer.
Thereby when loading a BMP and selecting `LOADBMP_RGBA`, an alpha value of 255 is added.
When saving the BMP the knowledge of an alpha value is only used to be able
to skip it and properly index the colors.

### Loading BMP

```c
unsigned char *pixels = NULL;
unsigned int width, height;

unsigned int err = loadbmp_decode_file("image.bmp", &pixels, &width, &height, LOADBMP_RGBA);

if (err)
	printf("LoadBMP Load Error: %u\n", err);

// Remember to free the pixel data when it isn't needed anymore.
free(pixels);
```


### Saving BMP

```c
unsigned char *pixels = ...;
unsigned int width, height;

unsigned int err = loadbmp_encode_file("image.bmp", pixels, width, height, LOADBMP_RGBA);

if (err)
	printf("LoadBMP Load Error: %u\n", err);
```


## Reporting Bugs & Requests

Feel free to use the [issue tracker][LoadBMPIssues],
for reporting bugs, submitting patches or requesting features.

Before submitting bugs, make sure that you're using the latest version of [LoadBMP][LoadBMP].


## License

```
Copyright (c) 2016 Christian Vallentin <mail@vallentinsource.com>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not
   be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
   distribution.
```


[LoadBMP]: https://github.com/MrVallentin/LoadBMP
[LoadBMPLicense]: https://github.com/MrVallentin/LoadBMP/blob/master/LICENSE

[LoadBMPBuildStatus]: https://drone.io/github.com/MrVallentin/LoadBMP/status.png
[LoadBMPCI]: https://drone.io/github.com/MrVallentin/LoadBMP/latest

[LoadBMPVersionBadge]: https://img.shields.io/badge/release-v1.0.0-blue.svg
[LoadBMPLicenseBadge]: https://img.shields.io/badge/license-%20free%20to%20use%2C%20share%2C%20modify%20and%20redistribute-blue.svg

[LoadBMPIssues]: https://github.com/MrVallentin/LoadBMP/issues

[BMP]: https://en.wikipedia.org/wiki/BMP_file_format
