#include <WiFi.h>
#include <displays.hpp>


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
        String mac_suffix = WiFi.macAddress().substring(15);  // the last 2 chars
        return String("LIVE_") + String(DISPLAY_T::NAME) + "_" + mac_suffix;
    }
};
