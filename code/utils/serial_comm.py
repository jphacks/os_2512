import serial
import time

# ==== 設定 ====
PORT = "COM6"       # デバイスマネージャーで確認
BAUD = 115200       # M5Stack側と一致させる
TIMEOUT = 0
# ===============

class Serialize_controler:
    def __init__(self, port=PORT, baud=BAUD, timeout=TIMEOUT):
        """シリアルポートを開く"""
        try:
            self.ser = serial.Serial(port, baud, timeout=timeout)
            print(f"[接続成功] {port} @ {baud}bps")
        except Exception as e:
            print("接続エラー:", e)
            self.ser = None


    def send_to_m5(self, message: str):
        """1回だけ送信する"""
        try:
            self.ser.write((message + "\n").encode())
            print(f"[PC→M5] 送信: {message}")
        except Exception as e:
            print("送信エラー:", e)


    def receive_from_m5(self):
        """1回だけ受信する（データがあれば返す）"""
        try:
            line = self.ser.readline().decode(errors="ignore").strip()
            if line:
                print(f"[M5→PC] 受信: {line}")
                return line
            else:
                return None  # 何も来ていなければNoneを返す
        except Exception as e:
            print("受信エラー:", e)
            return None


    def close(self):
        """シリアルポートを閉じる"""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("[シリアルポートを閉じました]")


def main():
    controller = Serialize_controler()

    # 例：送信だけ行う
    controller.send_to_m5("Hello M5!")


    # 例：受信を1回試みる
    while True:
        data = controller.receive_from_m5()
        if data:
            print("受信結果:", data)
        else:
            print("受信なし（何も送られていません）")
        if data  == "CH_5":
            break

    controller.close()
    print("[接続終了]")


if __name__ == "__main__":
    main()
