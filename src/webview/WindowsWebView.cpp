/// thằng này code ngu vãi
#ifdef _WIN32

#include "WebView.hpp"

#include <windows.h>
#include <wrl.h>
#include <WebView2.h>

#include <string>

using Microsoft::WRL::ComPtr;

namespace geotube::webview {

    static HWND s_window = nullptr;
    static ComPtr<ICoreWebView2Environment> s_environment;
    static ComPtr<ICoreWebView2Controller> s_controller;
    static ComPtr<ICoreWebView2> s_webview;

    static bool s_visible = false;

    void create() {
        if (s_webview)
            return;

        s_window = GetActiveWindow();

        if (!s_window)
            return;

        CreateCoreWebView2EnvironmentWithOptions(
            nullptr,
            nullptr,
            nullptr,
            Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
                [](HRESULT result, ICoreWebView2Environment* environment) -> HRESULT {
                    if (FAILED(result) || !environment)
                        return result;

                    s_environment = environment;

                    environment->CreateCoreWebView2Controller(
                        s_window,
                        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                            [](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                                if (FAILED(result) || !controller)
                                    return result;

                                s_controller = controller;

                                controller->get_CoreWebView2(
                                    &s_webview
                                );

                                if (!s_webview)
                                    return E_FAIL;

                                s_controller->put_IsVisible(FALSE);

                                return S_OK;
                            }
                        ).Get()
                    );

                    return S_OK;
                }
            ).Get()
        );
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

        s_webview->Navigate(wideURL.c_str());
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
            s_controller->put_IsVisible(visible ? TRUE : FALSE);
    }

    bool isVisible() {
        return s_visible;
    }

}

#endif

