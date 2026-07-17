# Student Manager System

基于 Qt 6 的学生信息管理系统，包含学生信息、成绩、考勤及人脸采集等功能。

> 项目原始完成时间：2026 年 7 月；之后整理并上传至 GitHub。

## 主要功能

- 学生信息管理
- 成绩录入与查询
- 考勤记录与管理
- 摄像头视频采集
- 基于 SeetaFace6 的人脸相关功能

## 开发环境

- C++17
- Qt 6.5 或更高版本（Core、Widgets、Sql）
- CMake 3.19 或更高版本
- OpenCV
- SeetaFace6

当前 `CMakeLists.txt` 中的第三方库路径为本机开发路径：

```text
C:/app/opencv/build/x64/vc16/lib
C:/app/seetaface/sf6.0_windows
```

在其他电脑上构建时，请将这两个路径修改为对应的 OpenCV 和 SeetaFace6 安装位置。

## 使用 Qt Creator 构建

1. 使用 Qt Creator 打开项目根目录中的 `CMakeLists.txt`。
2. 选择安装了 Qt 6 的 Desktop Kit。
3. 确认 OpenCV 与 SeetaFace6 路径配置正确。
4. 配置项目后执行构建和运行。

## 目录说明

- `*.cpp`、`*.h`：程序源代码
- `*.ui`：Qt Designer 界面文件
- `face_resource.qrc`：Qt 资源文件
- `CMakeLists.txt`：CMake 构建配置

本仓库不包含本机生成的 `build` 目录、Qt Creator 用户配置、数据库和编译产物。
