// SpriteAtlas — load 24/32-bit BMP into CPU pixel buffer (BGRA)
#include "SpriteAtlas.h"
#include <Windows.h>
#include <cstdio>
#include <cstring>
#include <vector>

namespace inkball {

#pragma pack(push, 1)
struct BmpFileHeader {
    uint16_t bfType;       // "BM"
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BmpInfoHeader {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;     // positive = bottom-up
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

bool SpriteAtlas::LoadFromFile(const std::wstring& path) {
    Clear();

    HANDLE hFile = CreateFileW(
        path.c_str(), GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    DWORD bytesRead = 0;
    BmpFileHeader fileHdr = {};
    ReadFile(hFile, &fileHdr, sizeof(fileHdr), &bytesRead, nullptr);

    if (fileHdr.bfType != 0x4D42) {  // "BM"
        CloseHandle(hFile);
        return false;
    }

    BmpInfoHeader infoHdr = {};
    ReadFile(hFile, &infoHdr, sizeof(infoHdr), &bytesRead, nullptr);

    width = infoHdr.biWidth;
    height = (infoHdr.biHeight < 0) ? -infoHdr.biHeight : infoHdr.biHeight;
    bool topDown = (infoHdr.biHeight < 0);

    int32_t bytesPerPixel = infoHdr.biBitCount / 8;
    int32_t rowSize = ((width * bytesPerPixel + 3) / 4) * 4;  // DWORD aligned

    // Seek to pixel data
    SetFilePointer(hFile, fileHdr.bfOffBits, nullptr, FILE_BEGIN);

    std::vector<uint8_t> rawRow(rowSize);
    pixels.resize(static_cast<size_t>(width) * height, 0);

    for (int32_t y = 0; y < height; ++y) {
        ReadFile(hFile, rawRow.data(), rowSize, &bytesRead, nullptr);

        int32_t destY = topDown ? y : (height - 1 - y);

        for (int32_t x = 0; x < width; ++x) {
            size_t rawOff = static_cast<size_t>(x) * bytesPerPixel;
            size_t dstOff = static_cast<size_t>(destY) * width + x;
            uint32_t& px = pixels[dstOff];

            uint8_t B = rawRow[rawOff];
            uint8_t G = rawRow[rawOff + 1];
            uint8_t R = rawRow[rawOff + 2];
            uint8_t A = (bytesPerPixel >= 4) ? rawRow[rawOff + 3] : 0xFF;

            px = (static_cast<uint32_t>(A) << 24) |
                 (static_cast<uint32_t>(R) << 16) |
                 (static_cast<uint32_t>(G) << 8)  |
                 (static_cast<uint32_t>(B));
        }
    }

    CloseHandle(hFile);
    return true;
}

void SpriteAtlas::Clear() {
    width = 0;
    height = 0;
    pixels.clear();
}

} // namespace inkball
