#include "display_manager.h"

DisplayManager::DisplayManager() {
}

void DisplayManager::begin() {
    setupDisplay();
}

void DisplayManager::setupDisplay() {
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(1);
}

void DisplayManager::clearScreen() {
    M5.Lcd.fillScreen(BLACK);
}

void DisplayManager::setDefaultTextProperties() {
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(1);
}

void DisplayManager::showSendMode(bool hasSignal, const IRSignal& signal) {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    setDefaultTextProperties();
    
    M5.Lcd.println("=== SEND MODE ===");
    M5.Lcd.println();
    M5.Lcd.println("A: Send signal");
    M5.Lcd.println("B: Register mode");
    M5.Lcd.println();
    
    if (hasSignal) {
        M5.Lcd.setTextColor(GREEN);
        M5.Lcd.println("Signal registered!");
        M5.Lcd.printf("Protocol: %s\n", typeToString(signal.protocol).c_str());
        M5.Lcd.printf("Bits: %d\n", signal.bits);
        M5.Lcd.setTextColor(WHITE);
    } else {
        M5.Lcd.setTextColor(RED);
        M5.Lcd.println("No signal registered");
        M5.Lcd.setTextColor(WHITE);
    }
}

void DisplayManager::showRegisterMode(int registerCount) {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    setDefaultTextProperties();
    
    M5.Lcd.println("=== REGISTER MODE ===");
    M5.Lcd.println();
    M5.Lcd.printf("Waiting: %d/%d signals\n", registerCount, REGISTRATION_ATTEMPTS);
    M5.Lcd.println();
    M5.Lcd.println("Send same signal");
    M5.Lcd.printf("%d times...\n", REGISTRATION_ATTEMPTS);
    M5.Lcd.println();
    M5.Lcd.println("Japanese TV brands");
    M5.Lcd.println("supported!");
    M5.Lcd.println("B: Exit register mode");
    
    // 進捗表示
    showRegisterProgress(registerCount, REGISTRATION_ATTEMPTS);
}

void DisplayManager::showMessage(const String& message, int duration) {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    setDefaultTextProperties();
    M5.Lcd.println(message);
    if (duration > 0) {
        delay(duration);
    }
}

void DisplayManager::showSendingMessage() {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    setDefaultTextProperties();
    M5.Lcd.println("Sending signal...");
}

void DisplayManager::showSignalReceivedMessage(int count, const IRSignal& signal) {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    setDefaultTextProperties();
    
    M5.Lcd.println("=== REGISTER MODE ===");
    M5.Lcd.println();
    M5.Lcd.printf("Signal %d/%d received!\n", count, REGISTRATION_ATTEMPTS);
    M5.Lcd.printf("Protocol: %s\n", typeToString(signal.protocol).c_str());
    M5.Lcd.printf("Bits: %d\n", signal.bits);
    M5.Lcd.println();
    if (count < REGISTRATION_ATTEMPTS) {
        M5.Lcd.println("Wait for next signal...");
    }
}

void DisplayManager::showUnsupportedProtocolMessage() {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    setDefaultTextProperties();
    
    M5.Lcd.println("UNKNOWN protocol!");
    M5.Lcd.println();
    M5.Lcd.println("Supported brands:");
    M5.Lcd.println("Sony, Panasonic,");
    M5.Lcd.println("Sharp, Mitsubishi,");
    M5.Lcd.println("JVC, Samsung, LG");
}

void DisplayManager::showNoSignalMessage() {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    setDefaultTextProperties();
    M5.Lcd.println("No signal registered!");
}

void DisplayManager::showSignalSentMessage() {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    setDefaultTextProperties();
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.println("Signal sent!");
    M5.Lcd.setTextColor(WHITE);
}

void DisplayManager::showRegisterProgress(int current, int total) {
    M5.Lcd.setCursor(0, M5.Lcd.getCursorY() + 10);
    for (int i = 0; i < total; i++) {
        if (i < current) {
            M5.Lcd.setTextColor(GREEN);
            M5.Lcd.print("O ");
        } else {
            M5.Lcd.setTextColor(DARKGREY);
            M5.Lcd.print("- ");
        }
    }
    M5.Lcd.setTextColor(WHITE);
}

void DisplayManager::showErrorMessage(const String& error) {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("ERROR:");
    M5.Lcd.println(error);
    M5.Lcd.setTextColor(WHITE);
}

void DisplayManager::showSignalDetails(const IRSignal& signal) {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    setDefaultTextProperties();
    
    M5.Lcd.println("=== SIGNAL DETAILS ===");
    M5.Lcd.println();
    M5.Lcd.printf("Protocol: %s\n", typeToString(signal.protocol).c_str());
    M5.Lcd.printf("Value: 0x%llX\n", signal.value);
    M5.Lcd.printf("Bits: %d\n", signal.bits);
    
    if (signal.protocol == PANASONIC && signal.bits == 48) {
        uint16_t address = (signal.value >> 32) & 0xFFFF;
        uint32_t data = signal.value & 0xFFFFFFFF;
        M5.Lcd.printf("Addr: 0x%04X\n", address);
        M5.Lcd.printf("Data: 0x%08X\n", data);
    }
}

void DisplayManager::showSupportedBrands() {
    clearScreen();
    M5.Lcd.setCursor(0, 0);
    setDefaultTextProperties();
    
    M5.Lcd.println("=== SUPPORTED BRANDS ===");
    M5.Lcd.println();
    M5.Lcd.println("- Sony");
    M5.Lcd.println("- Panasonic");
    M5.Lcd.println("- Sharp");
    M5.Lcd.println("- Mitsubishi");
    M5.Lcd.println("- JVC");
    M5.Lcd.println("- Samsung");
    M5.Lcd.println("- LG");
    M5.Lcd.println("- NEC");
}