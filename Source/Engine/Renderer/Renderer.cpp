#include "Renderer.h"
#include "Texture.h"

namespace neu {

    bool Renderer::Initialize() {
        // Initialize SDL video subsystem
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            LOG_ERROR("SDL_Init Error: {}", SDL_GetError());
            return false;
        }

        // Initialize SDL_ttf for TrueType font rendering
        if (!TTF_Init()) {
            LOG_ERROR("TTF_Init Error: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    void Renderer::Shutdown() {
        TTF_Quit();                         // Shutdown SDL_ttf
        SDL_GL_DestroyContext(m_context);   // Destroy the OpenGL context
        SDL_DestroyWindow(m_window);        // Destroy the window
        SDL_Quit();                         // Shutdown SDL
    }

    
    bool Renderer::CreateWindow(const std::string& name, int width, int height, bool fullscreen) {
        // Store the logical dimensions
        m_width = width;
        m_height = height;

        // Create the SDL window
        m_window = SDL_CreateWindow(name.c_str(), width, height, SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
        if (m_window == nullptr) {
            LOG_ERROR("SDL_CreateWindow Error: {}", SDL_GetError());
            SDL_Quit();
            return false;
        }

        // OpenGL
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetSwapInterval(1);

        m_context = SDL_GL_CreateContext(m_window);
        if (m_context == nullptr) {
            LOG_ERROR("SDL_CreateWindow Error: {}", SDL_GetError());
            SDL_Quit();
            return false;
        }
        gladLoadGL();

        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        return true;
    }

    void Renderer::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    }

    void Renderer::SetColor(float r, float g, float b, float a) {
        SDL_SetRenderDrawColorFloat(m_renderer, r, g, b, a);
    }

    void Renderer::DrawLine(float x1, float y1, float x2, float y2) {
        SDL_RenderLine(m_renderer, x1, y1, x2, y2);
    }

    void Renderer::DrawPoint(float x, float y) {
        SDL_RenderPoint(m_renderer, x, y);
    }

    void Renderer::DrawTexture(Texture& texture, float x, float y) {
        // Get the natural size of the texture
        vec2 size = texture.GetSize();

        // Set up destination rectangle at the specified position
        SDL_FRect destRect;
        destRect.x = x;
        destRect.y = y;
        destRect.w = size.x;
        destRect.h = size.y;

        // Render the entire texture (NULL source rect) to the destination rectangle
        SDL_RenderTexture(m_renderer, texture.m_texture, NULL, &destRect);
    }
        
    void Renderer::DrawTexture(Texture& texture, float x, float y, float angle, float scale, bool flipH)
    {
        vec2 size = texture.GetSize();

        // Calculate scaled dimensions
        SDL_FRect destRect;
        destRect.w = size.x * scale;
        destRect.h = size.y * scale;

        // Position the texture so (x, y) is at its center
        // Subtract half width/height to convert from center to top-left
        destRect.x = x - destRect.w * 0.5f;
        destRect.y = y - destRect.h * 0.5f;

        // Render with rotation and optional horizontal flip
        // NULL center point means rotate around the rectangle's center
        SDL_RenderTextureRotated(m_renderer, texture.m_texture, NULL, &destRect, angle, NULL, (flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
    }

    void Renderer::DrawTexture(Texture& texture, const rect& sourceRect, float x, float y, float angle, float scale, bool flipH) {
        // Convert our rect type to SDL_FRect for the source rectangle
        SDL_FRect srcRect;
        srcRect.x = sourceRect.x;
        srcRect.y = sourceRect.y;
        srcRect.w = sourceRect.w;
        srcRect.h = sourceRect.h;

        // Calculate scaled destination dimensions
        SDL_FRect destRect;
        destRect.w = srcRect.w * scale;
        destRect.h = srcRect.h * scale;

        // Position so (x, y) is at the center of the drawn region
        destRect.x = x - destRect.w * 0.5f;
        destRect.y = y - destRect.h * 0.5f;

        // Render the specified source region with rotation and optional flip
        SDL_RenderTextureRotated(m_renderer, texture.m_texture, &srcRect, &destRect, angle, NULL, (flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
    }

    /// <summary>
    /// Clears the entire render target with the current draw color.
    /// Call this at the beginning of each frame to clear the previous frame's contents.
    /// </summary>
    void Renderer::Clear() {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    /// <summary>
    /// Presents the rendered frame to the screen by swapping buffers.
    /// Call this at the end of each frame after all drawing operations are complete.
    /// Uses double buffering: renders to back buffer, then swaps to front buffer for display.
    /// </summary>
    void Renderer::Present() {
        SDL_GL_SwapWindow(m_window);
    }
}
