# OpenGL + OpenMesh + OpenCV配置

## OpenGL:
1. VC include
    1. glut.h 放入 ${VC include}

2. VC lib
    1. glut32.lib 放入 ${VC lib}

3. VC bin
    1. glut32.dll 放入 ${VC bin}

4. 连接器
    1. 输入
        1. 附加依赖性 添加 opengl32.lib，glu32.lib，glut32.lib

## OpenMesh:
1. VC++目录
    1. 包含目录 添加 ${OpenMeshPath}\include、${OpenMeshPath}\include\opencv、${OpenMeshPath}\include\opencv2
    2. 库目录 添加 ${OpenMeshPath}\lib

2. C/C++
    1. 预处理器
        1. 预处理器定义 添加 _USE_MATH_DEFINES

3. 连接器
    1. 输入
        1. 附加依赖性 添加 OpenMeshCored.lib、OpenMeshToolsd.lib、OpenMeshCore.lib、OpenMeshTools.lib

## OpenCV:
1. VC++目录
    1. 包含目录 添加 ${OpenCVPath}\include
    2. 库目录 添加 ${OpenCVPath}\lib

2. 连接器
    1. 输入
        1. 附加依赖性 添加 opencv_ts300d.lib、opencv_world300d.lib、opencv_ts300.lib、opencv_world300.lib
