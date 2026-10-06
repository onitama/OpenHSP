#include "dx11_renderer.h"
#include <cstring>
#include <wincodec.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

#define SAFE_RELEASE(p) do { if (p) { (p)->Release(); (p) = 0; } } while (0)

namespace {
    struct Constants { float transform[16]; };
    const char *kShader =
    "cbuffer C : register(b0) { row_major float4x4 transform; };"
    "struct I { float3 p:POSITION; float4 c:COLOR; float2 uv:TEXCOORD; };"
    "struct O { float4 p:SV_POSITION; float4 c:COLOR; float2 uv:TEXCOORD; };"
    "O VS(I i) { O o; o.p=mul(float4(i.p,1),transform); o.c=i.c; o.uv=i.uv; return o; }"
    "float4 PSC(O i):SV_TARGET { return i.c; }"
    "Texture2D t:register(t0); SamplerState s:register(s0);"
    "float4 PST(O i):SV_TARGET { float4 c=t.Sample(s,i.uv)*i.c; clip(c.a-0.0001); return c; }";
    // HLSL をコンパイルする。entry はエントリポイント、profile はシェーダーモデル、blob は生成バイトコードの受け取り先。
    bool Compile(const char *entry, const char *profile, ID3DBlob **blob) {
        ID3DBlob *errors = 0;
        HRESULT hr = D3DCompile(kShader, strlen(kShader), 0, 0, 0, entry, profile,
                                D3DCOMPILE_ENABLE_STRICTNESS, 0, blob, &errors);
        SAFE_RELEASE(errors); return SUCCEEDED(hr);
    }
}

// 描画層を初期化する。すべてのDirect3Dリソースを未生成状態にする。
HGIO_DX11_RENDERER::HGIO_DX11_RENDERER() : device_(0), context_(0), swapChain_(0), rtv_(0),
 backBuffer_(0), vs_(0), psColor_(0), psTexture_(0), layout_(0), vertexBuffer_(0),
 constantBuffer_(0), pointSampler_(0), linearSampler_(0), width_(0), height_(0) {
    memset(blend_, 0, sizeof(blend_));
}

// 描画層を破棄する。保持しているDirect3Dリソースを解放する。
HGIO_DX11_RENDERER::~HGIO_DX11_RENDERER() { Destroy(); }

// DX11デバイスとスワップチェーンを作成する。window は出力先、width/height は初期サイズ、fullscreen は全画面指定、vsync は互換用引数。
bool HGIO_DX11_RENDERER::Create(HWND window, int width, int height, bool fullscreen, bool vsync) {
    DXGI_SWAP_CHAIN_DESC sc = {};
    sc.BufferCount = 2;
    sc.BufferDesc.Width = width;
    sc.BufferDesc.Height = height;
    sc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sc.OutputWindow = window;
    sc.SampleDesc.Count = 1;
    sc.Windowed = fullscreen ? FALSE : TRUE;
    sc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
    D3D_FEATURE_LEVEL obtained;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, levels,
        ARRAYSIZE(levels), D3D11_SDK_VERSION, &sc, &swapChain_, &device_, &obtained, &context_);
    // Windows 7-era D3D11 runtimes reject a feature-level list containing 11.1.
    if (hr == E_INVALIDARG) hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags,
        levels + 1, ARRAYSIZE(levels) - 1, D3D11_SDK_VERSION, &sc, &swapChain_, &device_, &obtained, &context_);
    if (FAILED(hr)) return false;
    return CreateTargets(width, height) && CreatePipeline();
}

// バックバッファからレンダーターゲットとビューポートを作る。width/height は描画領域のピクセル数。
bool HGIO_DX11_RENDERER::CreateTargets(int width, int height) {
    ReleaseTargets(); width_ = width; height_ = height;
    if (FAILED(swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBuffer_))) return false;
    if (FAILED(device_->CreateRenderTargetView(backBuffer_, 0, &rtv_))) return false;
    context_->OMSetRenderTargets(1, &rtv_, 0);
    D3D11_VIEWPORT vp = { 0, 0, (FLOAT)width, (FLOAT)height, 0, 1 }; context_->RSSetViewports(1, &vp);
    return true;
}

