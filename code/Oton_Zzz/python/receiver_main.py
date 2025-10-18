import serial
import time

# ==== 設定 ====
PORT = "COM6"       # デバイスマネージャーで確認
BAUD = 115200       # M5Stack側と一致させる
# ===============

def main():
    print("=== Python → M5Stack シリアル送信開始 ===")
    with serial.Serial(PORT, BAUD, timeout=1) as ser:
        while True:
            try:
                msg = input("送信するテキストを入力してください > ")
                if msg.lower() in ["exit", "quit"]:
                    print("終了します。")
                    break

                # 文字列を送信（末尾に改行をつける）
                ser.write((msg + "\n").encode())
                print(f"[PC→M5] 送信: {msg}")

                time.sleep(0.1)
            except KeyboardInterrupt:
                print("\n終了します。")
                break
            except Exception as e:
                print("エラー:", e)

if __name__ == "__main__":
    main()
