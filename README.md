# 개발환경
nvm 설치 후 nvm install --lts
node기반의 개발 예정

# 작업 순서

## 1. tcp서버 구현
nodejs 소켓서버 사용해 완료
## 2. 명령어 파싱
tcp서버에서 사용할 명령어와 파싱법
테스트용 명령어 7,8,9(GPIO 해당 핀)
실제 구현용 L(Left), R(Right), F(Forward), B(Backward)
명령어(공백)숫자(0~100)형태로 메시지 보낼 것
***
pc에서 가능/불가능
***
## 3. 명령어 통해 pwm제어
해당 tcp 서버를 통해 raspi led 밝기 조절 되는 것 확인
## 4. 라즈베리파이 ap로
https://d-tail.tistory.com/5
https://blog.naver.com/PostView.nhn?blogId=hahav000&logNo=221342472577&categoryNo=9&parentCategoryNo=0&viewDate=&currentPage=1&postListTopCurrentPage=1&from=postView&userTopListOpen=true&userTopListCount=5&userTopListManageOpen=false&userTopListCurrentPage=1

***
모듈 오기 전/후 가능여부
***
## 5. Attitude 불러오기
모듈로부터 attitude 받아오기
## 6. 계산
attitude 통해 yaw,pitch,angle 계산하는 부분 아두이노로 부터 포팅
## 7. 전달
MSP통해서 FC보드로 계산한 값 전달
## 8. 포팅
raspi 3b+ 에서 이루어진 작업임. 이를 zero에서 해보기

