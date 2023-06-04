#include "AtomEcho.hpp"

// https://github.com/m5stack/M5Unified/blob/master/examples/Advanced/Speaker_SD_wav_file/Speaker_SD_wav_file.ino

static constexpr const size_t WAV_N_BUFS = 3;
static constexpr const size_t WAV_BUF_SIZE = 1024;
static uint8_t wav_data[WAV_N_BUFS][WAV_BUF_SIZE];

struct __attribute__((packed)) wav_header_t
{
    char RIFF[4];
    uint32_t chunk_size;
    char WAVEfmt[8];
    uint32_t fmt_chunk_size;
    uint16_t audiofmt;
    uint16_t channel;
    uint32_t sample_rate;
    uint32_t byte_per_sec;
    uint16_t block_size;
    uint16_t bit_per_sample;
};

struct __attribute__((packed)) sub_chunk_t
{
    char identifier[4];
    uint32_t chunk_size;
    uint8_t data[1];
};

AtomEcho::AtomEcho(void) : _brightness(255) {
}

AtomEcho::~AtomEcho(void) {
}

void AtomEcho::begin(void) {
    auto cfg = M5.config();
    cfg.internal_mic = true;
    cfg.internal_spk = true;
    M5.begin(cfg);
}

void AtomEcho::update(void) {
    M5.update();
}

bool AtomEcho::wasPressed(void) const {
    return M5.BtnA.wasPressed();
}

bool AtomEcho::isPressed(void) const {
    return M5.BtnA.isPressed();
}

void AtomEcho::setVolume(uint8_t v) {
    M5.Speaker.setVolume(v);
}

bool AtomEcho::playWav(FS& fs, const char* filename) {
    if (filename == nullptr || !fs.exists(filename)) {
        ESP_LOGE("AtomEcho", "WAV File is not found");
        return false;
    }
    ESP_LOGE("AtomEcho", "Playing WAV file: %s", filename);
    File file = fs.open(filename);

    wav_header_t header;
    file.read((uint8_t*)&header, sizeof(wav_header_t));

    ESP_LOGV("wav", "RIFF           : %.4s", header.RIFF);
    ESP_LOGV("wav", "chunk_size     : %d", header.chunk_size);
    ESP_LOGV("wav", "WAVEfmt        : %.8s", header.WAVEfmt);
    ESP_LOGV("wav", "fmt_chunk_size : %d", header.fmt_chunk_size);
    ESP_LOGV("wav", "audiofmt       : %d", header.audiofmt);
    ESP_LOGV("wav", "channel        : %d", header.channel);
    ESP_LOGV("wav", "sample_rate    : %d", header.sample_rate);
    ESP_LOGV("wav", "byte_per_sec   : %d", header.byte_per_sec);
    ESP_LOGV("wav", "block_size     : %d", header.block_size);
    ESP_LOGV("wav", "bit_per_sample : %d", header.bit_per_sample);

    if (memcmp(header.RIFF, "RIFF", 4) ||
        memcmp(header.WAVEfmt, "WAVEfmt ", 8) || header.audiofmt != 1 ||
        header.bit_per_sample < 8 || header.bit_per_sample > 16 ||
        header.channel == 0 || header.channel > 2) {
        file.close();
        return false;
    }
    file.seek(offsetof(wav_header_t, audiofmt) + header.fmt_chunk_size);
    sub_chunk_t sub_chunk;

    file.read((uint8_t*)&sub_chunk, 8);

    ESP_LOGV("wav", "sub id         : %.4s", sub_chunk.identifier);
    ESP_LOGV("wav", "sub chunk_size : %d", sub_chunk.chunk_size);

    while (memcmp(sub_chunk.identifier, "data", 4)) {
        if (!file.seek(sub_chunk.chunk_size, SeekMode::SeekCur)) {
            break;
        }
        file.read((uint8_t*)&sub_chunk, 8);

        ESP_LOGV("wav", "sub id         : %.4s", sub_chunk.identifier);
        ESP_LOGV("wav", "sub chunk_size : %d", sub_chunk.chunk_size);
    }

    if (memcmp(sub_chunk.identifier, "data", 4)) {
        file.close();
        return false;
    }

    int32_t data_len = sub_chunk.chunk_size;
    bool flg_16bit = (header.bit_per_sample >> 4);

    size_t idx = 0;
    while (data_len > 0) {
        size_t len = data_len < WAV_BUF_SIZE ? data_len : WAV_BUF_SIZE;
        len = file.read(wav_data[idx], len);
        data_len -= len;

        if (flg_16bit) {
            M5.Speaker.playRaw((const int16_t*)wav_data[idx], len >> 1,
                               header.sample_rate, header.channel > 1, 1, 0);
        } else {
            M5.Speaker.playRaw((const uint8_t*)wav_data[idx], len,
                               header.sample_rate, header.channel > 1, 1, 0);
        }
        idx = idx < (WAV_N_BUFS - 1) ? idx + 1 : 0;
    }
    file.close();
    return true;
}

void AtomEcho::showLED(const led_color_t& color) const {
    showLED(color.R, color.G, color.B);
}

void AtomEcho::showLED(uint8_t r, uint8_t g, uint8_t b) const {
    neopixelWrite(RGB_LED_PIN, getColorValue(r), getColorValue(g),
                  getColorValue(b));
}

void AtomEcho::setBrightness(uint8_t brightness) {
    this->_brightness = brightness;
}

uint8_t AtomEcho::getColorValue(uint8_t v) const {
    return v * _brightness * MAX_BRIGHTNESS / 100 / 255;
}
