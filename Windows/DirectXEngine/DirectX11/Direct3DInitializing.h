#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
//#include <xnamath.h>



IDXGISwapChain              *g_SwapChain;             //用于交换前后台缓冲区
ID3D11Device                *g_d3d11Device;           //调用不需要渲染的函数
ID3D11DeviceContext         *g_d3d11DevCon;           //调用需要渲染的函数
ID3D11RenderTargetView      *g_renderTargetView;      //渲染目标视图

float g_red = 0.0f;
float g_green = 0.0f;
float g_blue = 0.0f;
int g_colormodr = 1;
int g_colormodg = 1;
int g_colormodb = 1;


HWND g_hwnd = NULL;

const int g_Width = 300;
const int g_Height = 300;



bool InitializeDirect3d11App(HINSTANCE hInstance)
{
    //Describe our Buffer
    DXGI_MODE_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

    bufferDesc.Width = g_Width;
    bufferDesc.Height = g_Height;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //Describe our g_SwapChain
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = g_hwnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    //Create our g_SwapChain
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
        D3D11_SDK_VERSION, &swapChainDesc, &g_SwapChain, &g_d3d11Device, NULL, &g_d3d11DevCon);

    //Create our BackBuffer
    ID3D11Texture2D* BackBuffer;
    g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

    //Create our Render Target
    g_d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &g_renderTargetView);
    BackBuffer->Release();

    //Set our Render Target
    g_d3d11DevCon->OMSetRenderTargets(1, &g_renderTargetView, NULL);

    return true;
}

void ReleaseObjects()
{
    g_SwapChain->Release();
    g_d3d11Device->Release();
    g_d3d11DevCon->Release();
}

bool InitScene()
{
    return true;
}

void UpdateScene()
{
    //Update the colors of our scene
    g_red += g_colormodr * 0.00005f;
    g_green += g_colormodg * 0.00002f;
    g_blue += g_colormodb * 0.00001f;

    if (g_red >= 1.0f || g_red <= 0.0f)
        g_colormodr *= -1;
    if (g_green >= 1.0f || g_green <= 0.0f)
        g_colormodg *= -1;
    if (g_blue >= 1.0f || g_blue <= 0.0f)
        g_colormodb *= -1;
}

void DrawScene()
{
    //Clear our backbuffer to the updated color
    D3DXCOLOR bgColor(g_red, g_green, g_blue, 1.0f);

    g_d3d11DevCon->ClearRenderTargetView(g_renderTargetView, bgColor);

    //Present the backbuffer to the screen
    g_SwapChain->Present(0, 0);
}
