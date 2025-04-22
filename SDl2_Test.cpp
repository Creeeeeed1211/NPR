#include <SDL.h>
#include <iostream>
#include <cmath>
#include <algorithm> 
#include<vector>    
#include <sys/stat.h> 
#include <sys/types.h> 
#include <filesystem>
#include <sstream>
#include <map>
#include <tuple>
#include <algorithm>
#include<string>
#include<vector>

int num;
int frameNumber = 0;
const char* frames = "frames/frame_0000.bmp";


void applyGaussianBlur(SDL_Surface* surface, int kernelSize = 5) {
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }

    Uint32* pixels = (Uint32*)surface->pixels;
    int pitch = surface->pitch / 4;

    // 复制原始图像数据
    Uint32* originalPixels = new Uint32[surface->w * surface->h];
    std::copy(pixels, pixels + surface->w * surface->h, originalPixels);

    // 5x5 高斯核 (近似)
    float kernel[5][5] = {
        { 1,  4,  6,  4, 1 },
        { 4, 16, 24, 16, 4 },
        { 6, 24, 36, 24, 6 },
        { 4, 16, 24, 16, 4 },
        { 1,  4,  6,  4, 1 }
    };

    float kernelSum = 256.0f; // 归一化因子

    int halfKernel = kernelSize / 2;

    for (int y = halfKernel; y < surface->h - halfKernel; ++y) {
        for (int x = halfKernel; x < surface->w - halfKernel; ++x) {
            float sumR = 0, sumG = 0, sumB = 0;

            // 计算高斯模糊
            for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
                for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
                    int nx = x + kx;
                    int ny = y + ky;

                    Uint32 pixel = originalPixels[ny * pitch + nx];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, surface->format, &r, &g, &b);

                    float weight = kernel[ky + halfKernel][kx + halfKernel];
                    sumR += r * weight;
                    sumG += g * weight;
                    sumB += b * weight;
                }
            }

            Uint8 finalR = std::min(255, std::max(0, (int)(sumR / kernelSum)));
            Uint8 finalG = std::min(255, std::max(0, (int)(sumG / kernelSum)));
            Uint8 finalB = std::min(255, std::max(0, (int)(sumB / kernelSum)));

            pixels[y * pitch + x] = SDL_MapRGB(surface->format, finalR, finalG, finalB);
        }
    }

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }

    delete[] originalPixels;
}

void adjustBrightness(SDL_Surface* surface, float brightnessFactor) {
    
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }

    Uint32* pixels = (Uint32*)surface->pixels;
    int pitch = surface->pitch / 4;

    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            Uint32 pixel = pixels[y * pitch + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Scale the RGB values by the brightness factor
            Uint8 newR = std::min(255, (int)(r * brightnessFactor));
            Uint8 newG = std::min(255, (int)(g * brightnessFactor));
            Uint8 newB = std::min(255, (int)(b * brightnessFactor));

            // Set the new brighter pixel
            pixels[y * pitch + x] = SDL_MapRGB(surface->format, newR, newG, newB);
        }
    }

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
}

void applyPosterization(SDL_Surface* surface, int numBands) {
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }

    Uint32* pixels = (Uint32*)surface->pixels;
    int pitch = surface->pitch / 4;

    // Calculate intensity step size based on the number of bands
    int bandSize = 256 / numBands;

    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            Uint32 pixel = pixels[y * pitch + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            // Compute intensity (grayscale value)
            int intensity = (r + g + b) / 3;

            // Quantize intensity to the nearest band
            int quantizedIntensity = (intensity / bandSize) * bandSize;

            // Scale RGB components proportionally to match quantized intensity
            float scale = quantizedIntensity / 255.0f;
            Uint8 newR = (Uint8)(r * scale);
            Uint8 newG = (Uint8)(g * scale);
            Uint8 newB = (Uint8)(b * scale);

            // Set the new posterized pixel
            pixels[y * pitch + x] = SDL_MapRGB(surface->format, newR, newG, newB);
        }
    }

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
}

