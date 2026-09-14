/// tạm thời như này
#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include "webview/WebView.hpp"

#include <Geode/modify/MenuLayer.hpp>

$on_mod(Loaded) {
    log::info("GeoTube loaded!");

    if (Mod::get()->getSettingValue<bool>("auto-open")) {
        geotube::webview::create();
    }
}

class $modify(GeoTubeMenuLayer, MenuLayer) {

    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto myButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png"),
            this,
            menu_selector(GeoTubeMenuLayer::onGeoTube)
        );

        auto menu = this->getChildByID("bottom-menu");

        if (!menu) {
            log::error("GeoTube: bottom-menu not found!");
            return true;
        }

        menu->addChild(myButton);

        myButton->setID("geotube-button"_spr);

        menu->updateLayout();

        return true;
    }

    void onGeoTube(CCObject*) {
#ifdef _WIN32
        geotube::webview::create();
        geotube::webview::loadURL("https://www.youtube.com");
        geotube::webview::setVisible(true);
#else
        FLAlertLayer::create(
            "GeoTube",
            "GeoTube WebView is not available on this platform yet.",
            "OK"
        )->show();
#endif
    }
};

