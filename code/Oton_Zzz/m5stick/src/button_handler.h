#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "config.h"
#include "M5StickCPlus2.h"

class ButtonHandler {
private:
    unsigned long lastButtonPressTime;
    static const unsigned long DEBOUNCE_DELAY = 200;  // デバウンス時間（ms）
    
    bool isDebounceTimeElapsed();
    
public:
    ButtonHandler();
    
    // 初期化
    void begin();
    
    // ボタン状態の更新
    void update();
    
    // ボタン押下検知
    bool isButtonAPressed();
    bool isButtonBPressed();
    
    // デバウンス機能付きボタン検知
    bool isButtonAPressedDebounced();
    bool isButtonBPressedDebounced();
};

#endif // BUTTON_HANDLER_H