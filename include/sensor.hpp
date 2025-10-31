
#pragma once

#include <Adafruit_SHT4x.h>
#include <Adafruit_BME280.h>
#include <SparkFun_SCD4x_Arduino_Library.h>
#include <SensirionI2cStcc4.h>
#include <SensirionI2CSgp41.h>
#include <BH1750.h>

Adafruit_SHT4x sht4;
Adafruit_BME280 bme;
SCD4x SCD4(SCD4x_SENSOR_SCD41);
SensirionI2cStcc4 stcc4;
SensirionI2CSgp41 sgp41;
BH1750 bh1750;

namespace LaskaKit::ZivyObraz {
    enum Sensor
    {
        _NONE = 0,
        _SHT4x = 1,
        _BME280 = 1 << 1,
        _SCD4x = 1 << 2,
        _STCC4 = 1 << 3,
        _SGP41 = 1 << 4,
        _BH1750 = 1 << 5
    };

    struct SHT4xReading
    {
        double temperature = 0;
        double humidity = 0;
    };

    struct BME280Reading
    {
        double temperature = 0;
        double humidity = 0;
        double pressure = 0;
    };

    struct SCD4xReading
    {
        double temperature = 0;
        double humidity = 0;
        double co2 = 0;
    };

    struct STCC4Reading
    {
        double temperature = 0;
        double humidity = 0;
        double co2 = 0;
    };

    struct SGP41Reading
    {
        uint16_t voc;
        uint16_t nox;
    };

    struct BH1750Reading
    {
        double lux = 0;
    };

    struct SensorReading
    {
        int flag = Sensor::_NONE;
        SHT4xReading sht;
        BME280Reading bme;
        SCD4xReading scd;
        STCC4Reading stcc4;
        SGP41Reading sgp41;
        BH1750Reading bh1750;
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
        reading.sht.temperature = temp.temperature;
        reading.sht.humidity = hum.relative_humidity;
        reading.flag |= Sensor::_SHT4x;
        return true;
    }

    bool readBME280(SensorReading& reading)
    {
        if (!bme.begin()) {
            return false;
        }
        reading.bme.temperature = bme.readTemperature();
        reading.bme.humidity = bme.readHumidity();
        reading.bme.pressure = bme.readPressure() / 100.0;
        reading.flag |= Sensor::_BME280;
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
        reading.scd.temperature = SCD4.getTemperature();
        reading.scd.humidity = SCD4.getHumidity();
        reading.scd.co2 = SCD4.getCO2();
        reading.flag |= Sensor::_SCD4x;
        return true;
    }

    // Single shot measurement
    bool readSTCC4(SensorReading& reading)
    {
        stcc4.begin(Wire, STCC4_I2C_ADDR_64);
        delay(6);

        // Ensure sensor is in idle state
        uint16_t error = stcc4.exitSleepMode();
        if (error) { return false; }

        // Warmup to get rid of the 390 ppm start value
        error = stcc4.startContinuousMeasurement();
        if (error) { return false; }
        Serial.println("Waiting 30s to warmup STCC4");
        delay(30 * 1000);
        Serial.println("STCC4 warmup complete");

        error = stcc4.stopContinuousMeasurement();
        if (error) { return false; }

        error = stcc4.measureSingleShot();
        if (error) { return false; }

        int16_t co2 = 0;
        float temperature = 0;
        float humidity = 0;
        uint16_t status = 0;
        error = stcc4.readMeasurement(co2, temperature, humidity, status);
        if (error) { return false; }


        // Enter sleep mode
        error = stcc4.enterSleepMode();
        if (error) { return false; }

        reading.flag |= Sensor::_STCC4;
        reading.stcc4.temperature = static_cast<double>(temperature);
        reading.stcc4.humidity = static_cast<double>(humidity);
        reading.stcc4.co2 = static_cast<double>(co2);

        Serial.printf("STCC4 status: %d\n", status);
        return true;
    }

    bool readSGP41(SensorReading& reading)
    {
        sgp41.begin(Wire);

        uint16_t serialNumber[3];
        if (sgp41.getSerialNumber(serialNumber)) {
            return false;
        }

        uint16_t defaultRh = 0x8000;
        uint16_t defaultT = 0x6666;
        uint16_t srawVoc = 0;
        uint16_t srawNox = 0;
        for (int i = 0; i < 10; i++) {
            // NOx conditioning (10s)
            if (sgp41.executeConditioning(defaultRh, defaultT, srawVoc)) {
                return false;
            }
            delay(1000);
        }

        // Read Measurement
        if (sgp41.measureRawSignals(defaultRh, defaultT, srawVoc, srawNox)) {
            return false;
        }

        reading.sgp41.voc = srawVoc;
        reading.sgp41.nox = srawNox;
        reading.flag |= Sensor::_SGP41;
        return true;
    }

    bool readBH1750(SensorReading& reading)
    {
        if (!bh1750.begin()) {
            return false;
        }
        // wait for sensor to get ready
        delay(1000);
        float lux = bh1750.readLightLevel();
        reading.bh1750.lux = static_cast<double>(lux);
        reading.flag |= Sensor::_BH1750;
        return true;
    }


    SensorReading readSensors()
    {
        SensorReading reading;
        readSHT4x(reading);
        readBME280(reading);
        readSCD4x(reading);
        readSTCC4(reading);
        readSGP41(reading);
        readBH1750(reading);
        return reading;
    }

    void printSensors(SensorReading& reading)
    {
        Serial.println("Detected sensors:");
        if (reading.flag & Sensor::_SHT4x) {
            Serial.printf("SHT4x -> T: %f H: %f\n", reading.sht.temperature, reading.sht.humidity);
        }
        if (reading.flag & Sensor::_BME280) {
            Serial.printf("BME280 -> T: %f H: %f P: %f\n", reading.bme.temperature, reading.bme.humidity, reading.bme.pressure);
        }
        if (reading.flag & Sensor::_SCD4x) {
            Serial.printf("SCD4x -> T: %f H: %f CO2: %f\n", reading.scd.temperature, reading.scd.humidity, reading.scd.co2);
        }
        if (reading.flag & Sensor::_STCC4) {
            Serial.printf("STCC4 -> T: %f H: %f CO2: %f\n", reading.stcc4.temperature, reading.stcc4.humidity, reading.stcc4.co2);
        }
        if (reading.flag & Sensor::_SGP41) {
            Serial.printf("SGP41 -> VOC: %d NOx: %d\n", reading.sgp41.voc, reading.sgp41.nox);
        }
        if (reading.flag & Sensor::_BH1750) {
            Serial.printf("BH1750 -> LUX: %f\n", reading.bh1750.lux);
        }
        Serial.println();
    }
}

