import cv2
import time
import mediapipe as mp

class SleepDetectorBlendshapes:
    """Blendshapesを使った高精度睡眠検出クラス"""
    
    def __init__(self, sleep_threshold=3.0, blink_threshold=0.5, grace_period=1.0, model_path='./face_landmarker_v2_with_blendshapes.task'):
        """
        初期化
        
        Args:
            model_path: Face Landmarkerモデルのパス
            sleep_threshold: 目を閉じていると判定するまでの秒数
            blink_threshold: 目が閉じていると判定するBlendshapeの閾値
            grace_period: 目を開けても許容する猶予時間（秒）
        """
        self.model_path = model_path
        
        # 睡眠判定パラメータ
        self.SLEEP_THRESHOLD = sleep_threshold
        self.BLINK_THRESHOLD = blink_threshold
        self.GRACE_PERIOD = grace_period  # 猶予時間
        
        # タイマー管理用の変数
        self.eye_closed_start_time = None  # 目を閉じ始めた時刻
        self.eye_opened_start_time = None  # 目を開け始めた時刻（猶予時間用）
        self.is_sleeping = False           # 現在睡眠状態かどうか
        
        # 最新の検出結果を保存
        self.latest_result = None
        self.result_timestamp = 0
    
    def resultCallback(self, result: mp.tasks.vision.FaceLandmarkerResult, output_image: mp.Image, timestamp_ms: int):
        """
        LIVE_STREAMモード用の結果コールバック
        
        Args:
            result: 顔ランドマーク検出結果
            output_image: 出力画像
            timestamp_ms: タイムスタンプ（ミリ秒）
        """
        self.latest_result = result
        self.result_timestamp = timestamp_ms

    def getEyeBlinkValues(self):
        """
        Blendshapesから目の閉じ具合を取得
        
        Returns:
            tuple: (left_blink, right_blink, avg_blink)
                - left_blink: 左目の閉じ具合 (0.0-1.0)
                - right_blink: 右目の閉じ具合 (0.0-1.0)
                - avg_blink: 両目の平均閉じ具合
        """
        if (
            self.latest_result is None or 
            not self.latest_result.face_blendshapes or 
            len(self.latest_result.face_blendshapes) == 0
        ):
            return 0.0, 0.0, 0.0
        
        # 最初の顔のblendshapesを取得
        blendshapes = self.latest_result.face_blendshapes[0]
        
        left_blink = 0.0
        right_blink = 0.0
        
        # blendshapesから目の閉じ具合を検索
        for blendshape in blendshapes:
            if blendshape.category_name == "eyeBlinkLeft":
                left_blink = blendshape.score
            elif blendshape.category_name == "eyeBlinkRight":
                right_blink = blendshape.score
        
        # 両目の平均を計算
        avg_blink = (left_blink + right_blink) / 2.0
        
        return left_blink, right_blink, avg_blink
    
    def processResult(self):
        """
        最新の検出結果を処理して睡眠状態を判定（猶予時間付き）
        
        Returns:
            tuple: (is_sleeping, elapsed_seconds, status)
                - is_sleeping: 睡眠状態かどうか（True/False）
                - elapsed_seconds: 目を閉じている経過秒数
                - status: 現在の状態を表す文字列
                    - "No Face": 顔が検出されていない
                    - "Eyes Open": 目が開いている
                    - "Eyes Closed": 目が閉じている（まだ睡眠判定前）
                    - "Sleeping": 睡眠状態
                    - "Grace Period (X.Xs)": 猶予時間中
        """
        current_time = time.time()
        
        # 顔が検出されていない場合はリセット
        if self.latest_result is None or not self.latest_result.face_landmarks:
            self.eye_closed_start_time = None
            self.eye_opened_start_time = None
            self.is_sleeping = False
            return False, 0.0, "No Face"
        
        # Blendshapesから目の閉じ具合を取得
        left_blink, right_blink, avg_blink = self.getEyeBlinkValues()
        
        elapsed_seconds = 0.0
        status = "Eyes Open"
        
        # 閾値以上だと目が閉じていると判定
        if avg_blink >= self.BLINK_THRESHOLD:
            # --- 目が閉じている場合 ---
            self.eye_opened_start_time = None  # 猶予タイマーをリセット
            
            # 初めて目を閉じた場合、開始時刻を記録
            if self.eye_closed_start_time is None:
                self.eye_closed_start_time = current_time
            
            # 目を閉じてからの経過時間を計算
            elapsed_seconds = current_time - self.eye_closed_start_time
            status = "Eyes Closed"
            
            # 閾値秒数以上閉じていたら睡眠状態と判定
            if elapsed_seconds >= self.SLEEP_THRESHOLD:
                self.is_sleeping = True
                status = "Sleeping"
            
            return self.is_sleeping, elapsed_seconds, status
        else:
            # --- 目が開いている場合 ---
            if self.eye_closed_start_time is not None:
                # 直前まで目を閉じていた場合（猶予時間の判定に入る）
                if self.eye_opened_start_time is None:
                    self.eye_opened_start_time = current_time  # 猶予タイマースタート
                
                grace_elapsed = current_time - self.eye_opened_start_time
                elapsed_seconds = current_time - self.eye_closed_start_time
                
                if grace_elapsed > self.GRACE_PERIOD:
                    # 猶予時間を超えたら、完全にタイマーをリセット
                    self.eye_closed_start_time = None
                    self.eye_opened_start_time = None
                    self.is_sleeping = False
                    elapsed_seconds = 0.0
                    status = "Eyes Open"
                else:
                    # まだ猶予時間内
                    status = f"Grace Period ({grace_elapsed:.1f}s)"
            else:
                # ずっと目を開けている場合
                self.is_sleeping = False
                elapsed_seconds = 0.0
                status = "Eyes Open"
            
            return self.is_sleeping, elapsed_seconds, status


