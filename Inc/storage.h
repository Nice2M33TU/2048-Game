#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

FILE *storage_open_read(const char *file_path); // 打开文件以读取模式，如果文件不存在则返回NULL

FILE *storage_open_append(const char *file_path); // 打开文件以追加模式，如果文件不存在则创建新文件

bool storage_read_line(FILE *file, char *buffer, size_t buffer_size); // 从文件中读取一行文本，存储到buffer中，buffer_size为缓冲区大小，成功返回true，失败返回false

bool storage_write_user(FILE *file, const char *username, const char *password); // 将用户名和密码写入文件，格式为"username\tpassword\n"，成功返回true，失败返回false

bool storage_close(FILE *file); // 关闭文件，成功返回true，失败返回false
