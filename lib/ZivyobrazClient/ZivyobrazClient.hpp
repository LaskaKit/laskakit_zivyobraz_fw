#ifndef ZIVYOBRAZ_CLIENT_HPP
#define ZIVYOBRAZ_CLIENT_HPP

#include <HTTPClient.h>

namespace LaskaKit {

    enum class ContentType
    {
        ImageZ1,
        ImageZ2,
        ImageZ3,
        BMP,
        Unknown
    };

    void printContentType(ContentType ct)
    {
        switch (ct) {
            case ContentType::ImageZ1:
                Serial.println("image/z1");
                break;
            case ContentType::ImageZ2:
                Serial.println("image/z2");
                break;
            case ContentType::ImageZ3:
                Serial.println("image/z3");
                break;
            case ContentType::BMP:
                Serial.println("image/bmp");
                break;
            case ContentType::Unknown:
                Serial.println("unknown");
                break;
        }
    }

    ContentType ContentTypeHeaderStrToEnum(const String& header)
    {
        if (header == "image/z1")
            return ContentType::ImageZ1;
        if (header == "image/z2")
            return ContentType::ImageZ2;
        if (header == "image/z3")
            return ContentType::ImageZ3;
        if (header == "image/bmp")
            return ContentType::BMP;
        return ContentType::Unknown;
    }

    // interesting headers from zivyobraz
    struct ZivyobrazHeaders
    {
        const char* headerKeys[5] = {
            "Content-Type",
            "Timestamp",
            "Sleep",
            "Rotate",
            "Data-Length"
        }; 

        ContentType contentType = ContentType::Unknown;
        int timestamp = -1;
        int sleep = -1;
        int rotate = -1;
        int dataLength = -1;

        void prepareCollect(HTTPClient& httpClient)
        {
            httpClient.collectHeaders(headerKeys, 5);
        }

        void collect(HTTPClient& httpClient)
        {
            if (httpClient.hasHeader("Content-Type")) {
                this->contentType = ContentTypeHeaderStrToEnum(httpClient.header("Content-Type"));
            }
            if (httpClient.hasHeader("Timestamp")) {
                this->timestamp = httpClient.header("Timestamp").toInt();
            }
            if (httpClient.hasHeader("Sleep")) {
                this->sleep = httpClient.header("Sleep").toInt();
            }
            if (httpClient.hasHeader("Rotate")) {
                this->rotate =  httpClient.header("Rotate").toInt();
            }
            if (httpClient.hasHeader("Data-Length")) {
                this->dataLength = httpClient.header("Data-Length").toInt();
            }
        }

        void print()
        {
            Serial.println("HEADERS:");
            printContentType(this->contentType);
            Serial.println(this->timestamp);
            Serial.println(this->sleep);
            Serial.println(this->rotate);
            Serial.println(this->dataLength);
        }
    };


    class ZivyobrazUrlBuilder
    {
    private:
        const char* baseUrl;

        size_t n_params = 0;
        char param_keys[10][20];
        char param_vals[10][20];

    public:
        ZivyobrazUrlBuilder(const char* baseUrl)
            : baseUrl(baseUrl)
        {}
        
        bool addParam(const char* key, const char* val)
        {
            if (this->n_params >= 10) {
                return false;
            }
            strcpy(param_keys[this->n_params], key);
            strcpy(param_vals[this->n_params], val);
            this->n_params++;
            return true;
        }

        void build(char* url)
        {
            strcpy(url, this->baseUrl);
            for (int i = 0; i < this->n_params; i++) {
                if (i == 0) {
                    strcat(url, "?");
                } else {
                    strcat(url, "&");
                }
                strcat(url, this->param_keys[i]);
                strcat(url, "=");
                strcat(url, this->param_vals[i]);
            }
        }
    };


    class ZivyobrazClient
    {
    private:
        const char* url;
        HTTPClient httpClient;

    public:
        void getHeaders()
        {
            this->httpClient.begin(this->url);
        }

        int test(uint8_t* buf)
        {
            ZivyobrazUrlBuilder builder("http://cdn.zivyobraz.eu/index.php");
            builder.addParam("mac", "48:CA:43:18:A6:58");
            builder.addParam("x", "800");
            builder.addParam("y", "480");
            builder.addParam("c", "BW");
            builder.addParam("fw", "2.5");

            char url[100];
            builder.build(url);
            Serial.println(url);

            this->httpClient.begin(url);

            ZivyobrazHeaders headers;

            headers.prepareCollect(httpClient);
            int httpCode = this->httpClient.GET();
            Serial.println(httpCode);
            if (httpCode == -1) {
                return 0;
            }
            headers.collect(this->httpClient);
            headers.print();
      
            int totalRead = 0;
            // 100 kb
            

            unsigned long startMillis = millis();
            while (true) {
                int available = httpClient.getStream().available();
                if (available == 0) {
                    vTaskDelay(10);
                    // 5 s timeout
                    if (millis() - startMillis > 300) {
                        break;
                    }
                    continue;
                } else {
                    startMillis = millis();
                }
                
                if (totalRead + available > 100000) {
                    break;
                }

                httpClient.getStream().read(buf + totalRead, available);
                totalRead += available;
            }

            Serial.print("Total read bytes: ");
            Serial.println(totalRead);

            Serial.print("Time taken [ms]: ");
            Serial.println(millis() - startMillis);

            Serial.print("Free heap: ");
            Serial.println(ESP.getFreeHeap());

            Serial.print("Max alloc heap: ");
            Serial.println(ESP.getMaxAllocHeap());

            Serial.print("RSSI: ");
            Serial.println(WiFi.RSSI());
            return totalRead;
        }
    };
}


#endif  // ZIVYOBRAZ_CLIENT_HPP
