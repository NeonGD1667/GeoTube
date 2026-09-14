#pragma once

#include <string>

namespace geotube::webview {

    void create();
    void destroy();

    void loadURL(const std::string& url);
    void executeJS(const std::string& js);

    void setVisible(bool visible);
    bool isVisible();

}

