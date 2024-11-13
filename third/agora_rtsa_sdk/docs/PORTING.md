# 移植指南

## 1. 修改 example/scripts/toolchain.cmake，设置交叉编译工具链的路径和前缀

比如，你使用的工具链是ARM官方的GNU Toolchain，则可以这样设置
```
set(DIR /home/username/toolchain/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/)
set(CROSS "arm-linux-gnueabihf-")
```

注意：工具链的位置必须设置为 **绝对路径**

## 2. 编译
```
$ cd example
$ ./build-arm.sh
```
编译完成后，会在当前目录里创建out/arm/目录，并在该目录下生成 hello_rtsa 可执行文件。

## 3. 运行 hello_rtsa 

复制以下文件到目标设备的 `/YOUR_DIR` 目录
1. agora_sdk/lib/libagora-rtc-sdk.so
2. example/out/arm/hello_rtsa
3. example/out/arm/send_video.h264
4. example/out/arm/send_audio_16k_1ch.pcm
5. example/out/arm/certificate.bin

在目标设备shell界面执行以下操作
```
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/YOUR_DIR
$ cd /YOUR_DIR
$ ./hello_rtsa -i YOUR_APPID -c hello_demo
```