# Windows 설정
- MSVC 설치 (아마도 Visual Studio 설치 - C++를 사용한 데스크톱 개발 워크로드 설정)
- OpenCV 설치 (https://github.com/opencv/opencv/releases)
- GStreamer 설치 (https://gstreamer.freedesktop.org/download/#windows)
    - MSVC 64-bit (VS 2019, Release CRT) : runtime installer & development installer
    - 환경변수 설정 : C:\Program Files\gstreamer\1.0\msvc_x86_64\bin
 
# 파일 설명

1. **main.cpp**, **MainWindow.cpp**, **MainWindow.h**  
   * 애플리케이션의 시작점(`main.cpp`)과 메인 창(`MainWindow`)입니다.  
   * `MainWindow`는 `VideoThread`와 `TcpThread`를 생성하고 실행하여 전체적인 동작을 관리합니다.  
   * `VideoThread`로부터 잘라낸 이미지(`QPixmap`)를 받으면, 동적으로 `QLabel` 위젯을 생성하여 화면에 표시합니다.  

2. **VideoThread.cpp**, **VideoThread.h**  
   * GStreamer 라이브러리를 사용하여 `rtsp://192.168.0.85:8554/test` 주소의 비디오 스트림을 수신합니다.  
   * 수신한 비디오 프레임을 OpenCV를 이용해 처리할 수 있도록 `cv::Mat` 형식으로 변환합니다.  
   * `TcpThread`가 공유 데이터(`Coordinate`)에 저장한 좌표를 이용해 비디오 프레임의 특정 영역을 잘라냅니다.  
   * 잘라낸 이미지를 `QPixmap`으로 변환한 후 `cropped` 시그널을 통해 `MainWindow`로 전송합니다.  

3. **TcpThread.cpp**, **TcpThread.h**  
   * `192.168.0.85:12345` 주소의 TCP 서버에 접속합니다.  
   * 서버로부터 JSON 형식의 데이터를 수신합니다. 이 데이터에는 비디오를 잘라낼 좌표 정보(`parsing width`)와 화자 정보(`speaker`)가 포함되어 있습니다.  
   * 수신한 데이터를 파싱하여 `Coordinate` 구조체에 저장함으로써 `VideoThread`와 데이터를 공유합니다.  

4. **Coordinate.h**  
   * `TcpThread`와 `VideoThread` 사이에서 데이터를 안전하게 공유하기 위한 구조체입니다.  
   * `QMutex`를 포함하여, 여러 스레드가 동시에 데이터에 접근할 때 발생할 수 있는 문제를 방지합니다.  

5. **CMakeLists.txt**  
   * 프로젝트의 빌드 방법을 정의하는 파일입니다.  
