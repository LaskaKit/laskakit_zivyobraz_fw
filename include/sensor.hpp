
#pragma once

#include <Adafruit_SHT4x.h>
#include <Adafruit_BME280.h>
#include <SparkFun_SCD4x_Arduino_Library.h>

Adafruit_SHT4x sht4;
Adafruit_BME280 bme;
SCD4x SCD4(SCD4x_SENSOR_SCD41);

namespace LaskaKit::ZivyObraz {
    enum SensorReadingFlag
    {
        NONE = 0,
        TEMPERATURE = 1,
        RELATIVE_HUMIDITY = 1 << 1,
        PRESSURE = 1 << 2,
        CO2 = 1 << 3
    };
    
    struct SensorReading
    {
        int flag = SensorReadingFlag::NONE;
        double temperature = 0;
        double relativeHumidity = 0;
        double pressure = 0;
        double co2 = 0;
    };

    bool readSHT4x(SensorReading& reading)
    {
        if (!sht4.begin()) {
            return false;
        }
        sht4.setPrecision(SHT4X_LOW_PRECISION);
        sht4.setHeater(SHT4X_NO_HEATER);
        sensors_event_t hum, temp;
        sht4.getEvent(&hum, &temp);
        reading.temperature = temp.temperature;
        reading.relativeHumidity = hum.relative_humidity;
        reading.flag = SensorReadingFlag::TEMPERATURE | SensorReadingFlag::RELATIVE_HUMIDITY;
        return true;
    }

    bool readBME280(SensorReading& reading)
    {
        if (!bme.begin()) {
            return false;
        }
        reading.temperature = bme.readTemperature();
        reading.relativeHumidity = bme.readHumidity();
        reading.pressure = bme.readPressure() / 100.0;
        reading.flag = SensorReadingFlag::TEMPERATURE | SensorReadingFlag::RELATIVE_HUMIDITY | SensorReadingFlag::PRESSURE;
        return true;
    }

    bool readSCD4x(SensorReading& reading)
    {
        if (!SCD4.begin(false, true, false)) {
            return false;
        }
        SCD4.measureSingleShot();
        while (!SCD4.readMeasurement()) {
            Serial.println("Waiting for SCD4 measurement...");
            delay(1000);
        }
        reading.temperature = SCD4.getTemperature();
        reading.relativeHumidity = SCD4.getHumidity();
        reading.co2 = SCD4.getCO2();
        reading.flag = SensorReadingFlag::TEMPERATURE | SensorReadingFlag::RELATIVE_HUMIDITY | SensorReadingFlag::CO2;
        return true;
    }


    SensorReading readSensors()
    {
        SensorReading reading;
        if (readSHT4x(reading)){
            Serial.println("Reading SHT4x.");
            return reading;
        }
        
        if (readBME280(reading)) {
            Serial.println("Reading BME280");
            return reading;
        }

        if (readSCD4x(reading)) {
            Serial.println("Reading SCD4x");
            return reading;
        }
        return reading;
    }
}

