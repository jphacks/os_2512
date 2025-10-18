#include "ir_controller.h"

IRController::IRController() : 
    irsend(IR_LED_PIN), 
    irrecv(RECV_PIN),
    registerCount(0),
    lastReceiveTime(0),
    lastSignalTime(0) {
}

IRController::~IRController() {
    if (savedSignal.rawData != nullptr) {
        delete[] savedSignal.rawData;
    }
}

void IRController::begin() {
    irsend.begin();
    irrecv.enableIRIn();
    resetRegistration();
}

void IRController::saveReceivedSignal(int index, decode_results* result) {
    registerBuffer[index].protocol = result->decode_type;
    registerBuffer[index].value = result->value;
    registerBuffer[index].bits = result->bits;
    registerBuffer[index].rawData = nullptr;  // RAWデータは使用しない
    registerBuffer[index].rawLen = 0;
    
    Serial.printf("Saved signal: Protocol=%s, Value=0x%llX, Bits=%d\n", 
        typeToString(result->decode_type).c_str(), result->value, result->bits);
        
    // Panasonicプロトコルの詳細情報表示
    if (result->decode_type == PANASONIC && result->bits == 48) {
        uint16_t address = (result->value >> 32) & 0xFFFF;
        uint32_t data = result->value & 0xFFFFFFFF;
        Serial.printf("  Panasonic details: Address=0x%04X, Data=0x%08X\n", address, data);
    }
}

bool IRController::verifyAndRegisterSignal() {
    // 3つの信号が同じかチェック（プロトコル、ビット数、値を比較）
    for (int i = 1; i < REGISTRATION_ATTEMPTS; i++) {
        if (registerBuffer[0].protocol != registerBuffer[i].protocol ||
            registerBuffer[0].bits != registerBuffer[i].bits ||
            registerBuffer[0].value != registerBuffer[i].value) {
            return false;
        }
    }
    
    // 信号を保存
    if (savedSignal.rawData != nullptr) {
        delete[] savedSignal.rawData;
    }
    
    savedSignal = registerBuffer[0];
    savedSignal.rawData = nullptr;  // RAWデータは使用しない
    savedSignal.isRegistered = true;
    
    Serial.printf("Signal registered: Protocol=%s, Bits=%d, Value=0x%llX\n", 
        typeToString(savedSignal.protocol).c_str(), savedSignal.bits, savedSignal.value);
    
    return true;
}

void IRController::clearRegisterBuffer() {
    for (int i = 0; i < REGISTRATION_ATTEMPTS; i++) {
        registerBuffer[i] = IRSignal();
    }
}

bool IRController::isValidSignal(const decode_results& result) {
    // リピート信号のチェック
    if (result.repeat) {
        Serial.println("Repeat signal detected - ignoring");
        return false;
    }
    
    // 20ビット以下はノイズとして除外
    if (result.bits < MIN_SIGNAL_BITS) {
        Serial.printf("Signal too short (%d bits) - ignoring as noise\n", result.bits);
        return false;
    }
    
    // UNKNOWNプロトコルは登録しない
    if (result.decode_type == UNKNOWN) {
        Serial.println("UNKNOWN protocol detected - only known protocols are supported");
        Serial.println("Supported Japanese TV brands:");
        Serial.println("Sony, Panasonic, Sharp, Mitsubishi, JVC, Samsung, LG, NEC");
        return false;
    }
    
    return true;
}

bool IRController::isRepeatSignal(unsigned long currentTime) {
    return (currentTime - lastSignalTime < REPEAT_IGNORE_TIME);
}

bool IRController::handleRegisterMode() {
    // タイムアウトチェック
    if (isRegistrationTimeout()) {
        Serial.println("Register timeout - resetting...");
        resetRegistration();
        return false;
    }
    
    // 赤外線信号を受信した場合
    if (irrecv.decode(&results)) {
        unsigned long currentTime = millis();
        
        // 短時間内の連続信号を無視（リピート対策）
        if (isRepeatSignal(currentTime)) {
            Serial.printf("Signal too soon after last one (%lu ms) - ignoring repeat\n", 
                          currentTime - lastSignalTime);
            irrecv.resume();
            return false;
        }
        
        // 信号の有効性チェック
        if (!isValidSignal(results)) {
            irrecv.resume();
            return false;
        }
        
        lastReceiveTime = currentTime;
        lastSignalTime = currentTime;
        
        Serial.printf("Valid signal %d received: %s\n", registerCount + 1, resultToHumanReadableBasic(&results).c_str());
        printSignalDetails(results);
        
        // 受信したデータを保存
        saveReceivedSignal(registerCount, &results);
        registerCount++;
        
        // 3回受信したら比較
        if (registerCount >= REGISTRATION_ATTEMPTS) {
            if (verifyAndRegisterSignal()) {
                Serial.println("Signal successfully registered!");
                return true;  // 登録完了
            } else {
                Serial.println("Signals don't match - resetting...");
                resetRegistration();
            }
        }
        
        irrecv.resume();
    }
    
    return false;  // まだ登録中
}