// HGIOの頂点形式を描画するシェーダー、頂点バッファ、サンプラー、ブレンド状態を作成する。
bool HGIO_DX11_RENDERER::CreatePipeline()
{
    ID3DBlob *vs = 0, *ps = 0; if (!Compile("VS", "vs_4_0", &vs)) return false;
    if (FAILED(device_->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), 0, &vs_))) {
        SAFE_RELEASE(vs); return false;
    }
    D3D11_INPUT_ELEMENT_DESC e[] = {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_B8G8R8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,16,D3D11_INPUT_PER_VERTEX_DATA,0}
    };
    HRESULT hr=device_->CreateInputLayout(e, ARRAYSIZE(e), vs->GetBufferPointer(), vs->GetBufferSize(), &layout_);
    SAFE_RELEASE(vs); if (FAILED(hr)) return false;
    if (!Compile("PSC", "ps_4_0", &ps)) return false;
    hr=device_->CreatePixelShader(ps->GetBufferPointer(),ps->GetBufferSize(),0,&psColor_);
    SAFE_RELEASE(ps); if (FAILED(hr)) return false;
    if (!Compile("PST", "ps_4_0", &ps)) return false;
    hr=device_->CreatePixelShader(ps->GetBufferPointer(),ps->GetBufferSize(),0,&psTexture_);
    SAFE_RELEASE(ps); if (FAILED(hr)) return false;
    D3D11_BUFFER_DESC vb={};
    vb.ByteWidth=sizeof(HGIO_DX11_VERTEX)*128;
    vb.Usage=D3D11_USAGE_DYNAMIC;
    vb.BindFlags=D3D11_BIND_VERTEX_BUFFER;
    vb.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
    if (FAILED(device_->CreateBuffer(&vb,0,&vertexBuffer_))) return false;
    D3D11_BUFFER_DESC cb={};
    cb.ByteWidth=sizeof(Constants);
    cb.Usage=D3D11_USAGE_DEFAULT;
    cb.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
    if (FAILED(device_->CreateBuffer(&cb,0,&constantBuffer_))) return false;
    D3D11_SAMPLER_DESC sd={};
    sd.Filter=D3D11_FILTER_MIN_MAG_MIP_POINT;
    sd.AddressU=sd.AddressV=sd.AddressW=D3D11_TEXTURE_ADDRESS_WRAP;
    if(FAILED(device_->CreateSamplerState(&sd,&pointSampler_))) return false;
    sd.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    if(FAILED(device_->CreateSamplerState(&sd,&linearSampler_))) return false;
    const D3D11_BLEND src[] = {D3D11_BLEND_ONE,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ZERO,D3D11_BLEND_ZERO};
    const D3D11_BLEND dst[] = {D3D11_BLEND_ZERO,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_INV_SRC_COLOR,D3D11_BLEND_INV_SRC_ALPHA};
    for(int i=0;i<5;i++){
        D3D11_BLEND_DESC b={};
        b.RenderTarget[0].BlendEnable=(i!=0);
        b.RenderTarget[0].SrcBlend=src[i];
        b.RenderTarget[0].DestBlend=dst[i];
        b.RenderTarget[0].BlendOp=D3D11_BLEND_OP_ADD;
        b.RenderTarget[0].SrcBlendAlpha=D3D11_BLEND_ONE;
        b.RenderTarget[0].DestBlendAlpha=D3D11_BLEND_INV_SRC_ALPHA;
        b.RenderTarget[0].BlendOpAlpha=D3D11_BLEND_OP_ADD;
        b.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
        if(FAILED(device_->CreateBlendState(&b,&blend_[i]))) return false;
    }
    // 表裏に関係なく描画するラスタライザー設定。
    D3D11_RASTERIZER_DESC desc = {};
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_NONE;
    desc.DepthClipEnable = TRUE;

    ID3D11RasterizerState* state = nullptr;
    if (FAILED(device_->CreateRasterizerState(&desc, &state))) {
        return false;
    }
    context_->RSSetState(state);
    state->Release();  // 設定後はコンテキストが参照を保持する。

    return true;
}

// バックバッファとレンダーターゲットを解放する。デバイス本体は解放しない。
void HGIO_DX11_RENDERER::ReleaseTargets() {
    SAFE_RELEASE(rtv_);
    SAFE_RELEASE(backBuffer_);
}

