#pragma once

namespace CK
{

class Texture
{
public:
	Texture() = default;
	Texture(std::string InFileName);
	~Texture();

public:
	void LoadPNG(std::string InFileName);
	void Release();
	bool IsIntialized() const { return Initialized; }
	LinearColor* GetBuffer() const;
	UINT32 GetWidth() const { return Width; }
	UINT32 GetHeight() const { return Height; }
	UINT32 GetSize() const { return Width * Height; }

private:
	bool Initialized = false;
	LinearColor* Buffer = nullptr;
	UINT32 Width = 0;
	UINT32 Height = 0;
};

}

