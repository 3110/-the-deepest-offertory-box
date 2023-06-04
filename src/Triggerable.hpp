#pragma once

/*
 * トリガーの発火ができることを表す
 */
class Triggerable {
public:
    /*
     * デストラクタ
     */
    virtual ~Triggerable(void) {
    }

    /*
     * トリガーが発火したかどうか
     *
     * @retval true トリガーが発火している
     * @retval false トリガーが発火していない
     */
    virtual bool isTriggered(void) = 0;

    /*
     * トリガーを有効にします。
     *
     * @retval true トリガーを有効にできた
     * @retval false トリガーを有効にできなかった
     */
    virtual bool enable(void) = 0;

    /*
     * トリガーを無効にします。
     *
     * @retval true トリガーを無効にできた
     * @retval false トリガーを無効にできなかった
     */
    virtual bool disable(void) = 0;

    /*
     * トリガーを有効かどうかを返します。
     *
     * @retval true トリガーが有効
     * @retval false トリガーが無効
     */
    virtual bool isEnabled(void) = 0;
};
