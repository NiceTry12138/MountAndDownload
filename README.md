# README

这是一个实验性质的插件，小组自己捣鼓的下载和热更新插件

## 热更新

可以使用 `UnrealPak.exe` 来创建 `.pak` 文件，然后动态的挂在这个 `Pak` 文件到游戏中，就可以实现动态添加内容到游戏中

首先创建一个 `txt` 文件用于标记哪些文件需要被打包到 `.pak` 文件中，大概内容如下

```txt
"F:\3_UEDemo\MountTest\Saved\Cooked\Windows\MountTest\Content\Mount\Level2.uexp" "../../../MountTest/Content/Mount/Level2.uexp" -compress
"F:\3_UEDemo\MountTest\Saved\Cooked\Windows\MountTest\Content\Mount\Level2.umap" "../../../MountTest/Content/Mount/Level2.umap" -compress
"F:\3_UEDemo\MountTest\Saved\Cooked\Windows\MountTest\Content\Mount\Mesh\M_MountTest.uasset" "../../../MountTest/Content/Mount/Mesh/M_MountTest.uasset" -compress
"F:\3_UEDemo\MountTest\Saved\Cooked\Windows\MountTest\Content\Mount\Mesh\M_MountTest.uexp" "../../../MountTest/Content/Mount/Mesh/M_MountTest.uexp" -compress
"F:\3_UEDemo\MountTest\Saved\Cooked\Windows\MountTest\Content\Mount\Mesh\Wall.uasset" "../../../MountTest/Content/Mount/Mesh/Wall.uasset" -compress
"F:\3_UEDemo\MountTest\Saved\Cooked\Windows\MountTest\Content\Mount\Mesh\Wall.ubulk" "../../../MountTest/Content/Mount/Mesh/Wall.ubulk" -compress
"F:\3_UEDemo\MountTest\Saved\Cooked\Windows\MountTest\Content\Mount\Mesh\Wall.uexp" "../../../MountTest/Content/Mount/Mesh/Wall.uexp" -compress
"F:\3_UEDemo\MountTest\Saved\Cooked\Windows\MountTest\Content\Mount\Blueprint\BP_MountTest_Actor.uasset" "../../../MountTest/Content/Mount/Blueprint/BP_MountTest_Actor.uasset" -compress
```

在 `UnrealPak.exe` 中可以通过 `-create` 字段来配置刚写好的 `txt` 文件

```bat
@echo off

@REM set UnrealPak="F:/UnrealLaunch/UE_5.3/Engine/Binaries/Win64/UnrealPak.exe"
set PATH=F:\UnrealLaunch\UE_5.3\Engine\Binaries\Win64;%PATH%
set InputTxtPath=%1
set OutputPakPath=%2
set RunPath=%CD%

if "%InputTxtPath%" == "" (
    set InputTxtPath=%RunPath%\CreatePakConfAndroid.txt
)

if "%OutputPakPath%" == "" (
    set OutputPakPath=%RunPath%\TestAndroid.pak
)

echo "UnrealPak Path => %UnrealPak%"
echo "InputTextPath => %InputTxtPath%"
echo "OutputPakPath => %OutputPakPath%"

UnrealPak.exe %OutputPakPath% -create=%InputTxtPath%

pause
```

## 下载

`HttpDownLoadTool` 插件用于下载指定网络路径下的文件，支持断点续传

对于服务来说要求支持 `Range`

可以使用 `npm install http-server` 来快速安装一个支持 `Range` 的简易 `Http` 请求文件的  `http-server` 库，然后通过 `npx http-server -p 8001` 来启动这个库，指定端口为 `8001`， 也就是可以通过 `http://127.0.0.1:8001` 来访问这个库

