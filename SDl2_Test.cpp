#include <SDL.h>
#include <iostream>
#include <cmath>
#include <algorithm> // for std::min and std::max
#include<vector>    
#include<SDL_image.h>

 //Apply a simple box blur to simulate watercolor bleeding
void applyBoxBlur(SDL_Surface* surface, int kernelSize = 3) {
    // Lock the surface for pixel access
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }

    Uint32* pixels = (Uint32*)surface->pixels;
    int pitch = surface->pitch / 4;

    // Create a copy of the original image for reference during blurring
    Uint32* originalPixels = new Uint32[surface->w * surface->h];
    std::copy(pixels, pixels + surface->w * surface->h, originalPixels);

    int halfKernel = kernelSize / 2;

    // Iterate over the image, skipping the edges
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

            // Compute the average color
            Uint8 avgR = sumR / count;
            Uint8 avgG = sumG / count;
            Uint8 avgB = sumB / count;

            // Set the blurred pixel
            pixels[y * pitch + x] = SDL_MapRGB(surface->format, avgR, avgG, avgB);
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

            // Blend edges with blurred colors
            Uint8 blended = (Uint8)((pixels[y * pitch + x] >> 16) * 0.7 + magnitude * 0.3);
           pixels[y * pitch + x] = SDL_MapRGB(surface->format, blended, blended, blended);
            //pixels[y * pitch + x] = SDL_MapRGB(surface->format, magnitude, magnitude, magnitude);
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

    Uint32* pixels = (Uint32*)surface->pixels;
    Uint32* texturePixels = (Uint32*)texture->pixels;

    int pitch = surface->pitch / 4;
    int texturePitch = texture->pitch / 4;

    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            // Map surface pixel to texture pixel
            int textureX = x % texture->w;
            int textureY = y % texture->h;

            Uint32 texPixel = texturePixels[textureY * texturePitch + textureX];
            Uint32 srcPixel = pixels[y * pitch + x];

            Uint8 srcR, srcG, srcB, texR, texG, texB;
            SDL_GetRGB(srcPixel, surface->format, &srcR, &srcG, &srcB);
            SDL_GetRGB(texPixel, texture->format, &texR, &texG, &texB);

            // Simple blending (adjust blending weights as needed)
            Uint8 finalR = (srcR * 1 + texR * 0.1);
            Uint8 finalG = (srcG * 1 + texG * 0.1);
            Uint8 finalB = (srcB * 1 + texB * 0.1);

            pixels[y * pitch + x] = SDL_MapRGB(surface->format, finalR, finalG, finalB);
        }
    }

    SDL_FreeSurface(texture);
}



int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Surface* image = SDL_LoadBMP("input.bmp");
    if (!image) {
        std::cerr << "Failed to load image: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Watercolor Effect", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, image->w, image->h, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_FreeSurface(image);
        SDL_Quit();
        return 1;
    }

    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
    SDL_Surface* formattedImage = SDL_ConvertSurface(image, screenSurface->format, 0);
    SDL_FreeSurface(image);

    applyBoxBlur(formattedImage,9);
  
    applySobelFilter(formattedImage);
    
    overlayTexture(formattedImage, "texture.bmp");

    SDL_BlitSurface(formattedImage, nullptr, screenSurface, nullptr);
    SDL_UpdateWindowSurface(window);

    // Wait for the user to close the window or press Esc
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }
        }
    }


    SDL_FreeSurface(formattedImage);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
