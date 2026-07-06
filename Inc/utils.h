#pragma once

#include <stdbool.h>
#include <stddef.h>

bool utils_string_equal(const char *a, const char *b); // 比较两个串的值，相同返回true，不同返回false

bool utils_read_line(const char *prompt, char *buffer, size_t size); // 输出prompt提示词，读取一行输入，返回true表示成功，false表示失败

bool utils_read_choice(char *choice); // 读取用户选择，返回true表示成功，false表示失败

void utils_copy_string(char *target, const char *source, size_t target_size); // 将source复制到target，最多复制target_size - 1个字符

bool utils_is_valid_username(char character); // 判断用户名是否合法，返回true表示合法，false表示不合法

bool utils_is_valid_password(char character); // 判断密码是否合法，返回true表示合法，false表示不合法