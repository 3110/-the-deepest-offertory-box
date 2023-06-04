#pragma once

#include <FS.h>
#include <M5Unified.h>
#include <esp_log.h>

class AtomEcho {
public:
    /* RGB */
    struct __attribute__((packed)) led_color_t
    {
        uint8_t R;
        uint8_t G;
        uint8_t B;
    };

    /* LEDの最大輝度 */
    static const uint8_t MAX_BRIGHTNESS = 20;
    /* SDAピン番号 */
    static constexpr int SDA_PIN = GPIO_NUM_26;
    /* SCLピン番号 */
    static constexpr int SCL_PIN = GPIO_NUM_32;
    /* RGB LEDピン番号 */
    static constexpr int RGB_LED_PIN = GPIO_NUM_27;

    /*
     * コンストラクタ
     */
    AtomEcho(void);

    /*
     * デストラクタ
     */
    virtual ~AtomEcho(void);

    /*
     * Atom Echoを初期化します。
     */
    virtual void begin(void);

    /*
     * Atom Echoの状態を更新します。
     */
    virtual void update(void);

    /*
     * ボタンが押されていたかを返します。
     *
     * @retval true ボタンが押されてた
     * @retval false ボタンが押されていなかった
     */
    virtual bool wasPressed(void) const;

    /*
     * ボタンが押されているかを返します。
     *
     * @retval true ボタンが押されている
     * @retval false ボタンが押されていない
     */
    virtual bool isPressed(void) const;

    /*
     * スピーカーの音量を設定します。
     *
     * @v 音量（0-255）
     */
    virtual void setVolume(uint8_t v);

    /*
     * WAVファイルを再生します。
     *
     * @param fs ファイルが置いてあるファイルシステム
     * @param filename 再生するWAVファイル名
     */
    virtual bool playWav(FS& fs, const char* filename);

    /*
     * 指定した色でLEDを光らせます。
     *
     * @param r 赤（0-255）
     * @param g 緑（0-255）
     * @param b 青（0-255）
     */
    virtual void showLED(uint8_t r, uint8_t g, uint8_t b) const;

    /*
     * 指定した色でLEDを光らせます。
     *
     * @param color led_color_tのインスタンス
     */
    virtual void showLED(const led_color_t& color) const;

    /*
     * LEDの明るさを設定します。
     *
     * @param brightness LEDの明るさ（0-255）
     */
    virtual void setBrightness(uint8_t brightness);

protected:
    /*
     * 輝度を考慮した色の値を返します。
     *
     * @param v 色の値（0-255）
     *
     * @return 輝度を考慮した色の値
     */
    virtual uint8_t getColorValue(uint8_t v) const;

private:
    uint8_t _brightness;
};