#pragma once

#include <Geode/Geode.hpp>

namespace geotube::ui {

class GeoTubePopup : public geode::Popup {
public:
    static GeoTubePopup* create();

protected:
    bool init();
};

}