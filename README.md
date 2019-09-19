# 소프트웨어 사용법
1. micro sd에 라즈비안 플래싱 후 라즈베리파이 부팅
2. wifi를 통해 인터넷으로 연결해주세요  
3. sudo apt-get update  
sudo apt-get install git 명령어로 git 설치해주세요
4. git clone https://github.com/keenranger/dr.one.git 으로 파일 내려받아 주세요  
===
여기까진 이미 되어있습니다.
===
5. cd dr.one 명령어로 폴더 안으로 들어가주세요
6. python3 main.program.py 입력하시면 프로그램이 실행됩니다.
7. 처음에 명령어를 주고, 서버가 켜집니다.  
그 후 fc보드와 통신을 시작하고, 아두이노에게 명령을 줄 수 있게 됩니다.
8. 파이의 ip주소는 ifconfig를 통해 확인하실 수 있습니다.  
플레이스토어에서 Telnet이라고하는 어플리케이션을 다운로드 후  
ip주소란에 파이의 주소, port란에 23452를 주시면 연결이 성립됩니다.  
9. Pi에서 도는 프로그램을 멈추시기 위해서는 Ctrl+c를 두번 눌러주시면 됩니다.  
간혹 비정상적으로 종료가 되면 포트가 닫히지 않아 다음 실행시 포트를 열지못해 문제가 생길 수 있습니다. 2~3분정도 기다리시면 포트가 닫힙니다.

# 명령어 요약
기본적으로 roll pitch yaw throttle 각각 1500씩을 줍니다.  
각 값의 최솟값은 1000, 최댓값은 2000입니다.  
a.을 입력하시면 아밍이됩니다. 비행 중에 a.을 입력하셔서 긴급 정지가 가능합니다.  
s.을 입력하시면 모든 값에 1500을 줍니다.  
u.숫자(0에서 100사이)을 입력하시면 throttle을 1500+숫자X5로 합니다.  
d.숫자(0에서 100사이)을 입력하시면 throttle을 1500+숫자X5로 합니다.  
l.숫자(0에서 100사이)을 입력하시면 roll 1500-숫자X5로 합니다.  
r.숫자(0에서 100사이)을 입력하시면 roll 1500+숫자X5로 합니다.  
  
  drone_calcuilation.py의 command_to_ppm_command 함수가 이 부분의 역할을 수행합니다.
  
  
  
  
  
  
# 참고자료
쓰레드 동기화 : https://niceman.tistory.com/139
# SPRacingF3
보드 정보 : https://cleanflight.readthedocs.io/en/latest/Board%20-%20SPRacingF3/#pinouts  

# 데이터 통신 구성
라파 파이썬에서 지속적으로 attitude읽어오며, 명령어를 msp통해 fc보드로 전송  
쓰로틀에 따라 bldc 제어를 위해 아두이노와 i2c통신을 함.
아두이노에서는 받은 정보를 통해 2개의 pwm out  
라파에서는 서버가 열려있어서 클라이언트가 보내는 메시지를 확인할 수 있어야함  

# 작업 순서

## ~~1. tcp서버 구현 -> 완료~~
파이썬 소켓서버 사용해서 완료
## ~~2. 명령어 파싱 -> 완료~~
tcp서버에서 사용할 명령어와 파싱법  
~~테스트용 명령어 7,8(GPIO 해당 핀)~~  
실제 구현용 L(Left), R(Right), F(Forward), B(Backward)  

명령어.숫자(0~100)형태로 메시지 보낼 것  
## 3. 명령어 통해 pwm제어
해당 tcp 서버를 통해 raspi led 밝기 조절 되는 것 확인
## 4. 라즈베리파이 ap로
https://d-tail.tistory.com/5
https://blog.naver.com/PostView.nhn?blogId=hahav000&logNo=221342472577&categoryNo=9&parentCategoryNo=0&viewDate=&currentPage=1&postListTopCurrentPage=1&from=postView&userTopListOpen=true&userTopListCount=5&userTopListManageOpen=false&userTopListCurrentPage=1

## 5. ~~Attitude 불러오기 -> 완료~~  
모듈로부터 attitude 받아오기
## 6. 계산
attitude 통해 yaw,pitch,angle 계산하는 부분 아두이노로 부터 포팅
## 7. 전달
MSP통해서 FC보드로 계산한 값 전달
## 8. 포팅
raspi 3b+ 에서 이루어진 작업임. 이를 zero에서 해보기