def main():
    """
    メイン処理（Blendshapes版 + 猶予時間ロジック）
    
    睡眠検出プログラム:
    - MediaPipeのBlendshapesを使用した高精度な目閉じ検出
    - eyeBlinkLeft, eyeBlinkRightの値を使用
    - 3秒間目を閉じていたら睡眠状態と判定
    - 1秒間の猶予時間でまばたきを誤検知しない
    """
    
    # 睡眠検出器の初期化（猶予時間1秒）
    detector = SleepDetectorBlendshapes(sleep_threshold=3.0, grace_period=1.0)
    
    # 公式準拠: BaseOptionsの設定
    BaseOptions = mp.tasks.BaseOptions
    FaceLandmarker = mp.tasks.vision.FaceLandmarker
    FaceLandmarkerOptions = mp.tasks.vision.FaceLandmarkerOptions
    VisionRunningMode = mp.tasks.vision.RunningMode
    
    # 公式準拠: LIVE_STREAMモードの設定（Blendshapes有効）
    options = FaceLandmarkerOptions(
        base_options=BaseOptions(model_asset_path=detector.model_path),
        running_mode=VisionRunningMode.LIVE_STREAM,  # リアルタイム処理モード
        num_faces=1,  # 検出する顔の最大数
        min_face_detection_confidence=0.5,  # 顔検出の信頼度閾値
        min_face_presence_confidence=0.5,   # 顔の存在判定の信頼度閾値
        output_face_blendshapes=True,       # 表情変化（Blendshapes）を有効化
        result_callback=detector.resultCallback  # 検出結果を受け取るコールバック関数
    )
    
    # カメラの初期化
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("Error: Could not open camera")
        return
    
    # 公式推奨: コンテキストマネージャーを使用（自動的にリソースを解放）
    with FaceLandmarker.create_from_options(options) as landmarker:
        print("Face Landmarker initialized with Blendshapes")
        
        frame_count = 0  # フレームカウンター
        start_time = time.time()  # プログラム開始時刻
        
        # メインループ（カメラ映像を処理）
        while True:
            # カメラから1フレーム読み込み
            ret, frame = cap.read()
            if not ret:
                print("Error: Failed to capture frame")
                break
            
            # 画像を左右反転（鏡像表示）
            frame = cv2.flip(frame, 1)
            
            # BGRからRGBに色空間を変換（MediaPipeはRGB形式を要求）
            rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            
            # MediaPipe Imageオブジェクトに変換
            mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=rgb_frame)
            
            # タイムスタンプを計算（ミリ秒単位）
            timestamp_ms = int((time.time() - start_time) * 1000)
            
            # 公式推奨: 非同期検出（LIVE_STREAMモード）
            landmarker.detect_async(mp_image, timestamp_ms)
            
            # 最新の検出結果を処理して睡眠判定
            is_sleeping, elapsed_seconds, status = detector.processResult()
            
            # Blendshapes値を取得（画面表示用）
            left_blink, right_blink, avg_blink = detector.getEyeBlinkValues()
            
            # --- ステータスに応じた色分け表示 ---
            if status == "Sleeping":
                color = (0, 0, 255)  # 赤: 睡眠中
            elif "Grace Period" in status:
                color = (255, 255, 0)  # 青緑: 猶予期間
            elif status == "Eyes Closed":
                color = (0, 255, 255)  # 黄: 目閉じ
            elif status == "Eyes Open":
                color = (0, 255, 0)  # 緑: 目開き
            else:  # "No Face"
                color = (128, 128, 128)  # 灰: 顔なし
            
            # 顔が検出されているかチェック
            if detector.latest_result is None or not detector.latest_result.face_landmarks:
                # 顔が検出されていない場合
                cv2.putText(frame, "No face detected", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.5, color, 3)
            else:
                # 顔が検出された場合 - ステータスと経過秒数を表示
                cv2.putText(frame, f"Status: {status}", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.2, color, 2)
                cv2.putText(frame, f"Timer: {elapsed_seconds:.1f}s / {detector.SLEEP_THRESHOLD:.1f}s", (10, 95), cv2.FONT_HERSHEY_SIMPLEX, 0.8, color, 2)
                
                # Blendshapes情報を表示
                cv2.putText(frame, f"Left Eye: {left_blink:.3f}", (10, 135), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
                cv2.putText(frame, f"Right Eye: {right_blink:.3f}", (10, 165), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
                cv2.putText(frame, f"Average: {avg_blink:.3f}", (10, 195), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)
                
                # 閾値ライン表示
                cv2.putText(frame, f"Threshold: {detector.BLINK_THRESHOLD:.1f}", (10, 225), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (128, 128, 128), 2)
            
            # フレームカウンターを表示
            frame_count += 1
            cv2.putText(frame, f"Frame: {frame_count}", (10, 255), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (200, 200, 200), 1)
            
            # 画面に表示
            cv2.imshow("Sleep Detection (Blendshapes Edition)", frame)
            
            # 'q'キーで終了
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
    
    # 公式推奨: コンテキストマネージャーで自動的にリソースを解放
    cap.release()
    cv2.destroyAllWindows()
    print("\nProgram terminated")


if __name__ == '__main__':
    main()