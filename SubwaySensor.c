#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringSerial.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

// GPIO
#define TRIG 17 // 초음파 센서 TRIG핀
#define ECHO 27 // 초음파 센서 ECHO핀
#define LED_RED 23
#define LED_GREEN 24
#define SPI_CHANNEL 0  // SPI 채널 (CEO 사용)
#define SPI_SPEED 1000000  // 1MHz 속도 (SPI 데이터 전송 속도)
#define LM35_CHANNEL 0  // MCP3008의 0번 채널 사용
//#define FILE_PATH "/home/seojoon/nodetest/sensor_data.csv"

#define UART_DEVICE "/dev/ttyAMA0" // 라즈베리파이 UART 포트

#define DISTANCE_THRESHOLD 20.0 // 거리 기준값
#define TEMPERATURE_THRESHOLD 30.0 // 온도 기준값
#define PRESSURE_THRESHOLD 10.0  // 압력 기준값

int serial_fd; // UART 파일 디스크립터

typedef struct // 센서 데이터 저장할 구조체
{
	float distance;
	float temperature;
	float pressure;
} SensorData;

// GPIO 안전하게 리셋하는 함수
void CleanUp(int signum)
{
    printf("GPIO 핀번호 리셋 \n");
    digitalWrite(TRIG, LOW);

    pinMode(TRIG, INPUT);
    pinMode(ECHO, INPUT);
    pinMode(LED_RED, INPUT);
    pinMode(LED_GREEN, INPUT);

    printf("센서 안전하게 종료! \n");
    exit(0);
}

// 핀출력(GPIO, SPI) 설정
void SetUp()
{
	if (wiringPiSetupGpio() == -1 || wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) == -1) {
        fprintf(stderr, "SPI 초기화  or GPIO 초기화 실패!\n");
        exit(1);
    }
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);

    digitalWrite(TRIG, LOW);
    fprintf(stderr, "초음파 센서 및 온도 센서 준비 완료 \n");

    signal(SIGINT, CleanUp); // Ctrl + C 입력 시 CleanUp 함수 실행
}

// 거리 센서
void GetDistance(SensorData* data)
{
    struct timeval start, stop;
    long elapsed_time;

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    while (digitalRead(ECHO) == LOW);
    gettimeofday(&start, NULL);

    while (digitalRead(ECHO) == HIGH);
    gettimeofday(&stop, NULL);

    elapsed_time = (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec);
    data -> distance = (elapsed_time * 0.0343) / 2; // 단위 : cm
}

// 온도 센서
void GetTemperature(SensorData* data){
    uint8_t buffer[3];
    int adc_value;
    float voltage;
	
    buffer[0] = 1; // MCP3008 시작 비트
    buffer[1] = (8 + LM35_CHANNEL) << 4; // 채널 선택
    buffer[2] = 0;
    
    wiringPiSPIDataRW(SPI_CHANNEL, buffer, 3);
    
     // 10비트 ADC 값 변환
    adc_value = ((buffer[1] & 3) << 8) + buffer[2];

    // 전압 변환 (3.3V 기준)
    voltage = (adc_value * 3.3) / 1023.0;

    // 온도 변환 (LM35는 10mV/C)
    data -> temperature = voltage * 100.0;
}

// 데이터 파일 저장
/*
void SaveData(float distance, float temperature){
	FILE *file = fopen(FILE_PATH, "a");
	if (file == NULL) {
		fprintf(stderr, "파일 열기 실패! \n");
		return;
	}
	//fprintf(stderr, "SaveData() 호출! : %.2f, %.2f\n" ,distance ,temperature);
	fprintf(file, "%.2f, %.2f\n" ,distance ,temperature);
	fclose(file);
} */

// UART 초기화 함수
void InitUART()
{
    if ((serial_fd = serialOpen(UART_DEVICE, 115200)) < 0) {
        fprintf(stderr, "UART 열기 실패\n");
        exit(1);
    }
    //printf("UART 통신 연결됨\n");
}

// STM32에서 Pressure값만 추출
void ReadPressureFromSTM32(SensorData* data)
{
    char buffer[64];  // UART 데이터 저장 버퍼
    int i = 0;
	
	serialFlush(serial_fd); // 이전 데이터가 남아있을 수 있으므로 UART 버퍼 비우기
	//memset(buffer, 0, sizeof(buffer));  // 버퍼 초기화
	
    // UART 데이터 수신
    while (1) {
        if (serialDataAvail(serial_fd)) {
            char c = serialGetchar(serial_fd);
            if (c == '\n' || i >= sizeof(buffer) - 1) {  // 개행 문자가 오면 종료
                buffer[i] = '\0';
                break;
            }
            buffer[i++] = c;
        } else {
            usleep(1000);  // 데이터를 기다리기 위해 약간의 딜레이 추가
        }
    }
    // 수신된 데이터 확인 (디버깅)
    //printf("UART 수신 데이터 : %s\n", buffer);

    // "Pressure: xxx.xx kg" 부분 찾기
    char *pressureStr = strstr(buffer, "Pressure:");
    if (pressureStr) {
        sscanf(pressureStr, "Pressure: %f", &data -> pressure);
    }
}

// LED 상태 업데이트
void UpdateLED(SensorData* data)
{
	if ((data -> distance < DISTANCE_THRESHOLD || data -> temperature > TEMPERATURE_THRESHOLD) && data -> pressure > PRESSURE_THRESHOLD) {
		digitalWrite(LED_RED, HIGH);
		digitalWrite(LED_GREEN, LOW);
	} else {
		digitalWrite(LED_RED, LOW);
		digitalWrite(LED_GREEN, HIGH);
	}
}

// 메인 실행 루프
int main()
{
    SetUp();
    InitUART();
    
    static SensorData sensorData; // 정적 할당
   
    while (1)
    {
	struct timeval start_time, end_time;
	gettimeofday(&start_time, NULL); // 시작 시간 측정
		
        GetDistance(&sensorData);
        GetTemperature(&sensorData);
        ReadPressureFromSTM32(&sensorData);
        
        //printf("압력 값 수신: %.2f kg\n", pressure);
        
        //SaveData(distance, temperature); // 파일 저장 
        
	// 거리 및 온도 데이터 JSON 형식으로 출력
        printf("{\"distance\" : %.2f, \"temperature\" : %.1f, \"pressure\": %.2f}\n", sensorData.distance, sensorData.temperature, sensorData.pressure);
        fflush(stdout);     // 버퍼를 즉시 비우면서 출력을 강제로 수행 -> printf문 전부 출력

        UpdateLED(&sensorData); // LED 활성
        
        gettimeofday(&end_time, NULL); // 끝난 시간 측정
        
        // 수행 시간 계산 (마이크로초 -> 초 변환)
        long elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000 + 
                            (end_time.tv_usec - start_time.tv_usec);
        long remaining_time = 5000000 - elapsed_time; // 5초 
        
        // 남은 시간이 있으면 대기 (5초가 될때까지)
        if (remaining_time > 0)
        {
		usleep(remaining_time); // 5초에서 실행 시간을 뺀 만큼 대기
        }
	}
    return 0;
}
