### 本项目基于ffmpeg 3.x和SDL 2.0构建

环境：vs2017 并安装qt插件，插件为x86的

在编译时，可能需要配置ffmpeg和SDL的 include文件和 libs文件。

参考 https://blog.csdn.net/hfuu1504011020/article/details/82590612

基于该博客的代码，本地调试，修补了一些bug，程序能正常启动，打开视频文件，播放及暂停、进度条调整播放进度。

在测试音频文件时，可能会出现问题，程序中一些条件还要修改。