// すべてのDX11リソースを解放する。再利用する場合はCreateを呼び出す。
void HGIO_DX11_RENDERER::Destroy() {
    if(context_) context_->ClearState();
    ReleaseTargets();
    for(int i=0;i<5;i++)SAFE_RELEASE(blend_[i]);
    SAFE_RELEASE(pointSampler_);
    SAFE_RELEASE(linearSampler_);
    SAFE_RELEASE(constantBuffer_);
    SAFE_RELEASE(vertexBuffer_);
    SAFE_RELEASE(layout_);
    SAFE_RELEASE(psTexture_);
    SAFE_RELEASE(psColor_);
    SAFE_RELEASE(vs_);
    SAFE_RELEASE(swapChain_);
    SAFE_RELEASE(context_);
    SAFE_RELEASE(device_);
}

// スワップチェーンをリサイズする。width/height は新しいクライアント領域のピクセル数。
bool HGIO_DX11_RENDERER::Resize(int width,int height) {
    if(!swapChain_ || !width || !height)return false;
    context_->OMSetRenderTargets(0,0,0);
    ReleaseTargets();
    return SUCCEEDED(swapChain_->ResizeBuffers(0,width,height,DXGI_FORMAT_UNKNOWN,0)) && CreateTargets(width,height);
}

// 左上原点の2D画面座標をクリップ空間へ変換する既定行列を設定する。
void HGIO_DX11_RENDERER::SetDefaultView() {
    memset(&view_, 0, sizeof(view_));
    view_.m00 = 2.0f / width_; view_.m11 = -2.0f / height_; view_.m22 = 1.0f; view_.m33 = 1.0f;
    view_.m30 = -1.0f; view_.m31 = 1.0f;
}

// 描画用の変換行列を設定する。matrix はgeometry.hの行ベクトル形式の16要素行列。
void HGIO_DX11_RENDERER::SetViewMatrix(const MATRIX *matrix) {
    if (matrix) view_ = *matrix;
}

// 1フレームの描画を開始する。color はAARRGGBB、clear はバックバッファ消去の有無。
void HGIO_DX11_RENDERER::BeginFrame(unsigned int color,bool clear) {
    context_->OMSetRenderTargets(1,&rtv_,0);
    SetDefaultView();
    if(clear){
        float c[4]={(color&255)/255.f,((color>>8)&255)/255.f,((color>>16)&255)/255.f,((color>>24)&255)/255.f};
        context_->ClearRenderTargetView(rtv_,c);
    }
}

// 1フレームを終了して画面へ表示する。vsync がtrueなら垂直同期を待つ。
HRESULT HGIO_DX11_RENDERER::EndFrame(bool vsync) {
    ID3D11ShaderResourceView *none=0;
    context_->PSSetShaderResources(0,1,&none);
    return swapChain_->Present(vsync?1:0,0);
}

// ブレンド方法を設定する。m はHGIO_DX11_BLEND、enabled はブレンドの有効化指定。
void HGIO_DX11_RENDERER::SetBlend(HGIO_DX11_BLEND m,bool enabled) {
    float f[4]={};
    context_->OMSetBlendState(enabled?blend_[m]:blend_[0],f,0xffffffff);
}

// テクスチャフィルターを設定する。linear がtrueなら線形補間、falseならポイントサンプリング。
void HGIO_DX11_RENDERER::SetLinearFilter(bool linear) {
    ID3D11SamplerState *s=linear?linearSampler_:pointSampler_;
    context_->PSSetSamplers(0,1,&s);
}

// ピクセルシェーダーにテクスチャを設定する。t にnullを渡すとテクスチャを解除する。
void HGIO_DX11_RENDERER::SetTexture(ID3D11ShaderResourceView *t) {
    context_->PSSetShaderResources(0,1,&t);
}

