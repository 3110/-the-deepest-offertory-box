#include <Preferences.h>
#include <SPIFFS.h>
#include <esp_log.h>

#include "AtomEcho.hpp"
#include "DistanceTrigger.hpp"
#include "ToFUnit.hpp"

static constexpr bool FORMAT_SPIFFS_IF_FAILED = true;
static constexpr const char* SOUND_EFFECT_WAV = "/sound-effect.wav";

static const char* NVS_NAMESPACE = "deepest-box";    // Max 15 chars
static const char* NVS_KEY_THRESHOLD = "threshold";  // Max 15 chars

static constexpr AtomEcho::led_color_t LED_COLOR_OK{0, 128, 0};
static constexpr AtomEcho::led_color_t LED_COLOR_ERROR{128, 0, 0};
static constexpr AtomEcho::led_color_t LED_COLOR_CALIBRATION{0, 0, 128};
static constexpr AtomEcho::led_color_t LED_COLOR_OFF{0, 0, 0};
static constexpr AtomEcho::led_color_t LED_COLOR_ENABLED{0, 128, 0};
static constexpr AtomEcho::led_color_t LED_COLOR_DISABLED{0, 0, 0};

static constexpr uint8_t CALIBRATION_COUNT = 10;
static constexpr uint8_t MM_WINDOW_SIZE = 10;
static constexpr uint8_t VOLUME = 150;

#if defined(DISTRIBUTION_FIRMWARE)
extern const uint8_t SOUND_EFFECT_WAV_START[] asm(
    "_binary_data_sound_effect_wav_start");
extern const uint8_t SOUND_EFFECT_WAV_END[] asm(
    "_binary_data_sound_effect_wav_end");
static const size_t SOUND_EFFECT_WAV_SIZE =
    (SOUND_EFFECT_WAV_END - SOUND_EFFECT_WAV_START);

static const char* DIST_FIRM_TAG = "DistFirm";

bool restoreWav(FS& fs, const char* filename, const uint8_t* buf, size_t size) {
    if (filename == nullptr) {
        ESP_LOGE(DIST_FIRM_TAG, "filename is null");
        return false;
    }
    if (buf == nullptr || size == 0) {
        ESP_LOGE(DIST_FIRM_TAG, "Invalid Buffer");
        return false;
    }
    if (fs.exists(filename)) {
        ESP_LOGW(DIST_FIRM_TAG, "WAV file %s exists. Skipping...", filename);
        return false;
    }
    ESP_LOGI(DIST_FIRM_TAG, "Restoring WAV file %s from firmware...", filename);
    File file = fs.open(filename, FILE_WRITE);
    if (!file) {
        ESP_LOGE(DIST_FIRM_TAG, "Failed to open %s for restoring", filename);
        return false;
    }
    const size_t s = file.write(buf, size);
    if (s != size) {
        ESP_LOGE(DIST_FIRM_TAG,
                 "Failed to write WAV file %s: expected = %d actual = %d",
                 filename, size, s);
    }
    file.close();
    return s == size;
}
#endif

AtomEcho echo;
DistanceTrigger<distance_unit_t, 3> trigger(new ToFUnit(Wire, AtomEcho::SDA_PIN,
                                                        AtomEcho::SCL_PIN));
Preferences prefs;

inline void forever(void) {
    echo.showLED(LED_COLOR_ERROR);
    while (true) {
        delay(1);
    }
}

void calibrationCallback(uint8_t count) {
    echo.showLED(LED_COLOR_CALIBRATION);
    delay(500);
    echo.showLED(LED_COLOR_OFF);
    delay(500);
}

void setup(void) {
    if (SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED) == false) {
        ESP_LOGE("SPIFFS", "Failed to mount SPIFFS");
        forever();
    }
#if defined(DISTRIBUTION_FIRMWARE)
    restoreWav(SPIFFS, SOUND_EFFECT_WAV, SOUND_EFFECT_WAV_START,
               SOUND_EFFECT_WAV_SIZE);
#endif
    if (prefs.begin(NVS_NAMESPACE, false) == false) {
        ESP_LOGE("NVS", "Failed to initialize %s", NVS_NAMESPACE);
        forever();
    }
    distance_unit_t threshold = prefs.getUShort(NVS_KEY_THRESHOLD, 0);
    echo.begin();
    echo.setVolume(VOLUME);
    ESP_LOGI("Atom Echo", "Volume: %d", VOLUME);
    echo.update();
    if (echo.isPressed() || threshold == 0) {
        ESP_LOGI("Trigger", "Calibration started");
        threshold = trigger.calibrate(CALIBRATION_COUNT, calibrationCallback);
        if (threshold == 0) {
            ESP_LOGE("Trigger", "Calibration failed");
            prefs.end();
            forever();
        } else {
            prefs.putUShort(NVS_KEY_THRESHOLD, threshold);
            ESP_LOGI("Trigger", "Calibration finished");
            echo.showLED(LED_COLOR_CALIBRATION);
        }
    }
    prefs.end();

    if (trigger.begin(threshold) == false) {
        ESP_LOGE("Trigger", "Failed to initialize %s", trigger.getName());
        forever();
    }
    ESP_LOGI("Trigger", "Distance Threshold: %dmm", threshold);
    trigger.enable();
}

void loop(void) {
    echo.update();
    echo.showLED(trigger.isEnabled() ? LED_COLOR_ENABLED : LED_COLOR_DISABLED);
    if (echo.wasPressed()) {
        if (trigger.isEnabled()) {
            trigger.disable();
        } else {
            trigger.enable();
        }
    }
    if (trigger.isTriggered()) {
        if (echo.playWav(SPIFFS, SOUND_EFFECT_WAV) == false) {
            echo.showLED(LED_COLOR_ERROR);
            forever();
        }
    }
    delay(1);
}
