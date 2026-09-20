#include "ui.hpp"

using namespace geode::prelude;

namespace geotube::ui {

GeoTubePopup* GeoTubePopup::create() {
    auto popup = new GeoTubePopup;

    if (popup->init()) {
        popup->autorelease();
        return popup;
    }

    delete popup;
    return nullptr;
}

bool GeoTubePopup::init() {
    if (!Popup::init(900.f, 600.f))
        return false;

    // GeoTube UI sẽ được thêm ở đây

    return true;
}

}