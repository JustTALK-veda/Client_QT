cmake_minimum_required(VERSION 3.16)

project(mytest_cut VERSION 0.1 LANGUAGES CXX)

if(APPLE)
    set(MACOSX_BUNDLE TRUE)
    set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-video-1.0)
pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)

set(OpenCV_DIR "C:/opencv/build") #window opencv 경로
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


include_directories(
    ${Qt6Widgets_INCLUDE_DIRS}
    ${GST_INCLUDE_DIRS}
    ${GSTREAMER_APP_INCLUDE_DIRS}
)
link_directories(
    ${GST_LIBRARY_DIRS}
    ${GSTREAMER_APP_LIBRARY_DIRS}
)

set(PROJECT_SOURCES
        main.cpp
        Coordinate.h
        TcpThread.h
        TcpThread.cpp
        MainWindow.h
        MainWindow.cpp
        VideoThread.h
        VideoThread.cpp

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(mytest_cut
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    # qt_add_executable(mytest_cut
    #       ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET vedafinal APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(mytest_cut SHARED$
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(mytest_cut
            ${PROJECT_SOURCES}
        )
    endif()
endif()

if(APPLE) #apple설정
    add_custom_command(TARGET mytest_cut POST_BUILD
        COMMAND codesign --force --deep --sign - "${CMAKE_BINARY_DIR}/mytest_cut.app"
    )
endif()

target_link_libraries(mytest_cut PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${GST_LIBRARIES} ${GSTREAMER_APP_LIBRARIES}) #${GST_LIBRARIES} 이거 추가
target_link_libraries(mytest_cut PRIVATE ${OpenCV_LIBS})
target_link_libraries(mytest_cut PRIVATE Qt6::Core Qt6::Network)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.mytest_cut)
endif()
set_target_properties(mytest_cut PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS mytest_cut
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(mytest_cut)
endif()
