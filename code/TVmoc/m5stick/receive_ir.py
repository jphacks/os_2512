import serial
import serial.tools.list_ports
import sys
from datetime import datetime

# シリアルポート設定
SERIAL_PORT = 'COM15'  # M5StickC Plus2のポート
BAUD_RATE = 115200

def find_m5stick_port():
    """M5StickC Plus2のポートを自動検出"""
    ports = serial.tools.list_ports.comports()

    # キーワードでM5StickC Plus2を検索
    keywords = ['CP210', 'Silicon Labs', 'CH340', 'USB-SERIAL']

    for port in ports:
        description = str(port.description).lower()
        manufacturer = str(port.manufacturer).lower() if port.manufacturer else ""

        for keyword in keywords:
            if keyword.lower() in description or keyword.lower() in manufacturer:
                return port.device

    # 見つからない場合は最初のポートを返す
    if ports:
        return ports[0].device

    return None

def main():
    global SERIAL_PORT

    # ポート自動検出
    if SERIAL_PORT is None:
        SERIAL_PORT = find_m5stick_port()
        if SERIAL_PORT is None:
            print("❌ シリアルポートが見つかりませんでした。")
            print("\n利用可能なポートを確認:")
            print("  python find_port.py")
            sys.exit(1)
        print(f"🔍 自動検出: {SERIAL_PORT}")

    print("=" * 60)
    print("IR Remote Receiver - PC Monitor")
    print("=" * 60)
    print(f"接続先: {SERIAL_PORT} @ {BAUD_RATE} baud")
    print("待機中... (Ctrl+C で終了)")
    print("-" * 60)

    try:
        # シリアルポートを開く
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

        while True:
            # シリアルデータを読み取る
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8', errors='ignore').strip()

                if line:
                    timestamp = datetime.now().strftime("%H:%M:%S")

                    # ボタンに応じた処理
                    if line == "TV_OFF":
                        print(f"[{timestamp}] 🔴 テレビ電源OFF")
                    elif line == "CH_1":
                        print(f"[{timestamp}] 📺 チャンネル1")
                    elif line == "CH_2":
                        print(f"[{timestamp}] 📺 チャンネル2")
                    elif line == "CH_3":
                        print(f"[{timestamp}] 📺 チャンネル3")
                    elif line == "CH_4":
                        print(f"[{timestamp}] 📺 チャンネル4")
                    elif line == "CH_5":
                        print(f"[{timestamp}] 📺 チャンネル5")
                    else:
                        # その他のメッセージ
                        print(f"[{timestamp}] {line}")

    except serial.SerialException as e:
        print(f"\n❌ エラー: シリアルポートを開けません: {e}")
        print(f"\n利用可能なポートを確認してください:")
        print("  Windows: デバイスマネージャーで確認")
        print("  Mac/Linux: ls /dev/tty* または ls /dev/cu*")
        sys.exit(1)

    except KeyboardInterrupt:
        print("\n\n終了します...")
        ser.close()
        sys.exit(0)

if __name__ == "__main__":
    main()
