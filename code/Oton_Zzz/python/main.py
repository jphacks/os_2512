import serial
import time

# ==== 設定 ====
PORT = "COM5"       # デバイスマネージャーで確認したポート名
BAUD = 115200       # M5Stack側と一致させる（一般的に115200）
# ===============

def main():
    print("=== M5Stackシリアル受信開始 ===")
    with serial.Serial(PORT, BAUD, timeout=1) as ser:
        while True:
            try:
                line = ser.readline().decode(errors="ignore").strip()
                if line:
                    print(f"[受信] {line}")
                time.sleep(0.05)
            except KeyboardInterrupt:
                print("\n終了します。")
                break
            except Exception as e:
                print("エラー:", e)

if __name__ == "__main__":
    main()
