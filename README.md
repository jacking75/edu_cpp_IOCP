# 단계 별로 IOCP 실습
- 코드에 버그가 있을 수 있다.(버그를 잡는 것도 공부!)    
- Modern C++을 적극적으로 사용하는 것을 추천한다.  
- 'Tutorial/ChatServerWithLogger' 있는 프로젝트는 채팅 서버에 plog 라이브러리를 사용하고 있으며, Windows Event Log에 로그 데이터를 기록할 수 있다.     
- Visual Studio 2022를 사용한다(하위 버전으로 변경해도 코드 변경은 없다)
- [YOUTUBE](https://www.youtube.com/watch?v=RMRsvll7hrM&list=PLW_xyUw4fSdbYjgwC-JCCFWznhayrZv77 )  
- [학습 가이드](./iocp_learning_guide.md)
    
<br>  
  
  
## 실습 단계
실습을 하기 전에 꼭 IOCP에 대해 학습을 한다.  
IOCP는 오래된 기술로 서적에서 다루기도 했고, 특히 네이버에서 검색해도 많은 자료를 찾을 수 있다(네이버 검색하면 한글 자료)     
- 1 단계. 가장 간단한 IOCP 서버. Echo 서버 코드 만들기  
    - [설명](https://claude.ai/artifacts/ce2d7d86-47a1-49dd-b3e4-f3966baaf4e7 )
    - IOCP API 동작에 대해서 이해할 수 있다.
- 2 단계. OverlappedEx 구조체에 있는 `char m_szBuf[ MAX_SOCKBUF ]`를 stClientInfo 구조체로 이동 및 코드 분리하기
    - [설명](https://claude.ai/artifacts/944015d4-c1f3-4f73-8792-024d8bee632e )
    - 앞 단계에는 OverlappedEx 구조체에 `m_szBuf`가 있어서 불 필요한 메모리 낭비가 발생함
- 3 단계. 애플리케이션과 네트워크 코드 분리하기  
    - [설명](https://claude.ai/artifacts/add4cfd2-ec6e-4fb0-b2d5-aa16adbc47d5 )
    - IOCP를 네트워크 라이브러리화 하기 위해 네트워크와 서버 로직 코드를 분리한다.
    - 연결, 끊어짐, 데이터 받음을 애플리케이션에 전달하기  
- 4 단계. 네트워크와 로직(패킷 or 요청) 처리 각각의 스레드로 분리하기  
    - [설명](https://claude.ai/artifacts/67fd7863-baac-4d2f-a532-577504dff3e6 )  
    - Send를 Recv와 다른 스레드에서 하기  
    - send를 연속으로 보낼 수 있는 구조가 되어야 한다.  
- 5 단계. 1-Send 구현하기  
    - [설명](https://claude.ai/artifacts/97fd6653-fff4-40a3-bd4a-5789a2eaffa9 )
    - 버퍼에 쌓아 놓고, send 스레드에서 보내기.   
- 6 단계. 1-Send 구현하기  
    - [설명](https://claude.ai/artifacts/29e5c2c2-5f8e-4be8-b9cd-ff305143bc05 )
    - queue에 담아 놓고 순차적으로 보내기.    
- 7 단계. 비동기 Accept 사용하기
    - [설명](https://claude.ai/artifacts/c77f0565-d005-4169-91b6-2c2cddb3084b )
    - 6단계까지는 Accept 처리를 동기 I/O로 했다. 이것을 비동기I/O로 바꾼다. 이로써 네트워크 동작이 모두 비동기 I/O가 된다
    - 6단계에서 이어서 기능을 구현한다.
- 8 단계. 채팅 서버 만들기 (ChatServer_01)
    - [설명](https://claude.ai/artifacts/9cc619fb-5f8a-4b7b-b040-59dbaecefd42 )
    - 패킷 구조 사용하기, 로그인
- 9 단계. 로그인 때 Redis 사용하기 (ChatServer_02)       
    - 설명: 
        - [채팅 서버 클래스 구조 다이어그램](https://claude.ai/artifacts/57796c56-f9b4-46e6-acb2-8483653c8654)
        - [스레드 구조와 메모리 관리 다이어그램](https://claude.ai/artifacts/1f777b14-4063-418f-8584-6b46c083d798 )
        - [프로토콜 처리 및 상태 관리 다이어그램](https://claude.ai/artifacts/7c5dba87-5557-4993-8284-1e7b13547ee7 )
    - C++에서 Redis를 사용하는 방법은 [examples_cpp_redis](https://github.com/jacking75/examples_cpp_redis )를 참고한다.
- 10 단계. 방 입장, 방 나가기, 방 채팅 구현하기 (ChatServer_03)         
    - 설명: 
        - [채팅 서버 아키텍처 다이어그램](https://claude.ai/artifacts/2c11c368-13de-44f3-88cb-d7ec2aea6dd1 )  
        - [클래스 관계도](https://claude.ai/artifacts/21a3b596-58c8-4d42-b1e9-9c83c8bd0e2a )  
        - [패킷 처리 흐름도](https://claude.ai/artifacts/ee1c0e39-351e-4803-9ca0-02c301ec0639 )  
  
  
    
### 더 진행 한다면...    
- 11 단계. 최적화 하기    
    - GetQueuedCompletionStatusEx 버전 사용하기    
    - 서버에서 사용하는 설정을 입력 받기  
    - 동적 할당을 최소화 하기  
    - 링버퍼 구현 추가(덮어 쓰기 방지)  
	- Lock 사용 범위를 줄이거나 좀 더 가벼운 Lock 사용하기
	- 더미 클라이언트로 테스트 하기   
- 12 단계. Network, Content, Host 각 레이어로 프로젝트 나누기  
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
  
