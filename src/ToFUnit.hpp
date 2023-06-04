#pragma once

#include <VL53L0X.h>
#include <Wire.h>

#include "DistanceMeasurable.hpp"

typedef uint16_t distance_unit_t;

/*
 * M5Stack の ToF Unitを扱うクラス
 *
 * https://docs.m5stack.com/en/unit/tof
 */
class ToFUnit : public DistanceMeasurable<distance_unit_t, 3> {
public:
    /* 計測できる最小長（30mm） */
    static constexpr distance_unit_t MIN_DISTANCE_MM = 30;
    /* 計測できる最大長（2000mm） */
    static constexpr distance_unit_t MAX_DISTANCE_MM = 2000;
    /* I2Cアドレス */
    static constexpr uint8_t I2C_ADDRESS = 0x29;
    /* 接続待ちのタイムアウト（500ミリ秒） */
    static constexpr uint16_t DEFAULT_CONNECTION_TIMEOUT = 500;
    /* 測定精度（±3%）*/
    static constexpr double ACCURACY = 0.03;
    /* 測定不能だった場合の値。8190，8191が返る */
    static constexpr distance_unit_t OUT_OF_RANGE_MIN = 8190;
    static constexpr distance_unit_t OUT_OF_RANGE_MAX = 8191;

    /*
     * コンストラクタ
     *
     * @param wire 接続に使用するI2C接続
     * @param sda SDAピン番号
     * @param scl SCLピン番号
     * @param timeout 接続待ちのタイムアウト（ミリ秒）
     */
    ToFUnit(TwoWire& wire, uint8_t sda, uint8_t scl,
            uint16_t timeout = DEFAULT_CONNECTION_TIMEOUT);

    /*
     * デストラクタ
     */
    virtual ~ToFUnit(void);

    /*
     * ToF Unitを初期化します。
     *
     * @retval true  初期化が成功した
     * @retval false 初期化が失敗した
     */
    virtual bool begin(void);

    /*
     * ユニットの名前を返します
     *
     * @return ユニットの名前
     */
    virtual const char* getName(void) const;

    /*
     * 測定した距離（mm）を返します
     *
     * @param distance 測定した距離
     * @retval true 測定できた場合
     * @retval false 測定できなかった場合
     */
    virtual bool getDistance(distance_unit_t& distance);

    /*
     * 測定できる最小長（mm）を返します。
     *
     * @return 計測できる最小長（mm）
     */
    virtual distance_unit_t getMinDistance(void) const;

    /*
     * 測定できる最大長（mm）を返します。
     *
     * @return 計測できる最大長（mm）
     */
    virtual distance_unit_t getMaxDistance(void) const;

    /*
     * 測定精度を返します。
     *
     * @return 測定精度（パーセント：0.0-1.0）
     */
    virtual double getAccuracy(void) const;

private:
    const uint8_t _sda;
    const uint8_t _scl;
    const uint16_t _timeout;

    VL53L0X _sensor;
    TwoWire& _wire;
};