//Apply a simple box blur to simulate watercolor bleeding
void applyBoxBlur(SDL_Surface* surface, int kernelSize = 3) {
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }

    Uint32* pixels = (Uint32*)surface->pixels;
    int pitch = surface->pitch / 4;

    // Create a copy of the original image for reference during blurring
    Uint32* originalPixels = new Uint32[surface->w * surface->h];
    std::copy(pixels, pixels + surface->w * surface->h, originalPixels);

    int halfKernel = kernelSize / 2;

    for (int y = halfKernel; y < surface->h - halfKernel; ++y) {
        for (int x = halfKernel; x < surface->w - halfKernel; ++x) {
            int sumR = 0, sumG = 0, sumB = 0, count = 0;

            // Apply box blur kernel
            for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
                for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
                    int nx = x + kx;
                    int ny = y + ky;

                    Uint32 pixel = originalPixels[ny * pitch + nx];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, surface->format, &r, &g, &b);

                    sumR += r;
                    sumG += g;
                    sumB += b;
                    count++;
                }
            }

            Uint8 avgR = sumR / count;
            Uint8 avgG = sumG / count;
            Uint8 avgB = sumB / count;

            // Blend blurred result with the original color for softness
            Uint32 originalPixel = originalPixels[y * pitch + x];
            Uint8 origR, origG, origB;
            SDL_GetRGB(originalPixel, surface->format, &origR, &origG, &origB);

            Uint8 finalR = (avgR * 0.8 + origR * 0.2);
            Uint8 finalG = (avgG * 0.8 + origG * 0.2);
            Uint8 finalB = (avgB * 0.8 + origB * 0.2);

            pixels[y * pitch + x] = SDL_MapRGB(surface->format, finalR, finalG, finalB);
        }
    }

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }

    delete[] originalPixels;
}
// Apply Sobel edge detection filter for edge enhancement
void applySobelFilter(SDL_Surface* surface) {
    SDL_LockSurface(surface);

    Uint32* pixels = (Uint32*)surface->pixels;
    int pitch = surface->pitch / 4;

    Uint32* originalPixels = new Uint32[surface->w * surface->h];
    std::copy(pixels, pixels + surface->w * surface->h, originalPixels);

    int gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int gy[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    for (int y = 1; y < surface->h - 1; ++y) {
        for (int x = 1; x < surface->w - 1; ++x) {
            int sumX = 0, sumY = 0;

            // Apply Sobel kernel
            for (int j = -1; j <= 1; ++j) {
                for (int i = -1; i <= 1; ++i) {
                    Uint32 pixel = originalPixels[(y + j) * pitch + (x + i)];
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
                    int intensity = (r + g + b) / 3;

                    sumX += intensity * gx[j + 1][i + 1];
                    sumY += intensity * gy[j + 1][i + 1];
                }
            }

            int magnitude = (int)sqrt(sumX * sumX + sumY * sumY);
            magnitude = std::min(255, std::max(0, magnitude));

            // Blend edge detection with original color
            Uint32 originalPixel = originalPixels[y * pitch + x];
            Uint8 origR, origG, origB;
            SDL_GetRGB(originalPixel, surface->format, &origR, &origG, &origB);

            Uint8 finalR = (origR * 0.7 + magnitude * 0.3);
            Uint8 finalG = (origG * 0.7 + magnitude * 0.3);
            Uint8 finalB = (origB * 0.7 + magnitude * 0.3);

            pixels[y * pitch + x] = SDL_MapRGB(surface->format, finalR, finalG, finalB);
        }
    }

    SDL_UnlockSurface(surface);
    delete[] originalPixels;
}
// Overlay a texture to simulate watercolor paper
void overlayTexture(SDL_Surface* surface, const char* texturePath) {
    SDL_Surface* texture = SDL_LoadBMP(texturePath);
    if (!texture) {
        std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
        return;
    }

    // 转换纹理格式以匹配主图像
    SDL_Surface* convertedTexture = SDL_ConvertSurface(texture, surface->format, 0);
    SDL_FreeSurface(texture);
    texture = convertedTexture;

    Uint32* pixels = (Uint32*)surface->pixels;
    Uint32* texturePixels = (Uint32*)texture->pixels;

    int pitch = surface->pitch / 4;
    int texturePitch = texture->pitch / 4;

    float blendFactor = 0.2f; // 调整透明度混合值

    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            int textureX = x % texture->w;
            int textureY = y % texture->h;

            Uint32 texPixel = texturePixels[textureY * texturePitch + textureX];
            Uint32 srcPixel = pixels[y * pitch + x];

            Uint8 srcR, srcG, srcB, texR, texG, texB;
            SDL_GetRGB(srcPixel, surface->format, &srcR, &srcG, &srcB);
            SDL_GetRGB(texPixel, texture->format, &texR, &texG, &texB);

            Uint8 finalR = std::clamp(int(srcR * (1.0f - blendFactor) + texR * blendFactor), 0, 255);
            Uint8 finalG = std::clamp(int(srcG * (1.0f - blendFactor) + texG * blendFactor), 0, 255);
            Uint8 finalB = std::clamp(int(srcB * (1.0f - blendFactor) + texB * blendFactor), 0, 255);

            pixels[y * pitch + x] = SDL_MapRGB(surface->format, finalR, finalG, finalB);
        }
    }

    SDL_FreeSurface(texture);
}

