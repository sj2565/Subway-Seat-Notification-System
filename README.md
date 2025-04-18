# 지하철 좌석 알림 시스템

## 1. 프로젝트 소개
각종 센서(거리, 온도, 압력)를 기반으로 특정 조건을 만족할 시 빈좌석 혹은 좌석 점유인지 웹에서 확인 가능하도록 <br>설계했으며
빈좌석은 초록색, 좌석 점유는 빨간색으로 직관적이게 표현. <br>
거리센서와 온도센서는 Raspberry Pi로, 압력센서는 STM32 Cortex-M4시리즈로 동작. <br>
센서 데이터를 전송 및 전달 받기 위해 GPIO 입출력, UART, SPI 통신 활용. <br>
이외에도 데이터 값들을 기반으로 머신러닝 기법 중 하나인 IsolationForest를 적용하여 학습시켰으며 실시간으로 <br>이상값 탐지 가능. <br>

## 2. 프로젝트 이미지

<img width="450" alt="작품사진" src="https://github.com/user-attachments/assets/043153a3-3059-4a3f-bdfa-932dab903b04">

## 3. 프로젝트 회로도

<img width="850" alt="지하철 회로도" src="https://github.com/user-attachments/assets/6d929607-3185-4ba5-b549-eba194ff2b51">

## 4. 활용된 기술

### ⚙ 임베디드 시스템
- **Raspberry Pi 4 Model B** (센서 데이터 처리 및 SPI통신)
- **STM32 Black Pill (STM32F411CEU6)** (압력센서 감지 및 UART통신)
- **C (WiringPi)** (Raspberry Pi에서 센서 제어)

### 📡 통신
- **UART** (STM32 ↔ Raspberry Pi 시리얼 통신)
- **SPI** (Raspberry Pi 데이터 전송)

### 🌐 프론트엔드
- **HTML, CSS, JavaScript** (좌석 상태 그래프 시각화)

### 💻 서버 및 백엔드
- **Node.js** (JavaScript 런타임)
- **Express.js** (웹 서버 프레임워크)
- **WebSocket (socket.io)** (실시간 데이터 전송)

### 👩‍💻 머신러닝
- **Python** (모델 설계)
- **Isolation Forest** (이상값 탐지)

### 🛠 IDE
- **Visual Studio**
- **Python3 IDLE**
- **STM32cubeIDE**

## 5. 프로젝트 실행 이미지
<img width="330" alt="빈좌석" src="https://github.com/user-attachments/assets/2745f2a7-9344-44ce-b4c8-71d6fbafec09" /><p>&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;빈좌석 이미지</p>
<img width="330" alt="좌석점유" src="https://github.com/user-attachments/assets/796346cc-e048-4780-8966-c9bb245755ea" />
<img width="330" alt="이상좌석" src="https://github.com/user-attachments/assets/30dfd44f-be11-4f61-9228-3ffd9485f989" /> <br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;좌석점유 이미지 &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;이상값 좌석 이미지<br><br><br> 
<img width="800" alt="이상값" src="https://github.com/user-attachments/assets/732a0738-6e32-4dbf-b688-ed0406b4a4d9" /> <br> AI 모델 이상값 탐지 : distance가 1213.4로 측정되어 이상값이라 판단.

## 6. 설치 및 실행
**C프로그램 컴파일**
```bash
# 라즈베리파이에서 컴파일 진행
gcc -o SubwaySensor SubwaySensor.c -lwiringPi
```
**Python 라이브러리 설치**
```bash
python3 -m pip install numpy pandas scipy scikit-learn

# pip 설치 안될 시 가상환경을 통해서 설치
python3 -m venv 설정할 이름

# 가상환경 활성화
source 설정할 이름/bin/activate
```

**Node 서버 가동 및 실행**
```bash
node SubwayServer
```
## 7. 향후 개선할 점
+ C -> Node(서버)로 보낼 때 단순 버퍼 형식이 아니라 UART나 TCP/UDP 같은 통신 활용.
+ AI 모델 평가를 위해 정확도 및 손실함수를 적용.
+ LM35(온도센서)에 디지털 신호 처리(DSP) 연습 겸 적용.
-------------------------------------------------------------------------------------
고려해 볼 만한 사항
+ CRC를 추가하여 비트 검사.
+ 오프라인 모드를 추가하여 네트워크가 끊기더라도 최근 좌석 상태를 로컬에서 유지하도록 개선.
+ Isolation Forest 외에 다른 모델 추가 (Ensemble Learning).
