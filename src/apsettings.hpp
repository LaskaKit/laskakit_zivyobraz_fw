#include <displays.hpp>
#include <esp_mac.h>


struct APSettings
{
    String ssid;
    String password;

    void init() {
        ssid = make_ssid();
        password = "zivyobraz";
    }

    String getConnstr() {
        return "WIFI:S:" + ssid + ";T:WPA;P:" + password + ";";
    }

private:
    static String make_ssid() {
        uint8_t mac[6];
        esp_efuse_mac_get_default(mac);
        char buf[3];
        sprintf(buf, "%02X", mac[5]);
        String mac_suffix = String(buf);  // the last 2 chars
        return String("LIVE_") + String(DISPLAY_T::NAME) + "_" + mac_suffix;
    }
};
