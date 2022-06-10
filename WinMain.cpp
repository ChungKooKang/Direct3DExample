#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <wrl/client.h>
#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

const wchar_t gClassName[]{ L"MyWindowClass" };
const wchar_t gTitle[]{ L"Direct3D" };
const int WINDOW_WIDTH{ 800 };
const int WINDOW_HEIGHT{ 600 };

Microsoft::WRL::ComPtr<ID3D11Device>             gspDevice;
Microsoft::WRL::ComPtr<ID3D11DeviceContext>      gspDeviceContext{};
Microsoft::WRL::ComPtr<IDXGISwapChain>           gspSwapChain{};

Microsoft::WRL::ComPtr<ID3D11Texture2D>          gspRenderTarget{};
Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   gspRenderTargetView{};
Microsoft::WRL::ComPtr<ID3D11Texture2D>          gspDepthStencil{};
Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   gspDepthStencilView{};

HWND gHwnd{};
HINSTANCE gInstance{};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void InitD3D();
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
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpfnWndProc = WindowProc;
    wc.cbSize = sizeof(WNDCLASSEX);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(nullptr, L"Failed to register window class!", L"Error",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    RECT wr{ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
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
    //TODO : 처음 처리
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
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
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
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // BackBuffer는 RenderTarget으로 지정할 목적
    scd.OutputWindow = gHwnd;                               // 그림을 그릴 윈도우
    scd.SampleDesc.Count = 1;                               // 1xMSAA - anti aliasing 안함.
    scd.Windowed = TRUE;                                    // 창모드

    
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

    // 스웹체인(Front, Back) <- Back에 그릴 예정 <- RenderTarget을 Backbuffer로 지정
    gspSwapChain->GetBuffer(0, IID_PPV_ARGS(gspRenderTarget.ReleaseAndGetAddressOf()));

    // resource -> resource view로 연결
    gspDevice->CreateRenderTargetView(gspRenderTarget.Get(), nullptr, gspRenderTargetView.GetAddressOf());
    
}

void DestroyD3D()
{
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