void IRController::resetRegistration() {
    registerCount = 0;
    lastSignalTime = 0;
    lastReceiveTime = millis();  // 現在時刻をセット
    clearRegisterBuffer();
}

bool IRController::isRegistrationTimeout() const {
    return (registerCount > 0 && (millis() - lastReceiveTime) > RECEIVE_TIMEOUT);
}

bool IRController::sendRegisteredSignal() {
    if (!savedSignal.isRegistered) {
        Serial.println("No signal registered!");
        return false;
    }
    
    Serial.println("Sending registered signal...");
    
    // 既知のプロトコルのみ送信対応
    if (savedSignal.protocol != UNKNOWN && savedSignal.bits > 0) {
        Serial.printf("Sending known protocol: %s, Value: 0x%llX, Bits: %d\n", 
            typeToString(savedSignal.protocol).c_str(), savedSignal.value, savedSignal.bits);
        
        switch (savedSignal.protocol) {
            case NEC:
                irsend.sendNEC(savedSignal.value, savedSignal.bits);
                break;
            case SONY:
                irsend.sendSony(savedSignal.value, savedSignal.bits);
                break;
            case RC5:
                irsend.sendRC5(savedSignal.value, savedSignal.bits);
                break;
            case RC6:
                irsend.sendRC6(savedSignal.value, savedSignal.bits);
                break;
            case SAMSUNG:
                irsend.sendSAMSUNG(savedSignal.value, savedSignal.bits);
                break;
            case LG:
                irsend.sendLG(savedSignal.value, savedSignal.bits);
                break;
            case PANASONIC:
                irsend.sendPanasonic64(savedSignal.value, savedSignal.bits);
                break;
            case SHARP:
                irsend.sendSharpRaw(savedSignal.value, savedSignal.bits);
                break;
            case MITSUBISHI:
                irsend.sendMitsubishi(savedSignal.value, savedSignal.bits);
                break;
            case JVC:
                irsend.sendJVC(savedSignal.value, savedSignal.bits);
                break;
            default:
                Serial.printf("Unsupported protocol: %s\n", typeToString(savedSignal.protocol).c_str());
                return false;
        }
    } else {
        Serial.println("No valid signal data to send!");
        return false;
    }
    
    Serial.println("Signal sent!");
    return true;
}

bool IRController::checkForRegisteredSignal() {
    // 登録済み信号がない場合は監視しない
    if (!savedSignal.isRegistered) {
        return false;
    }
    
    // 赤外線信号を受信した場合
    if (irrecv.decode(&results)) {
        unsigned long currentTime = millis();
        
        // リピート信号は無視
        if (results.repeat) {
            irrecv.resume();
            return false;
        }
        
        // 短時間内の連続信号を無視（リピート対策）
        if (isRepeatSignal(currentTime)) {
            irrecv.resume();
            return false;
        }
        
        // 受信信号が登録済み信号と一致するかチェック
        bool isMatching = (results.decode_type == savedSignal.protocol &&
                          results.value == savedSignal.value &&
                          results.bits == savedSignal.bits);
        
        if (isMatching) {
            lastSignalTime = currentTime;
            Serial.printf("REGISTERED_SIGNAL_DETECTED: Protocol=%s, Value=0x%llX, Bits=%d\n",
                         typeToString(results.decode_type).c_str(), 
                         results.value, 
                         results.bits);
            irrecv.resume();
            return true;
        }
        
        irrecv.resume();
    }
    
    return false;
}

void IRController::printSignalDetails(const decode_results& result) {
    Serial.printf("Protocol: %s, Value: 0x%llX, Bits: %d, Repeat: %s\n", 
                  typeToString(result.decode_type).c_str(), 
                  result.value, 
                  result.bits, 
                  result.repeat ? "Yes" : "No");
    
    // Panasonicの場合の詳細表示
    if (result.decode_type == PANASONIC) {
        Serial.printf("Raw Panasonic data analysis:\n");
        Serial.printf("  Full value: 0x%llX (%d bits)\n", result.value, result.bits);
        if (result.bits == 48) {
            uint16_t addr = (result.value >> 32) & 0xFFFF;
            uint32_t cmd = result.value & 0xFFFFFFFF;
            Serial.printf("  Address: 0x%04X, Command: 0x%08X\n", addr, cmd);
        }
    }
}