#ifndef IR_CONTROLLER_H
#define IR_CONTROLLER_H

#include "config.h"
#include <IRsend.h>
#include <IRrecv.h>

class IRController {
private:
    IRsend irsend;
    IRrecv irrecv;
    decode_results results;
    
    IRSignal savedSignal;
    IRSignal registerBuffer[REGISTRATION_ATTEMPTS];
    int registerCount;
    unsigned long lastReceiveTime;
    unsigned long lastSignalTime;
    
    // プライベートメソッド
    void saveReceivedSignal(int index, decode_results* result);
    bool verifyAndRegisterSignal();
    void clearRegisterBuffer();
    bool isValidSignal(const decode_results& result);
    bool isRepeatSignal(unsigned long currentTime);
    
public:
    IRController();
    ~IRController();
    
    // 初期化
    void begin();
    
    // 信号登録関連
    bool handleRegisterMode();
    void resetRegistration();
    int getRegisterCount() const { return registerCount; }
    bool isRegistrationTimeout() const;
    
    // 信号送信
    bool sendRegisteredSignal();
    
    // 送信モード時の信号監視
    bool checkForRegisteredSignal();
    
    // 状態確認
    bool isSignalRegistered() const { return savedSignal.isRegistered; }
    IRSignal getRegisteredSignal() const { return savedSignal; }
    
    // デバッグ用
    void printSignalDetails(const decode_results& result);
};

#endif // IR_CONTROLLER_H