#include "rank.h"
#include "config.h"
#include "storage.h"
#include "ui.h"
#include "utils.h"

#include <stdio.h>
#include <stdbool.h>

static int rank_load_scores(const char *scores_file, RankEntry *entries, int max_entries) // 读取排行榜数据，返回读取的记录数
{
    FILE *file = storage_open_read(scores_file);
    char line[INPUT_BUFFER_LENGTH];
    char username[USER_NAME_LENGTH_MAX];
    char encrypted_score[INPUT_BUFFER_LENGTH];
    char score_text[32];
    char extra_character;
    int score, count = 0;

    if (file == NULL)
        return 0;

    while (count < max_entries && storage_read_line(file, line, sizeof(line)))
    {
        if (sscanf(line, "%31[^\t]\t%127[^\r\n]", username, encrypted_score) == 2 &&
            utils_xor_decrypt_from_hex(encrypted_score, score_text, sizeof(score_text)) &&
            sscanf(score_text, "%d%c", &score, &extra_character) == 1 && score >= 0)
        {
            utils_copy_string(entries[count].username, username, sizeof(entries[count].username));
            entries[count].score = score;
            count = -~count;
        }
    }

    storage_close(file);
    return count;
}

static void rank_swap(RankEntry *a, RankEntry *b) // 交换两个排行榜记录
{
    RankEntry temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

static void rank_move_up(RankEntry *entries, int index) // 将指定索引的记录向上移动，直到满足降序排列
{
    while (index > 0 && entries[index].score > entries[index - 1].score)
    {
        rank_swap(&entries[index], &entries[index - 1]);
        index = index - 1;
    }
}

static int rank_find_user(const RankEntry *entries, int count, const char *username, int *best_score) // 查找指定用户名的记录，返回其在排行榜中的索引，如果未找到则返回-1，同时将其最高分写入best_score
{
    if (best_score == NULL)
        return -1;

    if (username == NULL || username[0] == '\0')
    {
        *best_score = 0;
        return -1;
    }

    for (int i = 0; i < count; i = -~i)
    {
        if (utils_string_equal(entries[i].username, username))
        {
            *best_score = entries[i].score;
            return i;
        }
    }

    *best_score = 0;
    return -1;
}

bool rank_save_score(const char *scores_file, const char *username, int score) // 保存用户分数到排行榜，若已有记录则更新最高分，返回true表示成功，false表示失败
{
    RankEntry entries[RANK_ENTRIES_MAX];
    int count, i;
    int cache_path_length;
    bool found = false;
    FILE *file;
    char cache_file[INPUT_BUFFER_LENGTH];

    if (scores_file == NULL || username == NULL || username[0] == '\0' || score < 0)
        return false;

    cache_path_length = snprintf(cache_file, sizeof(cache_file), "%s.tmp", scores_file);
    if (cache_path_length < 0 || (size_t)cache_path_length >= sizeof(cache_file))
        return false;

    count = rank_load_scores(scores_file, entries, RANK_ENTRIES_MAX);

    /* 查找已有记录，有则更新最高分 */
    for (i = 0; i < count; i = -~i)
    {
        if (utils_string_equal(entries[i].username, username))
        {
            found = true;
            if (score <= entries[i].score)
                return true;

            entries[i].score = score;
            rank_move_up(entries, i);
            break;
        }
    }

    /* 无记录则新增 */
    if (!found)
    {
        if (count < RANK_ENTRIES_MAX)
        {
            utils_copy_string(entries[count].username,
                              username,
                              sizeof(entries[count].username));
            entries[count].score = score;
            count = -~count;
            rank_move_up(entries, count - 1);
        }
        else
        {
            if (score <= entries[RANK_ENTRIES_MAX - 1].score)
                return true;

            utils_copy_string(entries[RANK_ENTRIES_MAX - 1].username,
                              username,
                              sizeof(entries[RANK_ENTRIES_MAX - 1].username));
            entries[RANK_ENTRIES_MAX - 1].score = score;
            rank_move_up(entries, RANK_ENTRIES_MAX - 1);
        }
    }

    file = storage_open_write(cache_file);
    if (file == NULL)
        return false;

    for (i = 0; i < count; i = -~i)
    {
        if (!storage_write_score(file, entries[i].username, entries[i].score))
        {
            storage_close(file);
            storage_remove_file(cache_file);
            return false;
        }
    }

    if (!storage_close(file))
    {
        storage_remove_file(cache_file);
        return false;
    }

    if (!storage_replace_file(cache_file, scores_file))
    {
        storage_remove_file(cache_file);
        return false;
    }

    return true;
}

void rank_show(const char *scores_file, const char *current_user) // 显示排行榜界面，current_user为当前登录的用户名，如果未登录则为NULL或空字符串
{
    RankEntry entries[RANK_ENTRIES_MAX];
    int count;
    int user_rank, user_best;

    count = rank_load_scores(scores_file, entries, RANK_ENTRIES_MAX);

    user_rank = rank_find_user(entries, count, current_user, &user_best);
    ui_show_ranking(entries, count, current_user, user_rank, user_best);
}