// 頂点を即時描画する。t はプリミティブ種別、v は頂点配列、n は頂点数、textured はテクスチャシェーダーの選択。
void HGIO_DX11_RENDERER::Draw(HGIO_DX11_TOPOLOGY t,const HGIO_DX11_VERTEX *v,unsigned int n,bool textured) {
    if (!n || n > 128) return;
    HGIO_DX11_VERTEX expanded[128];
    unsigned int out = n;
    D3D11_PRIMITIVE_TOPOLOGY native = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    if (t == HGIO_TOPOLOGY_TRIANGLE_FAN) {
        if (n < 3 || 3 * (n - 2) > ARRAYSIZE(expanded)) return;
        out = 0;
        for (unsigned int i = 1; i + 1 < n; ++i) {
            expanded[out++] = v[0]; expanded[out++] = v[i]; expanded[out++] = v[i + 1];
        }
        v = expanded; native = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
    D3D11_MAPPED_SUBRESOURCE m;
    if (FAILED(context_->Map(vertexBuffer_,0,D3D11_MAP_WRITE_DISCARD,0,&m))) return;
    memcpy(m.pData,v,sizeof(*v)*out); context_->Unmap(vertexBuffer_,0);
    UINT stride=sizeof(*v),offset=0; Constants c; memcpy(c.transform, &view_, sizeof(c.transform));
    context_->UpdateSubresource(constantBuffer_,0,0,&c,0,0);
    context_->IASetInputLayout(layout_); context_->IASetVertexBuffers(0,1,&vertexBuffer_,&stride,&offset);
    context_->IASetPrimitiveTopology(native); context_->VSSetShader(vs_,0,0);
    context_->VSSetConstantBuffers(0,1,&constantBuffer_); context_->PSSetShader(textured?psTexture_:psColor_,0,0);
    context_->Draw(out,0);
}

// GPUテクスチャを作成する。r は受け取り先、w/h はサイズ、f は形式、p/pitch は任意の初期画素と1行のバイト数。
bool HGIO_DX11_RENDERER::CreateTexture(HGIO_DX11_TEXTURE *r,int w,int h,DXGI_FORMAT f,const void *p,unsigned int pitch) {
    // r must either be zero-initialized or have been returned by this class.
    if (!r || w <= 0 || h <= 0) return false;
    DeleteTexture(r);
    D3D11_TEXTURE2D_DESC d={}; d.Width=w; d.Height=h; d.MipLevels=1; d.ArraySize=1; d.Format=f; d.SampleDesc.Count=1;
    d.Usage=D3D11_USAGE_DEFAULT; d.BindFlags=D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA s={}; s.pSysMem=p; s.SysMemPitch=pitch;
    if (FAILED(device_->CreateTexture2D(&d,p?&s:0,&r->texture))) return false;
    if (FAILED(device_->CreateShaderResourceView(r->texture,0,&r->srv))) {
        DeleteTexture(r); return false;
    }
    r->width=w; r->height=h; return true;
}

// テクスチャ画素を更新する。t は対象、p/pitch は入力画素、area は更新範囲（nullなら全体）。
bool HGIO_DX11_RENDERER::UpdateTexture(const HGIO_DX11_TEXTURE *t,const void *p,unsigned int pitch,const RECT *area) {

    if (!t || !t->texture || !p) return false;
    D3D11_BOX b={};
    if(area){
        if(area->left<0||area->top<0||area->right>t->width||area->bottom>t->height||area->left>=area->right||area->top>=area->bottom)return false;
        b.left=area->left;b.top=area->top;b.right=area->right;b.bottom=area->bottom;b.back=1;
        context_->UpdateSubresource(t->texture,0,&b,p,pitch,0);
    } else{
        context_->UpdateSubresource(t->texture, 0, 0, p, pitch, 0);
    }
    return true;
}

// テクスチャを解放する。t はCreateTexture系で初期化された構造体。
void HGIO_DX11_RENDERER::DeleteTexture(HGIO_DX11_TEXTURE *t) {
    if(!t)return; SAFE_RELEASE(t->srv);
    SAFE_RELEASE(t->texture); t->width=t->height=0;
}

// バックバッファをBGRA8で読み出す。x/y/w/h は範囲、dst は出力先、rowPitch はdstの1行バイト数。
bool HGIO_DX11_RENDERER::ReadBack(int x,int y,int w,int h,void *dst,unsigned int rowPitch) {
    if(!backBuffer_||!dst||x<0||y<0||w<=0||h<=0||x+w>width_||y+h>height_||rowPitch<(unsigned)w*4) return false;
    D3D11_TEXTURE2D_DESC d={};
    backBuffer_->GetDesc(&d);
    d.Width=w;d.Height=h;d.BindFlags=0;d.MiscFlags=0;d.Usage=D3D11_USAGE_STAGING;d.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
    ID3D11Texture2D *staging=0;
    if(FAILED(device_->CreateTexture2D(&d,0,&staging))) return false;
    D3D11_BOX box={(UINT)x,(UINT)y,0,(UINT)(x+w),(UINT)(y+h),1};
    context_->CopySubresourceRegion(staging,0,0,0,0,backBuffer_,0,&box);
    D3D11_MAPPED_SUBRESOURCE m; HRESULT hr=context_->Map(staging,0,D3D11_MAP_READ,0,&m);
    if(SUCCEEDED(hr)){
        for(int row=0;row<h;row++)memcpy((char*)dst+row*rowPitch,(char*)m.pData+row*m.RowPitch,w*4);context_->Unmap(staging,0);
    }
    SAFE_RELEASE(staging);return SUCCEEDED(hr);
}

// メモリ上の画像をWICでデコードしてテクスチャ化する。r は受け取り先、data/size は画像バイト列。
bool HGIO_DX11_RENDERER::CreateTextureFromMemory(HGIO_DX11_TEXTURE *r,const void *data,size_t size) {
    if(!r||!data||!size)return false;
    IWICImagingFactory *f=0;
    IWICStream *stream=0;
    IWICBitmapDecoder *dec=0;
    IWICBitmapFrameDecode *frame=0;
    IWICFormatConverter *cv=0;
    bool ok=false;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory,0,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&f)))) goto done;
    if (FAILED(f->CreateStream(&stream))||FAILED(stream->InitializeFromMemory((WICInProcPointer)data,(DWORD)size))||FAILED(f->CreateDecoderFromStream(stream,0,WICDecodeMetadataCacheOnLoad,&dec))||FAILED(dec->GetFrame(0,&frame))||FAILED(f->CreateFormatConverter(&cv))) goto done;
    UINT w,h;
    if(FAILED(frame->GetSize(&w,&h))||FAILED(cv->Initialize(frame,GUID_WICPixelFormat32bppPBGRA,WICBitmapDitherTypeNone,0,0,WICBitmapPaletteTypeCustom))) goto done;
    {
        unsigned int pitch=w*4, bytes=pitch*h;
        unsigned char *pixels=new unsigned char[bytes];
        if(SUCCEEDED(cv->CopyPixels(0,pitch,bytes,pixels))){ok=CreateTexture(r,(int)w,(int)h,DXGI_FORMAT_B8G8R8A8_UNORM,pixels,pitch);} delete[] pixels;
    }
