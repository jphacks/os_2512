#include "M5StickCPlus2.h"
#include "config.h"
#include "ir_controller.h"
#include "display_manager.h"
#include "display_manager_img.h"
#include "button_handler.h"

// グローバルオブジェクト
IRController irController;
DisplayManager displayManager;
ButtonHandler buttonHandler;

// プログラムの状態
Mode currentMode = SEND_MODE;
TVstatus currentTVstatus = TV_OFF;
Oton currentOtonStatus = AWAKE;

// 関数の前方宣言
void updateDisplay();
void handleSendSignal();
void handleRegisterMode();
void toggleMode();
void toggleTVstatus();
void setOtonStatus(Oton status);
void handleSerialCommand();
void handleSendModeMonitoring();

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

void toggleTVstatus() {
  if (currentTVstatus == TV_OFF) {
    currentTVstatus = TV_ON;
    setOtonStatus(AWAKE);
  } else {
    currentTVstatus = TV_OFF;
  }
  Serial.print("TV status changed to: ");
  Serial.println(currentTVstatus == TV_ON ? "ON" : "OFF");
  updateDisplay();
}

void setOtonStatus(Oton status) {
  currentOtonStatus = status;
  updateDisplay();
}

void updateDisplay() {
  if (currentMode == REGISTER_MODE) {
    displayManager.showRegisterMode(irController.getRegisterCount());
    return;
  }
  if( currentTVstatus == TV_OFF) {
    displayManager.showImage((uint16_t*)imgOff);
    return;
  }
  if (currentOtonStatus == SLEEP) {
    displayManager.showImage((uint16_t*)imgSleep);
  } else {
    displayManager.showImage((uint16_t*)imgAwake);
  }
}

void handleSendSignal() {
  if (irController.sendRegisteredSignal()) {
    displayManager.showMessage("Signal sent!", 1000, GREEN);
  } else {
    if (!irController.isSignalRegistered()) {
      displayManager.showMessage("No signal registered!", 2000, RED);
    } else {
      displayManager.showMessage("ERROR: Unsupported protocol!", 2000, RED);
    }
  }
  updateDisplay();
}

void handleRegisterMode() {
  int previousRegisterCount = irController.getRegisterCount();
  bool registrationComplete = irController.handleRegisterMode();
  int currentRegisterCount = irController.getRegisterCount();
  
  if (registrationComplete) {
    Serial.println("Registration completed successfully!");
    currentMode = SEND_MODE;
    updateDisplay();
  } else if (irController.isRegistrationTimeout()) {
    updateDisplay();  // IRController内でリセット済み
  } else if (previousRegisterCount != currentRegisterCount) {
    updateDisplay();
  }
}

void handleSerialCommand() {
  if (!Serial.available() || currentMode != SEND_MODE) {
    return;
  }
  String command = Serial.readStringUntil('\n');
  command.trim();  // 前後の空白や改行を削除
  Serial.printf("Received command: '%s'\n", command.c_str());
  if (command == "OFF") {
    Serial.println("OFF command received - sending IR signal");
    if (currentTVstatus == TV_OFF) {
      Serial.println("TV is already OFF");
    return;
  }
    handleSendSignal();
    toggleTVstatus();
  } else if (command == "ALERT") {
    Serial.println("ALERT command received - sending IR signal");
    setOtonStatus(SLEEP);
  } else if(command == "AWAKE") {
    Serial.println("AWAKE command received - sending IR signal");
    setOtonStatus(AWAKE);

  } else {
    Serial.printf("Unknown command: '%s'\n", command.c_str());
    Serial.println("Send 'HELP' for available commands");
  }
}

void handleSendModeMonitoring() {
  // 送信モード時に登録済み信号を監視
  if (irController.checkForRegisteredSignal()) {
    // 登録済み信号を受信した場合の処理
    toggleTVstatus();
    Serial.println("Registered signal detected - toggled TV status");
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
  Serial.println("Serial Commands:");
  Serial.println("  TV_OFF - Send IR signal");
  Serial.println("  STATUS - Show status");
  Serial.println("  HELP   - Show commands");
  Serial.println("-------------------------");
  
  updateDisplay();
}

void loop() {
  buttonHandler.update();
  
  // シリアル通信でのコマンド処理
  handleSerialCommand();
  
  // Aボタン：登録された信号を送信（ボタンでも送信可能）
  if (buttonHandler.isButtonAPressed()) {
    if (currentMode == SEND_MODE) {
      toggleTVstatus();
      Serial.println("A button pressed - sending registered signal");
    }
  }
  
  // Bボタン：登録モードの切り替え
  if (buttonHandler.isButtonBPressed()) {
    toggleMode();
  }
  
  // モード別の処理
  if (currentMode == REGISTER_MODE) {
    // 登録モード時の受信処理
    handleRegisterMode();
  } else {
    // 送信モード時の信号監視
    handleSendModeMonitoring();
  }
  
  delay(50);
}