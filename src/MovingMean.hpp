#pragma once

#include <array>

/*
 * n項移動平均を求めるクラス
 *
 * @param T 値の型
 * @param WINDOW_SIZE 移動平均の項数
 */
template <class T = uint16_t, std::size_t WINDOW_SIZE = 3>
class MovingMean {
public:
    /*
     * コンストラクタ
     */
    MovingMean(void) : _ready(false), _window{0}, _pos(0), _sum(0) {
    }

    /*
     * デストラクタ
     */
    ~MovingMean(void) {
    }

    /*
     * 移動平均の値が取れる状態（項数分のデータを取得できた）かを返します。
     *
     * @retval true 移動平均の値が取れる
     * @retval false データが足りなくて移動平均の値が取れない
     */
    bool ready(void) const {
        return this->_ready;
    }

    /*
     * 値を追加します。
     * ready()がfalseの場合は返り値の移動平均値は正しくありません。
     *
     * @param v 値
     * @return 移動平均値
     */
    T update(T v) {
        this->_sum -= this->_window[this->_pos];
        this->_window[this->_pos] = v;
        this->_sum += v;
        ESP_LOGD("MM", "Value: %d, Sum: %d", v, this->_sum);
        ++(this->_pos);
        if (this->_pos == WINDOW_SIZE) {
            if (!this->_ready) {
                this->_ready = true;
            }
            this->_pos = 0;
        }
        return get();
    }

    /*
     * 移動平均値を取得します。
     * ready()がfalseの場合は移動平均の値として正しくありません。
     *
     * @return 移動平均値
     */
    inline T get(void) const {
        return static_cast<T>(this->_sum / WINDOW_SIZE + 0.5);
    }

    /*
     * 移動平均の項数を返します。
     *
     * @return 移動平均の項数
     */
    inline std::size_t size(void) const {
        return WINDOW_SIZE;
    }

private:
    bool _ready;
    std::array<T, WINDOW_SIZE> _window;
    size_t _pos;
    double _sum;
};