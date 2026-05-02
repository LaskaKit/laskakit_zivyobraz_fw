#pragma once

#include <SensirionI2cSht4x.h>
#include <Adafruit_BME280.h>
#include <SparkFun_SCD4x_Arduino_Library.h>
#include <SensirionI2cStcc4.h>
#include <SensirionI2CSgp41.h>
#include <Wire.h>
#include <esp_log.h>

namespace {
    SensirionI2cSht4x sht4;
    Adafruit_BME280 bme;
    SCD4x SCD4(SCD4x_SENSOR_SCD41);
    SensirionI2cStcc4 stcc4;
    SensirionI2CSgp41 sgp41;
}

namespace LaskaKit::ZivyObraz {

    enum class Sensor : uint8_t {
        S_SHT4x  = 0,
        S_BME280 = 1,
        S_SCD4x  = 2,
        S_STCC4  = 3,
        S_SGP41  = 4,
        S_COUNT
    };

    // Separate type for the detected-set bitmask
    using SensorMask = uint8_t;

    constexpr SensorMask sensorBit(Sensor s) {
        return static_cast<SensorMask>(1u << static_cast<uint8_t>(s));
    }

    constexpr SensorMask MASK_NONE   = 0;
    constexpr SensorMask MASK_SHT4x  = sensorBit(Sensor::S_SHT4x);
    constexpr SensorMask MASK_BME280 = sensorBit(Sensor::S_BME280);
    constexpr SensorMask MASK_SCD4x  = sensorBit(Sensor::S_SCD4x);
    constexpr SensorMask MASK_STCC4  = sensorBit(Sensor::S_STCC4);
    constexpr SensorMask MASK_SGP41  = sensorBit(Sensor::S_SGP41);


    struct SensorEntry {
        Sensor      sensor;
        uint8_t     address;
    };

    const SensorEntry sensorAddresses[] = {
        {Sensor::S_SHT4x,  0x44},
        {Sensor::S_SHT4x,  0x45},
        {Sensor::S_SHT4x,  0x46},
        {Sensor::S_BME280, 0x76},
        {Sensor::S_BME280, 0x77},
        {Sensor::S_SCD4x,  0x62},
        {Sensor::S_STCC4,  0x64},
        {Sensor::S_STCC4,  0x65},
        {Sensor::S_SGP41,  0x59},
    };
    constexpr size_t SENSOR_COUNT = sizeof(sensorAddresses) / sizeof(sensorAddresses[0]);


    struct ScanResult {
        SensorMask mask = MASK_NONE;
        uint8_t addresses[static_cast<uint8_t>(Sensor::S_COUNT)] = {};
    };


    ScanResult scanI2CSensors(TwoWire &wire = Wire, uint32_t timeoutMs = 10)
    {
        ScanResult result;
        wire.setTimeOut(timeoutMs);

        for (size_t i = 0; i < SENSOR_COUNT; ++i)
        {
            const uint8_t idx = static_cast<uint8_t>(sensorAddresses[i].sensor);
            if (result.mask & sensorBit(sensorAddresses[i].sensor)) continue;   // already found this sensor type

            wire.beginTransmission(sensorAddresses[i].address);
            if (wire.endTransmission(true) == 0) {
                result.mask |= sensorBit(sensorAddresses[i].sensor);
                result.addresses[idx] = sensorAddresses[i].address;
            }
        }
        return result;
    }

    struct SensorReading
    {
        SensorMask mask = MASK_NONE;
        struct {
            double temperature = 0;
            double humidity = 0;
        } sht;

        struct {
            double temperature = 0;
            double humidity = 0;
            double pressure = 0;
        } bme;

        struct {
            double temperature = 0;
            double humidity = 0;
            double co2 = 0;
        } scd;

        struct {
            double temperature = 0;
            double humidity = 0;
            double co2 = 0;
        } stcc4;

        struct {
            uint16_t voc = 0;
            uint16_t nox = 0;
        } sgp41;

    };

    bool readSHT4x(SensorReading& reading, uint8_t address)
    {
        sht4.begin(Wire, address);

        float temp = 0.0f, hum = 0.0f;
        uint16_t err = sht4.measureLowestPrecision(temp, hum);
        if (err) {
            return false;
        }

        reading.sht.temperature = temp;
        reading.sht.humidity = hum;
        reading.mask |= MASK_SHT4x;
        return true;
    }

