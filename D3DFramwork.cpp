#include <sstream>
#include "D3DFramwork.h"

void D3DFramwork::CalculateFPS()
{
    static int frameCount{};
    static float timeElapsed{};

    frameCount++;
    if (mTimer.Totaltime() - timeElapsed >= 1.0f)
    {
        float fps = static_cast<float>(frameCount);
        float mspf = 1000.0f / fps;

        std::wostringstream oss;

        oss.precision(6);
        oss << mTitleText << L"-"
            << L"FPS : " << fps <<
            L", Frame Time : " << mspf << L"(ms)";

        SetWindowText(mHwnd, oss.str().c_str());

        frameCount = 0;
        timeElapsed += 1.0f;

    }
}

void D3DFramwork::InitWindow(HINSTANCE hInstance)
{
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    mInstance = hInstance;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = CLASSNAME.c_str();
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbSize = sizeof(WNDCLASSEX);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(nullptr, L"Failed to register window class!", L"Error",
            MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    RECT wr{ 0, 0, mScreenWidth, mScreenHeight };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    mTitleText = TITLENAME;

    mHwnd = CreateWindowEx(NULL,
        CLASSNAME.c_str(),
        mTitleText.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (mHwnd == nullptr)
    {
        MessageBox(nullptr, L"Failed to create window class!", L"Error",
            MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    SetWindowLongPtr(mHwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(mHwnd, SW_SHOW);
    SetForegroundWindow(mHwnd);
    SetFocus(mHwnd);
    UpdateWindow(mHwnd);
}

void D3DFramwork::InitD3D()
{
     DXGI_SWAP_CHAIN_DESC scd;
    
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
    scd.BufferCount = 1;                                    // BackBuffer�� ����
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // PixelFormat
    scd.BufferDesc.Width = mScreenWidth;
    scd.BufferDesc.Height = mScreenHeight;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // BackBuffer�� RenderTarget���� ������ ����
    scd.OutputWindow = mHwnd;                               // �׸��� �׸� ������
    scd.SampleDesc.Count = 1;                               // 1xMSAA - anti aliasing ����.
    scd.Windowed = TRUE;                                    // â���
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    
    D3D11CreateDeviceAndSwapChain(
        NULL,                                     // Adapter�� pointer
        D3D_DRIVER_TYPE_HARDWARE,                 // �������� �ϵ���� (�׷��� ī��) �� ���ٴ� ��.
        NULL,                                     // ����Ʈ���� ������ ����
        NULL,                                     // Flag, �ɼ��� �ִ� ��.
        NULL,                                     // D3D_FEATURE_LEVEL �迭. �ּһ��
        0,                                        // �迭�� ũ��
        D3D11_SDK_VERSION,                        // SDK : library + �� �� ������. �� ����
        &scd,                                     // DXGI_SWAP_CHAIN_DESC ����ü�� ���� pointer
        mspSwapChain.ReleaseAndGetAddressOf(),    // ������ �ִ� ���� ������ �����ϰ� ������ ����ü�κθ�.
        mspDevice.ReleaseAndGetAddressOf(),       // ������ Device �������̽�
        NULL,                                     // �׷��� ī�尡 �����ϴ� DX ���� ���
        mspDeviceContext.ReleaseAndGetAddressOf() // ������ DeviceContext �������̽�
    );

    OnResize();
}

void D3DFramwork::OnResize()
{
    //SwapChain ũ�� ����

    //������ ���ؼ� ������ ��.
    ID3D11RenderTargetView* nullView[] = { nullptr };
    mspDeviceContext->OMSetRenderTargets(_countof(nullView), nullView, nullptr);

    mspRenderTargetView.Reset();
    mspDepthStencilView.Reset();
    mspRenderTarget.Reset();
    mspDepthStencil.Reset();

    mspDeviceContext->Flush();

    // ���� ������ ����
    mspSwapChain->ResizeBuffers(
        0,
        mScreenWidth,
        mScreenHeight,
        DXGI_FORMAT_UNKNOWN,
        0
    );


    // ����ü��(Front, Back) <- Back�� �׸� ���� <- RenderTarget�� Backbuffer�� ����
    mspSwapChain->GetBuffer(0, IID_PPV_ARGS(mspRenderTarget.ReleaseAndGetAddressOf()));

    // resource -> resource view�� ����
    mspDevice->CreateRenderTargetView(mspRenderTarget.Get(), nullptr, mspRenderTargetView.GetAddressOf());

    //Depth-Stencil Buffer
    CD3D11_TEXTURE2D_DESC t2d(
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        mScreenWidth,
        mScreenHeight,
        1,
        1, // 0�� �ָ� �⺻���� �Ǿ �� ����� ����. ��������
        D3D11_BIND_DEPTH_STENCIL
    );


    mspDevice->CreateTexture2D(&t2d, nullptr, mspDepthStencil.ReleaseAndGetAddressOf());

    CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(D3D11_DSV_DIMENSION_TEXTURE2D);
    mspDevice->CreateDepthStencilView(
        mspDepthStencil.Get(),
        &dsvd,
        mspDepthStencilView.ReleaseAndGetAddressOf()
    );

    // Output Merger �����ϱ�
    mspDeviceContext->OMSetRenderTargets(1, mspRenderTargetView.GetAddressOf(), mspDepthStencilView.Get());

    // Viewport (������ ����)
    CD3D11_VIEWPORT viewport(
        0.0f, 0.0f,
        static_cast<float>(mScreenWidth),
        static_cast<float>(mScreenHeight)
    );
    mspDeviceContext->RSSetViewports(1, &viewport);
}

void D3DFramwork::RenderFrame()
{

    float claer_color[4]{ 0.0f, 0.2f, 0.4f, 1.0f }; // rgba UNORM
    mspDeviceContext->ClearRenderTargetView(mspRenderTargetView.Get(), claer_color);
    mspDeviceContext->ClearDepthStencilView(
        mspDepthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0
    );

    // MY CODE �ۼ��ϴ� �ڸ�
    Render();

    mspSwapChain->Present(0, 0);
}

void D3DFramwork::Initialize(HINSTANCE hInstance, int width, int height)
{
    mScreenWidth = width;
    mScreenHeight = height;
    mPaused = false;

    InitWindow(hInstance);
    InitD3D();
}

void D3DFramwork::Destroy()
{

    mspSwapChain->SetFullscreenState(FALSE, nullptr);

    mspDepthStencilView.Reset();
    mspDepthStencil.Reset();
    mspRenderTargetView.Reset();
    mspRenderTarget.Reset();
    mspSwapChain.Reset();
    mspDeviceContext.Reset();
    mspDevice.Reset();

    DestroyWindow(mHwnd);
    UnregisterClass(CLASSNAME.c_str(), mInstance);
}

void D3DFramwork::GameLoop()
{
    MSG msg{};
    
    mTimer.Start();

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
            mTimer.Update();

            if (mPaused)
            {
                Sleep(100); // parameter�� ms�� �� ���� ���ٰ� ���� �������� ����.
            }
            else
            {
                CalculateFPS();

                // Game Loop
                Update(mTimer.DeltaTime());
                RenderFrame();
            }
            
        }
    }

}

LRESULT D3DFramwork::MessageHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE :
        if (LOWORD(lParam) == WA_INACTIVE)
        {
            mPaused = true;
            mTimer.Stop();
        }
        else
        {
            mPaused = false;
            mTimer.Resume();
        }
        break;

    case WM_PAINT:
        if (mResizing)
        {
            RenderFrame();
        }
        else
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_ENTERSIZEMOVE:
        mPaused = true;
        mTimer.Stop();
        mResizing = true;
        return 0;
        break;

    case WM_SIZE:
        mScreenWidth = LOWORD(lParam);
        mScreenHeight = HIWORD(lParam);

        if (mspDevice)
        {
            if (wParam == SIZE_MINIMIZED)
            {
                if (!mPaused)
                {
                    mTimer.Stop();
                }
                mPaused = true;
                mMinimized = true;
                mMaximized = false;
            }
            else if (wParam == SIZE_MAXIMIZED)
            {
                mTimer.Resume();
                mPaused = false;
                mMinimized = false;
                mMaximized = true;
                OnResize();
            }
            else if (wParam == SIZE_RESTORED)
            {
                if (mMinimized)
                {
                    mPaused = false;
                    mTimer.Resume();
                    mMinimized = false;
                    OnResize();
                }
                else if (mMaximized)
                {
                    mPaused = false;
                    mTimer.Resume();
                    mMaximized = false;
                    OnResize();
                }
                else if (mResizing)
                {
                    // ����ڰ� �巡�� ��
                }
                else
                {
                    mPaused = false;
                    mTimer.Resume();
                    OnResize();
                }
            }
            else
            {
                // ����ڰ� �巡�� ��

            }
        }
        return 0;
        break;

    case WM_EXITSIZEMOVE:
        mPaused = false;
        mTimer.Resume();
        mResizing = false;
        OnResize();
        return 0;
        break;

    case WM_GETMINMAXINFO:
        //�ִ� �ּ� ������ ����
        reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 640;
        reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 480;
        return 0;
        break;

    case WM_MENUCHAR:
        return MAKELRESULT(0, MNC_CLOSE);
        break;

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

LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    D3DFramwork* pFramework = reinterpret_cast<D3DFramwork*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    return pFramework->MessageHandle(hWnd, message, wParam, lParam);
}
