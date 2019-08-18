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

