# 2048 Game

基于 C、CMake、Ninja 和 MinGW 构建的控制台 2048 游戏项目。

## 开发环境

当前 `.vscode/settings.json` 和 `.vscode/tasks.json` 使用本机 CMake、Ninja 与 MinGW 的绝对路径，以保证在当前电脑上直接编译运行。

## 编译运行

在 VS Code 中按 `Ctrl+Shift+B`，默认任务 `2048: Build and Run` 会依次完成 CMake 配置、编译并运行程序。

生成的程序位于：

```text
build/bin/2048_game.exe
```

用户注册数据保存在 `Data/user.txt`。
