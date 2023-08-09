#pragma once

#include <esp_log.h>

#include "DistanceMeasurable.hpp"
#include "Triggerable.hpp"

/*
 * 測定した距離が閾値より短かくなったことをきっかけに発火するトリガー
 */
template <class T, size_t WINDOW_SIZE>
class DistanceTrigger : public Triggerable {
public:
    /*
     * コンストラクタ
     *
     * @param measurable DistanceMeasurableのインスタンス
     */
    DistanceTrigger(DistanceMeasurable<T, WINDOW_SIZE>* measurable)
        : _initialized(false),
          _enabled(false),
          _measurable(measurable),
          _threshold(0) {
    }

    /*
     * デストラクタ
     */
    virtual ~DistanceTrigger(void) {
        if (this->_measurable != nullptr) {
            delete this->_measurable;
            this->_measurable = nullptr;
        }
    }

    /*
     * トリガーの名前を返します。
     *
     * @return トリガーの名前
     */
    virtual const char* getName(void) const {
        if (this->_measurable == nullptr) {
            return nullptr;
        }
        return this->_measurable->getName();
    }

    /*
     * トリガーを初期化します。
     * 指定した閾値以下になるとトリガーが発火（isTriggered()がtrue）します。
     *
     * @param threshold 距離の閾値（mm）
     */
    virtual bool begin(T threshold) {
        if (this->_measurable == nullptr) {
            return false;
        }
        if (!this->_initialized) {
            this->_initialized = this->_measurable->begin();
        }
        if (this->_initialized) {
            this->_initialized = setThreshold(threshold);
        }
        return this->_initialized;
    }

    /*
     * トリガーが発火しているかを返します。
     *
     * @retval true 距離の測定値が閾値より大きい
     * @retval false 距離の測定値が閾値以下
     */
    virtual bool isTriggered(void) {
        if (!this->_initialized) {
            ESP_LOGE("Trigger", "Not initialized");
            return false;
        }
        if (!this->_enabled) {
            ESP_LOGV("Trigger", "Disabled");
            return false;
        }
        T distance = 0;
        if (this->_measurable->getDistance(distance) == false) {
            return false;
        }
        const double acc = this->_measurable->getAccuracy();
        const T lower = static_cast<T>(
            this->_measurable->getMinDistance() * (1.0 + acc) + 0.5);
        const T upper = static_cast<T>(this->_threshold * (1.0 - acc) + 0.5);
        const bool triggered = lower < distance && distance < upper;
        if (triggered) {
            ESP_LOGI("Trigger", "Fired: %dmm (%dmm, %dmm)", distance, lower,
                     upper);
        } else {
            ESP_LOGD("Trigger",
                     "Moving Mean Distance(window: %d): %dmm (%dmm, %dmm)",
                     WINDOW_SIZE, distance, lower, upper);
        }
        return triggered;
    }

    /*
     * トリガーを有効にします。
     *
     * @retval true トリガーを有効にできた
     * @retval false トリガーを有効にできなかった
     */
    virtual bool enable(void) {
        if (this->_enabled) {
            ESP_LOGW("Trigger", "Already enabled");
            return false;
        }
        this->_enabled = true;
        return true;
    }

    /*
     * トリガーを無効にします。
     *
     * @retval true トリガーを無効にできた
     * @retval false トリガーを無効にできなかった
     */
    virtual bool disable(void) {
        if (!this->_enabled) {
            ESP_LOGW("Trigger", "Already disabled");
            return false;
        }
        this->_enabled = false;
        return true;
    }

    /*
     * トリガーが有効かどうかを返します。
     *
     * @retval true トリガーが有効
     * @retval false トリガーが無効
     */
    virtual bool isEnabled(void) {
        return this->_enabled;
    }

    /*
     * 指定した回数だけ距離を測り，閾値を返します。
     *
     * @param count 距離を測る回数
     * @param callback コールバック関数。引数には距離を測った回数が入る。
     * @return 校正した距離の閾値
     */
    virtual T calibrate(uint8_t count,
                        void (*callback)(uint8_t count) = nullptr) {
        if (!this->_initialized) {
            this->_initialized = this->_measurable->begin();
        }
        return this->_measurable->calibrate(count, callback);
    }

protected:
    /*
     * 距離の閾値を設定します。
     *
     * @param distance 距離の閾値
     */
    virtual bool setThreshold(T distance) {
        if (this->_measurable == nullptr) {
            return false;
        }
        T maxDistance = this->_measurable->getMaxDistance();
        if (distance >= maxDistance) {
            ESP_LOGE("Trigger", "Illegal Threshold: %d(Max: %d)", distance,
                     maxDistance);
            return false;
        } else {
            this->_threshold = distance;
            return true;
        }
    }

private:
    bool _initialized;
    bool _enabled;
    DistanceMeasurable<T, WINDOW_SIZE>* _measurable;
    T _threshold;
};
