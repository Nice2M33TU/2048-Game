# 2048 Game

基于 C、CMake、Ninja 和 MinGW 构建的控制台 2048 游戏项目。

## 小组开发环境

不需要修改 Windows 环境变量。工具路径统一定义在 `.vscode/settings.json` 开头，作用类似代码中的 `#define`：

| 路径宏 | 指向的文件 |
| --- | --- | --- |
| `2048Game.cmakePath` | `cmake.exe` |
| `2048Game.ninjaPath` | `ninja.exe` |
| `2048Game.gccPath` | `gcc.exe` |

配置示例：

```json
{
    "2048Game.cmakePath": "D:\\Tools\\CMake\\bin\\cmake.exe",
    "2048Game.ninjaPath": "D:\\Tools\\Ninja\\ninja.exe",
    "2048Game.gccPath": "D:\\Tools\\MinGW\\bin\\gcc.exe"
}
```

`tasks.json` 通过 `${config:2048Game.cmakePath}` 等宏引用这些值。其他成员只需修改上述三行，不需要改任务文件或系统环境变量。

## 编译运行

在 VS Code 中按 `Ctrl+Shift+B`，默认任务 `2048: Build and Run` 会依次完成 CMake 配置、编译并运行程序。

生成的程序位于：

```text
build/bin/2048_game.exe
```

用户注册数据保存在 `Data/user.txt`。
