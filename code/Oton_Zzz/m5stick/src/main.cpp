#include "M5StickCPlus2.h"
#include "config.h"
#include "ir_controller.h"
#include "display_manager.h"
#include "button_handler.h"

// グローバルオブジェクト
IRController irController;
DisplayManager displayManager;
ButtonHandler buttonHandler;

// プログラムの状態
Mode currentMode = SEND_MODE;

// 関数の前方宣言
void updateDisplay();
void handleSendSignal();
void handleRegisterMode();
void toggleMode();

void toggleMode() {
  if (currentMode == SEND_MODE) {
    currentMode = REGISTER_MODE;
    irController.resetRegistration();
    Serial.println("Entered register mode - waiting for 3 identical signals...");
    Serial.println("Repeat signals will be automatically ignored");
  } else {
    currentMode = SEND_MODE;
    Serial.println("Returned to send mode");
  }
  updateDisplay();
}

void updateDisplay() {
  if (currentMode == SEND_MODE) {
    displayManager.showSendMode(irController.isSignalRegistered(), 
                               irController.getRegisteredSignal());
  } else {
    displayManager.showRegisterMode(irController.getRegisterCount());
  }
}

void handleSendSignal() {
  if (irController.sendRegisteredSignal()) {
    displayManager.showSignalSentMessage();
    delay(1000);
  } else {
    if (!irController.isSignalRegistered()) {
      displayManager.showNoSignalMessage();
      delay(2000);
    } else {
      displayManager.showErrorMessage("Unsupported protocol!");
      delay(2000);
    }
  }
  updateDisplay();
}

void handleRegisterMode() {
  bool registrationComplete = irController.handleRegisterMode();
  
  if (registrationComplete) {
    Serial.println("Registration completed successfully!");
    currentMode = SEND_MODE;
    updateDisplay();
  } else if (irController.isRegistrationTimeout()) {
    updateDisplay();  // IRController内でリセット済み
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  
  // 各オブジェクトの初期化
  irController.begin();
  displayManager.begin();
  buttonHandler.begin();
  
  Serial.println("M5StickC Plus2 IR Remote Control");
  Serial.println("A: Send registered signal");
  Serial.println("B: Enter register mode");
  Serial.println("-------------------------");
  
  updateDisplay();
}

void loop() {
  buttonHandler.update();
  
  // Aボタン：登録された信号を送信
  if (buttonHandler.isButtonAPressed()) {
    if (currentMode == SEND_MODE) {
      handleSendSignal();
    }
  }
  
  // Bボタン：登録モードの切り替え
  if (buttonHandler.isButtonBPressed()) {
    toggleMode();
  }
  
  // 登録モード時の受信処理
  if (currentMode == REGISTER_MODE) {
    handleRegisterMode();
  }
  
  delay(50);
}