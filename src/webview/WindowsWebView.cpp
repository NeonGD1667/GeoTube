/// hiện tại chỉ là mẫu , sau này add sau
#ifdef _WIN32

#include "WebView.hpp"

#include <Windows.h>
#include <WebView2.h>
#include <wrl.h>

#include <string>

using Microsoft::WRL::Callback;
using Microsoft::WRL::ComPtr;

namespace geotube::webview {

    static HWND s_window = nullptr;

    static ComPtr<ICoreWebView2Environment> s_environment;
    static ComPtr<ICoreWebView2Controller> s_controller;
    static ComPtr<ICoreWebView2> s_webview;

    static bool s_visible = false;


    static HWND findGeometryDashWindow() {
        struct WindowData {
            DWORD processID;
            HWND window;
        };

        WindowData data {
            GetCurrentProcessId(),
            nullptr
        };

        EnumWindows(
            [](HWND hwnd, LPARAM lParam) -> BOOL {
                auto* data =
                    reinterpret_cast<WindowData*>(lParam);

                DWORD windowProcessID = 0;

                GetWindowThreadProcessId(
                    hwnd,
                    &windowProcessID
                );

                if (windowProcessID != data->processID)
                    return TRUE;

                if (!IsWindowVisible(hwnd))
                    return TRUE;

                if (GetWindow(hwnd, GW_OWNER) != nullptr)
                    return TRUE;

                data->window = hwnd;

                return FALSE;
            },
            reinterpret_cast<LPARAM>(&data)
        );

        return data.window;
    }


    static void updateBounds() {
        if (!s_window || !s_controller)
            return;

        RECT rect {};
        GetClientRect(s_window, &rect);

        s_controller->put_Bounds(rect);
    }


    void create() {
        if (s_webview)
            return;

        s_window = findGeometryDashWindow();

        if (!s_window)
            return;

        HRESULT result =
            CreateCoreWebView2EnvironmentWithOptions(
                nullptr,
                nullptr,
                nullptr,
                Callback<
                    ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
                >(
                    [](HRESULT result,
                       ICoreWebView2Environment* environment) -> HRESULT {

                        if (FAILED(result) || !environment)
                            return result;

                        s_environment = environment;

                        return s_environment->CreateCoreWebView2Controller(
                            s_window,
                            Callback<
                                ICoreWebView2CreateCoreWebView2ControllerCompletedHandler
                            >(
                                [](HRESULT result,
                                   ICoreWebView2Controller* controller) -> HRESULT {

                                    if (FAILED(result) || !controller)
                                        return result;

                                    s_controller = controller;

                                    HRESULT hr =
                                        s_controller->get_CoreWebView2(
                                            &s_webview
                                        );

                                    if (FAILED(hr) || !s_webview)
                                        return hr;

                                    updateBounds();

                                    s_controller->put_IsVisible(
                                        s_visible ? TRUE : FALSE
                                    );

                                    return S_OK;
                                }
                            ).Get()
                        );
                    }
                ).Get()
            );

        if (FAILED(result))
            return;
    }


    void destroy() {
        s_webview.Reset();
        s_controller.Reset();
        s_environment.Reset();

        s_window = nullptr;
        s_visible = false;
    }


    void loadURL(const std::string& url) {
        if (!s_webview)
            return;

        std::wstring wideURL(
            url.begin(),
            url.end()
        );

        s_webview->Navigate(
            wideURL.c_str()
        );
    }


    void executeJS(const std::string& js) {
        if (!s_webview)
            return;

        std::wstring wideJS(
            js.begin(),
            js.end()
        );

        s_webview->ExecuteScript(
            wideJS.c_str(),
            nullptr
        );
    }


    void setVisible(bool visible) {
        s_visible = visible;

        if (s_controller)
            s_controller->put_IsVisible(
                visible ? TRUE : FALSE
            );
    }


    bool isVisible() {
        return s_visible;
    }

}

#endif

