#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "config.h"
#include "M5StickCPlus2.h"

class DisplayManager {
private:
    // 画面の基本設定
    void setupDisplay();
    void clearScreen();
    void setDefaultTextProperties();
    
public:
    DisplayManager();
    
    // 初期化
    void begin();
    
    // モード別画面表示
    void showSendMode(bool hasSignal, const IRSignal& signal);
    void showRegisterMode(int registerCount);

    // 画像表示
    void showImage(uint16_t* img);
    
    // メッセージ表示
    void showMessage(const String& message, int duration = 2000);
    void showSendingMessage();
    void showSignalReceivedMessage(int count, const IRSignal& signal);
    void showUnsupportedProtocolMessage();
    void showNoSignalMessage();
    void showSignalSentMessage();
    
    // 進捗表示
    void showRegisterProgress(int current, int total);
    
    // エラーメッセージ
    void showErrorMessage(const String& error);
    
    // 詳細情報表示
    void showSignalDetails(const IRSignal& signal);
    void showSupportedBrands();
};

#endif // DISPLAY_MANAGER_H