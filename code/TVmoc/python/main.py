"""
mp4 を OpenCV (cv2) でループ再生する簡単なスクリプト。

使い方:
    python main.py path/to/video.mp4 [--fullscreen]

キー操作:
    0 : 黒画面に切り替え（TVの電源OFF状態）
    1 : チャンネル1に切り替え
    q, ESC : 終了
    p, SPACE: 一時停止/再生
    f : フルスクリーン切替

引数:
    --speed: 再生速度の倍率（デフォルト 1.0）
    --window: ウィンドウ名（デフォルト 'Video'）
    --fullscreen: 起動時にフルスクリーン表示する（省略可）

注意: GUI が使えない環境（ヘッドレス）では再生できません。その場合は ffplay や VLC を使ってください。
"""

import argparse
import sys
import time
import os
import numpy as np

try:
    import cv2
except Exception as e:
    sys.stderr.write("Error: OpenCV (cv2) がインポートできません。\n")
    sys.stderr.write("インストール方法: pip install opencv-python\n")
    raise


def loop_play(video_path: str, speed: float = 1.0, window_name: str = "Video", fullscreen: bool = False):
    if not os.path.isfile(video_path):
        raise FileNotFoundError(f"指定されたファイルが見つかりません: {video_path}")

    cap = cv2.VideoCapture(video_path)
    if not cap.isOpened():
        raise RuntimeError(f"ビデオを開けませんでした: {video_path}")

    # 元のフレームレートを取得。取得できなければ 30 fps を想定
    fps = cap.get(cv2.CAP_PROP_FPS) or 30.0
    # waitKey の間隔（ms）を計算
    if speed <= 0:
        speed = 1.0
    delay_ms = max(1, int(1000.0 / (fps * speed)))

    paused = False
    # ウィンドウはまず NORMAL で作成しておく（フルスクリーン切替用）
    cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)

    # フルスクリーン初期化
    is_fullscreen = False
    if fullscreen:
        # WND_PROP_FULLSCREEN に WINDOW_FULLSCREEN を設定してフルスクリーン化
        cv2.setWindowProperty(window_name, cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
        is_fullscreen = True

    # 再生モード: 'video' (通常) または 'black' (真っ暗)
    mode = 'video'
    last_frame = None
    black_frame = None

    while True:
        # 通常再生モードかつ再生中であればフレームを進める
        if mode == 'video' and not paused:
            ret, frame = cap.read()
            if not ret:
                # ファイルの終端に達したら先頭に戻す
                cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
                continue
            last_frame = frame
            # 黒フレームを初回フレームのサイズで作成
            if black_frame is None:
                h, w = frame.shape[:2]
                black_frame = np.zeros((h, w, 3), dtype=frame.dtype)
            display = frame
        else:
            # 黒画面モードまたは一時停止時はフレームを進めない
            if mode == 'black':
                if black_frame is None:
                    # 動画の解像度が取得できなければデフォルトを使う
                    w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)) or 640
                    h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT)) or 480
                    black_frame = np.zeros((h, w, 3), dtype='uint8')
                display = black_frame
            else:
                # mode == 'video' but paused: 表示は最後のフレーム
                if last_frame is not None:
                    display = last_frame
                else:
                    # まだフレームがない場合は黒画面を表示
                    if black_frame is None:
                        w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)) or 640
                        h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT)) or 480
                        black_frame = np.zeros((h, w, 3), dtype='uint8')
                    display = black_frame

        # 表示
        cv2.imshow(window_name, display)

        key = cv2.waitKey(delay_ms) & 0xFF
        if key == ord('q') or key == 27:
            break
        elif key == ord('p') or key == ord(' '):
            paused = not paused
        elif key == ord('f') or key == ord('F'):
            # フルスクリーン切替
            is_fullscreen = not is_fullscreen
            if is_fullscreen:
                cv2.setWindowProperty(window_name, cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
            else:
                # 元に戻す
                cv2.setWindowProperty(window_name, cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_NORMAL)
        elif key == ord('1'):
            mode = 'video'
        elif key == ord('0'):
            mode = 'black'

    cap.release()
    cv2.destroyAllWindows()


def main():
    parser = argparse.ArgumentParser(description="OpenCV で mp4 をループ再生する")
    # file is optional now because VIDEO_PATH config may be used
    parser.add_argument("file", nargs='?', help="再生する mp4 ファイルパス (省略可: スクリプト内変数 VIDEO_PATH を使用) ")
    parser.add_argument("--speed", type=float, default=1.0, help="再生速度の倍率 (default=1.0)")
    parser.add_argument("--window", default="Video", help="ウィンドウ名 (default='Video')")
    parser.add_argument("--fullscreen", action="store_true", help="起動時にフルスクリーン表示する")

    args = parser.parse_args()

    # Configuration variables: set VIDEO_PATH and START_FULLSCREEN here to manage
    # playback without CLI args. If VIDEO_PATH is None, the CLI positional 'file'
    # will be used and is required. If START_FULLSCREEN is None, the CLI
    # --fullscreen flag is used.
    VIDEO_PATH = "videos/video1.mp4"
    # Example to set defaults in-script (uncomment and edit):
    # VIDEO_PATH = r".\videos\video1.mp4"

    START_FULLSCREEN = True
    # Example: START_FULLSCREEN = True

    # determine effective file and fullscreen setting
    file_to_play = VIDEO_PATH if VIDEO_PATH else args.file
    if not file_to_play:
        sys.stderr.write("Error: video file must be provided either by VIDEO_PATH variable or as a CLI argument.\n")
        parser.print_help()
        sys.exit(2)

    fullscreen = START_FULLSCREEN if (START_FULLSCREEN is not None) else args.fullscreen

    try:
        loop_play(file_to_play, speed=args.speed, window_name=args.window, fullscreen=fullscreen)
    except Exception as e:
        sys.stderr.write(f"Error: {e}\n")
        sys.exit(1)


if __name__ == "__main__":
    main()