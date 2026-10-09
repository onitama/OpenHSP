#pragma once
// DX11 draw backend used by the retained HGIO public API.
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include "../geometry.h"

struct HGIO_DX11_VERTEX {
    float x, y, z;
    unsigned int color;                 // legacy D3DCOLOR: AARRGGBB
    float u, v;
};

enum HGIO_DX11_BLEND { HGIO_BLEND_COPY, HGIO_BLEND_ALPHA, HGIO_BLEND_ADD,
                       HGIO_BLEND_SUB, HGIO_BLEND_INVSRCALPHA };
enum HGIO_DX11_TOPOLOGY { HGIO_TOPOLOGY_LINE_LIST, HGIO_TOPOLOGY_TRIANGLE_FAN };

struct HGIO_DX11_TEXTURE {
    ID3D11Texture2D *texture;
    ID3D11ShaderResourceView *srv;
    int width, height;
};

class HGIO_DX11_RENDERER {
public:
    HGIO_DX11_RENDERER();
    ~HGIO_DX11_RENDERER();
    bool Create(HWND window, int width, int height, bool fullscreen, bool vsync);
    void Destroy();
    bool Resize(int width, int height);
    bool Resize(int width, int height, bool fullscr);
    // hgio_render_start / hgio_render_end correspond to these two calls.
    void BeginFrame(unsigned int clearColor, bool clear);
    HRESULT EndFrame(bool vsync);
    // Kept as short aliases for callers already using the first backend revision.
    void Begin(unsigned int clearColor, bool clear) { BeginFrame(clearColor, clear); }
    HRESULT End(bool vsync) { return EndFrame(vsync); }
    // MATRIX uses the row-vector layout defined in geometry.h.  It is copied
    // directly to a row_major HLSL matrix, with no D3DX transpose required.
    void SetViewMatrix(const MATRIX *matrix);
    void SetDefaultView();
    void SetBlend(HGIO_DX11_BLEND mode, bool enabled);
    void SetLinearFilter(bool linear);
    void SetTexture(ID3D11ShaderResourceView *texture);
    bool CreateTexture(HGIO_DX11_TEXTURE *result, int width, int height,
                       DXGI_FORMAT format, const void *pixels, unsigned int rowPitch);
    bool UpdateTexture(const HGIO_DX11_TEXTURE *texture, const void *pixels,
                       unsigned int rowPitch, const RECT *area = 0);
    bool CreateTextureFromMemory(HGIO_DX11_TEXTURE *result, const void *data, size_t size);
    void DeleteTexture(HGIO_DX11_TEXTURE *texture);
    // Copies a region of the current back buffer to BGRA8 rows.  destRowPitch
    // must be at least width * 4; source coordinates are clipped to the target.
    bool ReadBack(int x, int y, int width, int height, void *dest, unsigned int destRowPitch);
    void Draw(HGIO_DX11_TOPOLOGY topology, const HGIO_DX11_VERTEX *vertices,
              unsigned int count, bool textured);

    void Test(void);

    ID3D11Device *Device() const { return device_; }
    ID3D11DeviceContext *Context() const { return context_; }
    IDXGISwapChain *SwapChain() const { return swapChain_; }
    ID3D11Texture2D *BackBuffer() const { return backBuffer_; }

private:
    bool CreateTargets(int width, int height);
    bool CreatePipeline();
    void ReleaseTargets();
    ID3D11Device *device_;
    ID3D11DeviceContext *context_;
    IDXGISwapChain *swapChain_;
    ID3D11RenderTargetView *rtv_;
    ID3D11Texture2D *backBuffer_;
    ID3D11VertexShader *vs_;
    ID3D11PixelShader *psColor_;
    ID3D11PixelShader *psTexture_;
    ID3D11InputLayout *layout_;
    ID3D11Buffer *vertexBuffer_;
    ID3D11Buffer *constantBuffer_;
    ID3D11SamplerState *pointSampler_;
    ID3D11SamplerState *linearSampler_;
    ID3D11BlendState *blend_[5];
    int width_, height_;
    bool resizing_;
    MATRIX view_;
};
