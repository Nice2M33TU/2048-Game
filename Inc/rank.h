#pragma once

#include <stdbool.h>

#include "config.h"

typedef struct
{
    char username[USER_NAME_LENGTH_MAX];
    int score;
} RankEntry;

void rank_show(const char *scores_file, const char *current_user); // 显示排行榜界面，current_user为当前登录的用户名，如果未登录则为NULL或空字符串

bool rank_save_score(const char *scores_file, const char *username, int score); // 保存用户分数到排行榜，若已有记录则更新最高分，返回true表示成功，false表示失败
