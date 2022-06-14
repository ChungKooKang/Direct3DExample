#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <wrl/client.h>
#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

const wchar_t gClassName[]{ L"MyWindowClass" };
const wchar_t gTitle[]{ L"Direct3D" };
int gScreenWidth{ 800 };
int gScreenHeight{ 600 };

Microsoft::WRL::ComPtr<ID3D11Device>             gspDevice;
Microsoft::WRL::ComPtr<ID3D11DeviceContext>      gspDeviceContext{};
Microsoft::WRL::ComPtr<IDXGISwapChain>           gspSwapChain{};

Microsoft::WRL::ComPtr<ID3D11Texture2D>          gspRenderTarget{};
Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   gspRenderTargetView{};
Microsoft::WRL::ComPtr<ID3D11Texture2D>          gspDepthStencil{};
Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   gspDepthStencilView{};

HWND gHwnd{};
HINSTANCE gInstance{};
bool gMinimized{ false };
bool gMaximized{ false };
bool gResizing{ false };

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void InitD3D();
void OnResize();
void DestroyD3D();
void Render();


int WINAPI WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    gInstance = hInstance;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = gClassName;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbSize = sizeof(WNDCLASSEX);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(nullptr, L"Failed to register window class!", L"Error",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    RECT wr{ 0, 0, gScreenWidth, gScreenHeight };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    gHwnd = CreateWindowEx(NULL,
        gClassName,
        gTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (gHwnd == nullptr)
    {
        MessageBox(nullptr, L"Failed to create window class!", L"Error",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(gHwnd, nShowCmd);
    SetForegroundWindow(gHwnd);
    SetFocus(gHwnd);
    UpdateWindow(gHwnd);

    InitD3D();

    MSG msg{};
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                break;
            }
        }
        else
        {
            // Game Loop
            Render();
        }
    }

    DestroyD3D();

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
)
{
    switch (message)
    {
    case WM_PAINT :
        if (gResizing)
        {
            Render();
        }
        else
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

        case WM_ENTERSIZEMOVE :
            gResizing = true;
            return 0;
            break;

        case WM_SIZE :
            gScreenWidth = LOWORD(lParam);
            gScreenHeight = HIWORD(lParam);

            if (gspDevice)
            {
                if (wParam == SIZE_MINIMIZED)
                {
                    gMinimized = true;
                    gMaximized = false;
                }
                else if (wParam == SIZE_MAXIMIZED)
                {
                    gMinimized = false;
                    gMaximized = true;
                    OnResize();
                }
                else if (wParam == SIZE_RESTORED)
                {
                    if (gMinimized)
                    {
                        gMinimized = false;
                        OnResize();
                    }
                    else if (gMaximized)
                    {
                        gMaximized = false;
                        OnResize();
                    }
                    else if (gResizing)
                    {
                        // 사용자가 드래그 중
                    }
                    else
                    {
                        OnResize();
                    }
                }
                else
                {
                    // 사용자가 드래그 중

                }
            }
            return 0;
            break;

        case WM_EXITSIZEMOVE :
            gResizing = false;
            OnResize();
            return 0;
            break;

        case WM_GETMINMAXINFO :
            //최대 최소 사이즈 지정
            reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 640;
            reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 480;
            return 0;
            break;

        case WM_MENUCHAR :
            return MAKELRESULT(0, MNC_CLOSE);
            break;

        case WM_CLOSE :
            DestroyWindow(hWnd);
            break;

        case WM_DESTROY :
            PostQuitMessage(0);
            break;

        default :
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void InitD3D()
{
    DXGI_SWAP_CHAIN_DESC scd;
    
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
    scd.BufferCount = 1;                                    // BackBuffer의 갯수
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // PixelFormat
    scd.BufferDesc.Width = gScreenWidth;
    scd.BufferDesc.Height = gScreenHeight;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // BackBuffer는 RenderTarget으로 지정할 목적
    scd.OutputWindow = gHwnd;                               // 그림을 그릴 윈도우
    scd.SampleDesc.Count = 1;                               // 1xMSAA - anti aliasing 안함.
    scd.Windowed = TRUE;                                    // 창모드
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    
    D3D11CreateDeviceAndSwapChain(
        NULL,                                     // Adapter의 pointer
        D3D_DRIVER_TYPE_HARDWARE,                 // 물리적인 하드웨어 (그래픽 카드) 를 쓴다는 말.
        NULL,                                     // 소프트웨어 렌더러 지정
        NULL,                                     // Flag, 옵션을 주는 것.
        NULL,                                     // D3D_FEATURE_LEVEL 배열. 최소사양
        0,                                        // 배열의 크기
        D3D11_SDK_VERSION,                        // SDK : library + 각 종 도구들. 의 버젼
        &scd,                                     // DXGI_SWAP_CHAIN_DESC 구조체에 대한 pointer
        gspSwapChain.ReleaseAndGetAddressOf(),    // 기존에 있던 것이 있으면 해제하고 생성된 스왑체인부름.
        gspDevice.ReleaseAndGetAddressOf(),       // 생성된 Device 인터페이스
        NULL,                                     // 그래픽 카드가 지원하는 DX 버전 목록
        gspDeviceContext.ReleaseAndGetAddressOf() // 생성된 DeviceContext 인터페이스
    );

    OnResize();

}
void OnResize()
{
    //SwapChain 크기 변경

    //안전을 위해서 변경한 것.
    ID3D11RenderTargetView* nullView[] = { nullptr };
    gspDeviceContext->OMSetRenderTargets(_countof(nullView), nullView, nullptr);

    gspRenderTargetView.Reset();
    gspDepthStencilView.Reset();
    gspRenderTarget.Reset();
    gspDepthStencil.Reset();

    gspDeviceContext->Flush();

    // 실제 사이즈 변경
    gspSwapChain->ResizeBuffers(
        0,
        gScreenWidth,
        gScreenHeight,
        DXGI_FORMAT_UNKNOWN,
        0
    );


    // 스웹체인(Front, Back) <- Back에 그릴 예정 <- RenderTarget을 Backbuffer로 지정
    gspSwapChain->GetBuffer(0, IID_PPV_ARGS(gspRenderTarget.ReleaseAndGetAddressOf()));

    // resource -> resource view로 연결
    gspDevice->CreateRenderTargetView(gspRenderTarget.Get(), nullptr, gspRenderTargetView.GetAddressOf());

    //Depth-Stencil Buffer
    CD3D11_TEXTURE2D_DESC t2d(
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        gScreenWidth,
        gScreenHeight,
        1,
        1, // 0을 주면 기본값이 되어서 다 만들어 진다. 주의하자
        D3D11_BIND_DEPTH_STENCIL
    );


    gspDevice->CreateTexture2D(&t2d, nullptr, gspDepthStencil.ReleaseAndGetAddressOf());

    CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(D3D11_DSV_DIMENSION_TEXTURE2D);
    gspDevice->CreateDepthStencilView(
        gspDepthStencil.Get(),
        &dsvd,
        gspDepthStencilView.ReleaseAndGetAddressOf()
    );

    // Output Merger 연결하기
    gspDeviceContext->OMSetRenderTargets(1, gspRenderTargetView.GetAddressOf(), gspDepthStencilView.Get());

    // Viewport (보여질 영역)
    CD3D11_VIEWPORT viewport(
        0.0f, 0.0f,
        static_cast<float>(gScreenWidth),
        static_cast<float>(gScreenHeight)
    );
    gspDeviceContext->RSSetViewports(1, &viewport);
}

void DestroyD3D()
{
    gspSwapChain->SetFullscreenState(FALSE, nullptr);

    gspDepthStencilView.Reset();
    gspDepthStencil.Reset();
    gspRenderTargetView.Reset();
    gspRenderTarget.Reset();
    gspSwapChain.Reset();
    gspDeviceContext.Reset();
    gspDevice.Reset();

    DestroyWindow(gHwnd);
    UnregisterClass(gClassName, gInstance);
}

void Render()
{
    float claer_color[4]{ 0.0f, 0.2f, 0.4f, 1.0f }; // rgba UNORM
    gspDeviceContext->ClearRenderTargetView(gspRenderTargetView.Get(), claer_color );
    gspDeviceContext->ClearDepthStencilView(
        gspDepthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0
    );

    gspSwapChain->Present(0, 0);
}