bool fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

void saveOutput(SDL_Surface* surface, const std::string& outputDir, const std::string& baseFilename) {
    // 确保 output 目录存在
    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directory(outputDir);
    }

    // 初始文件路径
    std::string outputPath = outputDir + "/" + baseFilename;

    // 如果文件已存在，则寻找新的文件名
    if (fileExists(outputPath)) {
        int index = 1;
        std::string newFilename;
        do {
            std::ostringstream oss;
            oss << outputDir << "/" << baseFilename.substr(0, baseFilename.find_last_of('.'))
                << "_" << index << ".bmp";
            newFilename = oss.str();
            index++;
        } while (fileExists(newFilename));

        outputPath = newFilename;
    }

    // 保存 BMP 文件
    if (SDL_SaveBMP(surface, outputPath.c_str()) != 0) {
        std::cerr << "Failed to save output image: " << SDL_GetError() << std::endl;
    }
    else {
        std::cout << "Output image saved to " << outputPath << std::endl;
    }
}
// 遍历文件夹中的 BMP 文件并处理

int countFilesInFolder(const std::string& folderPath) {
    int fileCount = 0;

    try {
        std::cout << "Files in folder '" << folderPath << "':" << std::endl;
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                std::cout << "  - " << entry.path().filename().string() << std::endl;
                fileCount++;
            }
        }
        std::cout << "Total files: " << fileCount << std::endl;
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error accessing folder: " << e.what() << std::endl;
        return -1; // 发生错误时返回 -1
    }

    return fileCount;
}


int main(int argc, char* argv[]) {
    std::string folderPath = "frames";  // 你要处理的文件夹
    std::string outputFolder = "output"; // 处理后保存的文件夹

    // 获取文件夹中所有 BMP 文件
    std::vector<std::string> frames;
    int num = countFilesInFolder(folderPath);

    if (num > 0) {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".bmp") {
                frames.push_back(entry.path().string());
            }
        }
    }
    else {
        std::cerr << "No BMP files found in folder '" << folderPath << "'." << std::endl;
        return 1;
    }

    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) { 
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 遍历所有 BMP 文件
    for (const std::string& filePath : frames) {
        std::cout << "Processing: " << filePath << std::endl;

        // 加载图像
        SDL_Surface* image = SDL_LoadBMP(filePath.c_str());
        if (!image) {
            std::cerr << "Failed to load image: " << filePath << " SDL_Error: " << SDL_GetError() << std::endl;
            continue;
        }

        // 创建窗口（可选，仅用于显示）
        SDL_Window* window = SDL_CreateWindow("Watercolor Effect", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, image->w, image->h, SDL_WINDOW_SHOWN);
        if (!window) {
            SDL_FreeSurface(image);
            std::cerr << "Failed to create window for: " << filePath << std::endl;
            continue;
        }

        SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
        SDL_Surface* formattedImage = SDL_ConvertSurface(image, screenSurface->format, 0);
        SDL_FreeSurface(image);

        if (!formattedImage) {
            std::cerr << "Failed to convert image format: " << filePath << " SDL_Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            continue;
        }

        applyPosterization(formattedImage, 12);
        applyBoxBlur(formattedImage, 9);
        applyGaussianBlur(formattedImage, 5);
        applySobelFilter(formattedImage);
        overlayTexture(formattedImage, "texture.bmp");
        adjustBrightness(formattedImage, 1.5f);
        

        // 获取文件名并保存
        std::string filename = std::filesystem::path(filePath).filename().string();
        saveOutput(formattedImage, outputFolder, filename);

        // 更新窗口显示
        SDL_BlitSurface(formattedImage, nullptr, screenSurface, nullptr);
        SDL_UpdateWindowSurface(window);

        // 等待一段时间（防止窗口立即关闭）
        SDL_Delay(500);  // 可调整时间

        // 释放资源
        SDL_FreeSurface(formattedImage);
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
    return 0;
}

