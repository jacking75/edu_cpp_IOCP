# IOCP 실습
코드에 버그가 있을 수 있다.(버그를 잡는 것도 공부!)    
Modern C++을 적극적으로 사용하는 것을 추천한다.  
  
  
## 실습 계획
참석자의 실력에 따라 편차가 있겠지만 기본적으로 1회 모임에 2단계씩 진행을 한다.  
- 모임에서 직접 구현하기
- 다음 모임까지 해야할 과제(과제를 구현한 코드는 다음에 모임에 배포 예정)  
    - 과제로 구현할 기능을 이미 구현한 코드를 보여주면서 설명을 해줌(이렇게 해야 명확하게 무엇을 구현해야 할지 알 수 있을테니)  
  
### 1회 모임  
1 단계. Echo 서버 코드 이해  

IOCP에 대해서 설명한다.
2단계 기능을 과제로 낸다.  
  
  
### 2회 모임 
2 단계. 애플리케이션과 네트워크 코드 분리하기  
    - 연결, 끊어짐, 데이터 받음을 애플리케이션에 전달하기  
3 단계. 네트워크와 로직 처리를 스레드 분리하기  
    - Send를 Recv와 다른 스레드에서 하기  
    - send를 연속으로 보낼 수 있는 구조가 되어야 한다.  
  
2단계 기능을 잘 구현했는지 확인한다.
3단계 기능을 실습으로 만들어 본다.  
4단계 기능을 과제로 낸다.  
  
  
### 3회 모임 
4. 1-Send 구현하기  
    - 버퍼에 쌓아 놓고, send 스레드에서 보내기.   
5. 1-Send 구현하기  
    - queue에 담아 놓고 순차적으로 보내기.    
  
4단계 기능을 잘 구현했는지 확인한다.  
5단계 기능을 실습으로 만들어 본다.  
6단계 기능을 과제로 낸다.  
  
   
### 4회 모임 
6. Accept 비동기로 바꾸기    
7. 채팅 서버 만들기 - 패킷 구조 사용하기, 로그인
      
6단계 기능을 잘 구현했는지 확인한다.  
7단계 기능을 실습으로 만들어 본다.  
8단계 완료를 과제로 낸다.    
  
  
### 5회 모임
8. 채팅 서버 만들기 - 로그인 때 Redis 사용하기        
9. 채팅 서버 만들기 - 방 입장, 방 나가기, 방 채팅 구현하기          
  
8단계 기능을 잘 구현했는지 확인한다.  
9단계 기능을 실습으로 만들어 본다.    
       
      
        
5회 모임까지가 공식적인 실습 모임이다.  
이 아래 단계는 개별적으로 하는 것을 추천한다.    
    
10 단계. 최적화 하기    
    - GetQueuedCompletionStatusEx 버전 사용하기    
    - 서버에서 사용하는 설정을 입력 받기  
    - 동적 할당을 최소화 하기  
    - 링버퍼 구현 추가(덮어 쓰기 방지)  
	- Lock 사용 범위를 줄이거나 좀 더 가벼운 Lock 사용하기
	- 더미 클라이언트로 테스트 하기   
  
11 단계. Network, Content, Host 각 레이어로 프로젝트 나누기  
    - Network, Content는 각각 정적 라이브러리로 만든다.  
    - Host는 콘솔 프로젝트. Network, Content 라이브러리를 사용한다.  
  
  
  
  
## 참고 글 모음
- [IOCP 기본 구조 이해](https://www.slideshare.net/namhyeonuk90/iocp )
- [IOCP에 대하여](https://www.joinc.co.kr/w/Site/win_network_prog/doc/iocp )
- [IOCP (I/O CompletionPort)](https://chfhrqnfrhc.tistory.com/entry/IOCP )
- [IOCP 정리](https://hmjo.tistory.com/159 )
- [IOCP](https://blog.naver.com/zzangrho/80150515226 )
- [IOCP 모델](https://blog.naver.com/handodos/140138259592 )
- [IOCP 모델 - 구조의 이해](https://zxwnstn.blog.me/221513630216 )
- [서버 모델 - 윈도우 IOCP](https://dev-ahn.tistory.com/114 )
- [IOCP 내용 정리](https://blog.naver.com/dkdldhekznal/221233789231 )
- [게임 서버를 제작하기 위한 IOCP 네트워크 클래스 만들기 Ver1](https://blog.naver.com/dkdldhekznal/221235469866 )
- [게임 서버를 제작하기 위한 IOCP 네트워크 클래스 만들기 Ver2](https://blog.naver.com/dkdldhekznal/221242411793 )
  
