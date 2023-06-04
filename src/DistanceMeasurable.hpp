#pragma once

#include <esp_log.h>
#include <stdint.h>

#include "MovingMean.hpp"

/*
 * 距離が測定できることを表す
 *
 * @param T 距離の型
 * @param WINDOW_SIZE 移動平均のウィンドウサイズ
 */
template <class T, size_t WINDOW_SIZE>
class DistanceMeasurable {
public:
    /*
     * デストラクタ
     */
    virtual ~DistanceMeasurable(void) {
    }

    /*
     * 初期化を開始します。
     *
     * @retval true 初期化が成功した
     * @retval false 初期化が失敗した
     */
    virtual bool begin(void) = 0;

    /*
     * 名前を返します
     *
     * @return 名前
     */
    virtual const char* getName(void) const = 0;

    /*
     * 測定した距離を返します
     *
     * @param distance 測定した距離
     * @retval true 測定できた場合
     * @retval false 測定できなかった場合
     */
    virtual bool getDistance(T& distance) = 0;

    /*
     * 測定できる最小長を返します。
     *
     * @return 計測できる最小長
     */
    virtual T getMinDistance(void) const = 0;

    /*
     * 測定できる最大長（mm）を返します。
     *
     * @return 計測できる最大長（mm）
     */
    virtual T getMaxDistance(void) const = 0;

    /*
     * 測定精度を返します。
     *
     * @return 測定精度（パーセント：0.0-1.0）
     */
    virtual double getAccuracy(void) const = 0;

    /*
     * 指定した回数だけ距離を測り，閾値を返します。
     * begin()を呼んだ後に呼ぶこと
     *
     * @param count 距離を測る回数
     * @param callback コールバック関数。引数には距離を測った回数が入る。
     * @return 校正した距離の閾値
     */
    virtual T calibrate(uint8_t count,
                        void (*callback)(uint8_t count) = nullptr) {
        uint8_t c = 0;
        double sum = 0;
        T distance = 0;
        while (c < count) {
            if (getDistance(distance) == false) {
                ESP_LOGE(getName(), "Calibration: Failed to measure");
                continue;
            }
            if (distance < getMinDistance() || distance > getMaxDistance()) {
                ESP_LOGW(getName(), "Calibration: Out of Range: %d", distance);
                continue;
            }
            ++c;
            sum += distance;
            if (callback != nullptr) {
                callback(c);
            }
            ESP_LOGD(getName(), "Calibration %3d: Distance: %dmm", c, distance);
        }
        return static_cast<T>(sum / count);
    };

protected:
    MovingMean<T, WINDOW_SIZE> _mm;
};
