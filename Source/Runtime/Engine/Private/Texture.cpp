
#include "Precompiled.h"
#include "lodepng.h"
using namespace CK;

Texture::Texture(std::string InFileName)
{
	LoadPNG(InFileName);
}

Texture::~Texture()
{
	Release();
}

void Texture::LoadPNG(std::string InFileName)
{
	std::vector<BYTE> image;
	unsigned int error = lodepng::decode(image, Width, Height, InFileName);
	if (error != NULL)
	{
		Width = 0;
		Height = 0;
		Initialized = false;
	}

	Buffer = new LinearColor[Width * Height];
	for (BYTE j = 0; j < Height; j++)
	{
		for (BYTE i = 0; i < Width; i++)
		{
			int pixelIndex = j * Width + i;
			int imageIndex = pixelIndex * 4;

			Buffer[j * Width + i] = LinearColor(
				Color32(
					image[imageIndex],
					image[imageIndex + 1],
					image[imageIndex + 2],
					image[imageIndex + 3]
				)
			);
		}
	}

	Initialized = true;
}

void Texture::Release()
{
	if (Initialized)
	{
		delete[] Buffer;
		Buffer = nullptr;
	}

	Initialized = false;
}

LinearColor* Texture::GetBuffer() const
{
	return Buffer;
}
