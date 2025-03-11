# Subway-Seat-Notification-System
수정하기

향후 개선할 점
1. C -> Node(서버)로 보낼 때 단순 버퍼 형식이 아니라 UART나 TCP/UDP로 활용
2. AI 모델 평가를 위해 정확도 및 손실함수를 적용
3. 전송 데이터를 문자열 -> 바이너리로 바꿔서 데이터 크기 감소 및 전송 속도 향상
4. 메모리 관리를 위해 직접 할당
-------------------------------------------------------------------------------------
고려해 볼 만한 사항
1. CRC를 추가하여 비트 검사
2. 데이터베이스를 설계하여 데이터를 수집한 후 좌석 빈도수 AI 모델을 설계해서 혼잡도 확인


어떻게 실행하는지 코드로 작성

✅ 1️⃣ 데이터 전처리가 필요한 이유
센서 데이터에서 발생할 수 있는 문제점
문제	설명	해결 방법
노이즈 (Noise)	초음파 센서나 온도 센서가 순간적으로 잘못된 값을 측정할 수 있음	이동 평균(SMA) 필터 적용
결측값 (Missing Data)	센서 오류로 인해 데이터가 빠져 있을 수 있음	평균값 또는 최근 값으로 대체
이상값 (Outlier)	사람이 없는데 온도가 급격히 상승하는 등 비정상적인 데이터	Isolation Forest로 자동 감지
데이터 스케일링	거리(cm)와 온도(°C)의 단위가 다름	StandardScaler 또는 MinMaxScaler 적용
💡 전처리를 하면 이상값을 더 정확히 감지할 수 있음! 🚀


2. Isolation Forest 외에 다른 모델 추가 (Ensemble Learning)
💡 왜 필요할까?

Isolation Forest만 사용하면 오탐(False Positive) 발생 가능
다른 모델과 결합하면 더 정밀한 감지 가능
📌 One-Class SVM 추가하여 비교 (ai_detector.py 수정)
✅ Isolation Forest + One-Class SVM 결합하여 더 정밀한 이상 감지
✅ 오탐(False Positive) 줄이고, 진짜 이상값만 탐지 가능

1. Isolation Forest + LOF 앙상블 적용 (이상 탐지 정확도 향상)
2. 모델 성능 평가 추가 (AUC-ROC, Precision-Recall)
3. 하이퍼파라미터 튜닝 작업
param_grid = {
    "n_estimators": [100, 200, 300],  # 트리 개수 조절
    "contamination": [0.01, 0.05, 0.1],  # 이상 탐지 비율 튜닝
    "max_samples": [128, 256, 512]  # 샘플 크기 조절
}
