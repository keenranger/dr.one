# 개발환경
nvm 설치 후 nvm install --lts
node기반의 개발 예정

# 작업 순서

## 1
tcp서버 구현 -> 종료
## 2
tcp서버에서 사용할 명령어와 파싱법
테스트용 명령어 7,8,9(GPIO 해당 핀)
실제 구현용 L(Left), R(Right), F(Forward), B(Backward)
명령어(공백)숫자(0~100)형태로 메시지 보낼 것
***
pc에서 가능/불가능
***
## 3
해당 tcp 서버를 통해 raspi led 밝기 조절 되는 것 확인
## 4
라즈베리파이 ap로 사용
***
모듈 오기 전/후 가능여부
***
## 5
모듈로부터 attitude 받아오기
## 6
attitude 통해 yaw,pitch,angle 계산하는 부분 아두이노로 부터 포팅
## 7
MSP통해서 FC보드로 계산한 값 전달
## 8
raspi 3b+ 에서 이루어진 작업임. 이를 zero에서 해보기