    bool readBME280(SensorReading& reading, uint8_t address)
    {
        if (!bme.begin(address)) {
            return false;
        }
        reading.bme.temperature = bme.readTemperature();
        reading.bme.humidity = bme.readHumidity();
        reading.bme.pressure = bme.readPressure() / 100.0;
        reading.mask |= MASK_BME280;
        return true;
    }

    bool readSCD4x(SensorReading& reading, uint8_t address)
    {
        if (!SCD4.begin(false, true, false)) {
            return false;
        }
        SCD4.measureSingleShot();
        while (!SCD4.readMeasurement()) {
            log_i("Waiting for SCD4 measurement...");
            delay(1000);
        }
        reading.scd.temperature = SCD4.getTemperature();
        reading.scd.humidity = SCD4.getHumidity();
        reading.scd.co2 = SCD4.getCO2();
        reading.mask |= MASK_SCD4x;
        return true;
    }

    // Single shot measurement
    bool readSTCC4(SensorReading& reading, uint8_t address)
    {
        stcc4.begin(Wire, address);
        delay(6);

        // Ensure sensor is in idle state
        uint16_t error = stcc4.exitSleepMode();
        if (error) { return false; }

        // Warmup to get rid of the 390 ppm start value
        error = stcc4.startContinuousMeasurement();
        if (error) { return false; }
        log_i("Waiting 30s to warmup STCC4");
        delay(30 * 1000); // TODO wait in light sleep
        log_i("STCC4 warmup complete");

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

        reading.mask |= MASK_STCC4;
        reading.stcc4.temperature = static_cast<double>(temperature);
        reading.stcc4.humidity = static_cast<double>(humidity);
        reading.stcc4.co2 = static_cast<double>(co2);

        log_i("STCC4 status: %d\n", status);
        return true;
    }

    bool readSGP41(SensorReading& reading, uint8_t address)
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
        reading.mask |= MASK_SGP41;
        return true;
    }

    SensorReading readSensors()
    {
        SensorReading reading;
        ScanResult scanResult = scanI2CSensors();

        if (scanResult.mask & MASK_SHT4x) {
            readSHT4x(reading, scanResult.addresses[static_cast<uint8_t>(Sensor::S_SHT4x)]);
        }
        if (scanResult.mask & MASK_BME280) {
            readBME280(reading, scanResult.addresses[static_cast<uint8_t>(Sensor::S_BME280)]);
        }
        if (scanResult.mask & MASK_SCD4x) {
            readSCD4x(reading, scanResult.addresses[static_cast<uint8_t>(Sensor::S_SCD4x)]);
        }
        if (scanResult.mask & MASK_STCC4) {
            readSTCC4(reading, scanResult.addresses[static_cast<uint8_t>(Sensor::S_STCC4)]);
        }
        if (scanResult.mask & MASK_SGP41) {
            readSGP41(reading, scanResult.addresses[static_cast<uint8_t>(Sensor::S_SGP41)]);
        }
        return reading;
    }

    void printSensors(SensorReading& reading)
    {
        log_i("Detected sensors:");
        if (reading.mask & MASK_SHT4x) {
            log_i("SHT4x -> T: %f H: %f", reading.sht.temperature, reading.sht.humidity);
        }
        if (reading.mask & MASK_BME280) {
            log_i("BME280 -> T: %f H: %f P: %f", reading.bme.temperature, reading.bme.humidity, reading.bme.pressure);
        }
        if (reading.mask & MASK_SCD4x) {
            log_i("SCD4x -> T: %f H: %f CO2: %f", reading.scd.temperature, reading.scd.humidity, reading.scd.co2);
        }
        if (reading.mask & MASK_STCC4) {
            log_i("STCC4 -> T: %f H: %f CO2: %f", reading.stcc4.temperature, reading.stcc4.humidity, reading.stcc4.co2);
        }
        if (reading.mask & MASK_SGP41) {
            log_i("SGP41 -> VOC: %d NOx: %d", reading.sgp41.voc, reading.sgp41.nox);
        }
    }
}