done:
    SAFE_RELEASE(cv);SAFE_RELEASE(frame);SAFE_RELEASE(dec);SAFE_RELEASE(stream);SAFE_RELEASE(f);
    return ok;
}


void HGIO_DX11_RENDERER::Test(void)
{
    RECT client = {};
    const int width = 200;
    const int height = 100;

    this->BeginFrame(0xff202838, true);
    this->SetBlend(HGIO_BLEND_COPY, false);

    // Yellow diagonal line.
    const HGIO_DX11_VERTEX line[] = {
        { 40.0f, 40.0f, 0.0f, 0xffffff00, 0.0f, 0.0f },
        { static_cast<float>(width - 40), static_cast<float>(height - 40), 0.0f, 0xffffff00, 0.0f, 0.0f },
    };
    this->Draw(HGIO_TOPOLOGY_LINE_LIST, line, ARRAYSIZE(line), false);

    // Opaque blue rectangle.
    const HGIO_DX11_VERTEX blueRect[] = {
        { 80.0f, 150.0f, 0.0f, 0xff3c8dff, 0.0f, 0.0f },
        { 280.0f, 150.0f, 0.0f, 0xff3c8dff, 0.0f, 0.0f },
        { 280.0f, 330.0f, 0.0f, 0xff3c8dff, 0.0f, 0.0f },
        { 80.0f, 330.0f, 0.0f, 0xff3c8dff, 0.0f, 0.0f },
    };
    this->Draw(HGIO_TOPOLOGY_TRIANGLE_FAN, blueRect, ARRAYSIZE(blueRect), false);

    // Half-transparent red rectangle using the renderer's alpha blend state.
    this->SetBlend(HGIO_BLEND_ALPHA, true);
    const HGIO_DX11_VERTEX redRect[] = {
        { 210.0f, 210.0f, 0.0f, 0x80ff4b4b, 0.0f, 0.0f },
        { 450.0f, 210.0f, 0.0f, 0x80ff4b4b, 0.0f, 0.0f },
        { 450.0f, 390.0f, 0.0f, 0x80ff4b4b, 0.0f, 0.0f },
        { 210.0f, 390.0f, 0.0f, 0x80ff4b4b, 0.0f, 0.0f },
    };
    this->Draw(HGIO_TOPOLOGY_TRIANGLE_FAN, redRect, ARRAYSIZE(redRect), false);
}



