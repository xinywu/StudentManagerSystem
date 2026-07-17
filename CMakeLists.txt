cmake_minimum_required(VERSION 3.19)

project(StudentManagerSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt
find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Widgets
    Sql
)

# OpenCV
# OpenCVConfig.cmake 实际所在目录
set(OpenCV_DIR "C:/app/opencv/build/x64/vc16/lib")
find_package(OpenCV REQUIRED)

# SeetaFace6 根目录
set(SEETAFACE_DIR "C:/app/seetaface/sf6.0_windows")
set(SEETAFACE_LIB_DIR "${SEETAFACE_DIR}/lib/x64")

qt_standard_project_setup()

qt_add_executable(StudentManagerSystem
    WIN32
    MACOSX_BUNDLE

    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    studentdialog.h
    studentdialog.cpp
    studentdialog.ui

    scoremanagementdialog.h
    scoremanagementdialog.cpp
    scoremanagementdialog.ui

    scoredialog.h
    scoredialog.cpp
    scoredialog.ui


    facecollectwindow.h
    facecollectwindow.cpp
    facecollectwindow.ui

    videocapturethread.h
    videocapturethread.cpp

    face_resource.qrc

    inputfilesprocessdialog.h
    inputfilesprocessdialog.cpp
    resetmodelprocessdialog.h
    resetmodelprocessdialog.cpp
    attendancemanagementdialog.h
    attendancemanagementdialog.cpp
    attendancemanagementdialog.ui
    attendancerecorddialog.h attendancerecorddialog.cpp attendancerecorddialog.ui
)

# OpenCV 和 SeetaFace6 头文件
target_include_directories(StudentManagerSystem
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
        "${SEETAFACE_DIR}/include"
)

target_link_libraries(StudentManagerSystem
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql

        # OpenCV 会自动区分 Debug 和 Release
        ${OpenCV_LIBS}

        # SeetaFace6：Debug 库和 Release 库分别链接
        debug "${SEETAFACE_LIB_DIR}/SeetaFaceDetector600d.lib"
        optimized "${SEETAFACE_LIB_DIR}/SeetaFaceDetector600.lib"

        debug "${SEETAFACE_LIB_DIR}/SeetaFaceLandmarker600d.lib"
        optimized "${SEETAFACE_LIB_DIR}/SeetaFaceLandmarker600.lib"

        debug "${SEETAFACE_LIB_DIR}/SeetaFaceAntiSpoofingX600d.lib"
        optimized "${SEETAFACE_LIB_DIR}/SeetaFaceAntiSpoofingX600.lib"

        debug "${SEETAFACE_LIB_DIR}/SeetaFaceTracking600d.lib"
        optimized "${SEETAFACE_LIB_DIR}/SeetaFaceTracking600.lib"

        debug "${SEETAFACE_LIB_DIR}/SeetaFaceRecognizer610d.lib"
        optimized "${SEETAFACE_LIB_DIR}/SeetaFaceRecognizer610.lib"

        debug "${SEETAFACE_LIB_DIR}/SeetaQualityAssessor300d.lib"
        optimized "${SEETAFACE_LIB_DIR}/SeetaQualityAssessor300.lib"

        debug "${SEETAFACE_LIB_DIR}/SeetaPoseEstimation600d.lib"
        optimized "${SEETAFACE_LIB_DIR}/SeetaPoseEstimation600.lib"
)

# 屏蔽旧版 SeetaFace 头文件的编码警告 C4828
if(MSVC)
    target_compile_options(StudentManagerSystem PRIVATE /wd4828)
endif()

install(TARGETS StudentManagerSystem
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET StudentManagerSystem
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
