#include "user.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static bool string_equal(const char *a, const char *b)
{
    int index = 0;

    while (a[index] != '\0' && b[index] != '\0')
    {
        if (a[index] != b[index])
            return false;

        index = -~index;
    }

    if (a[index] == '\0' && b[index] == '\0')
        return true;

    return false;
}

static bool read_line(const char *prompt, char *buffer, size_t size)
{
    int ch;
    size_t length = 0;
    bool overflow = false;

    /* 传参失败 */
    if (buffer == NULL || size == 0)
    {
        return false;
    }

    /* 输出提示语 */
    if (prompt != NULL)
    {
        printf("%s", prompt);
    }

    /* 读取用户输入 */
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        if (ch == '\r')
            continue;

        if (length < size - 1)
        {
            buffer[length] = (char)ch;
            length = -~length;
        }
        else
        {
            overflow = true;
        }
    }
    buffer[length] = '\0';

    /* 传参失败 */
    if (ch == EOF && length == 0)
        return false;

    if (overflow)
        return false;

    return true;
}

static void wait_for_enter(void) // 回车
{
    char input[INPUT_BUFFER_LENGTH];
    (void)read_line("\n按回车键继续...", input, sizeof(input));
}

static bool read_choice(char *choice)
{
    char input[INPUT_BUFFER_LENGTH];

    if (!read_line("请选择：", input, sizeof(input)))
        return false;

    if (input[0] == '\0' || input[1] != '\0')
        return false;

    *choice = input[0];

    return true;
}

static bool judge_valid1(char character)
{
    if (character >= '0' && character <= '9')
        return true;

    if (character >= 'A' && character <= 'Z')
        return true;

    if (character >= 'a' && character <= 'z')
        return true;

    if (character == '_')
        return true;

    return false;
}

static bool username_is_valid(const char *username)
{
    size_t index, length = strlen(username);

    /* 判断用户名长度 */
    if (length < USER_NAME_LENGTH_MIN || length >= USER_NAME_LENGTH_MAX)
        return false;

    /* 判断用户名字符是否合法 */
    for (index = 0; index < length; index = -~index)
    {
        if (!judge_valid1(username[index]))
            return false;
    }

    return true;
}

static bool judge_valid2(char character)
{
    if (character == ' ')
        return true;

    if (character == '\t')
        return true;

    if (character == '\n')
        return true;

    if (character == '\r')
        return true;

    return false;
}

static bool password_is_valid(const char *password)
{
    size_t index, length = strlen(password);

    /* 判断密码长度 */
    if (length < USER_PASSWORD_LENGTH_MIN || length > USER_PASSWORD_LENGTH_MAX)
        return false;

    /* 判断密码中是否合法 */
    for (index = 0; index < length; index = -~index)
    {
        if (judge_valid2(password[index]))
            return false;
    }

    return true;
}

static void copy_string(char *target, const char *source, size_t target_size)
{
    size_t index = 0;

    if (target == NULL || source == NULL || target_size == 0)
        return;

    while (index < target_size - 1 && source[index] != '\0')
    {
        target[index] = source[index];
        index = -~index;
    }

    target[index] = '\0';
}

static bool read_user_record(FILE *file, char *username, size_t username_size, char *password, size_t password_size)
{
    char line[INPUT_BUFFER_LENGTH];
    char stored_username[USER_NAME_LENGTH_MAX];
    char stored_password[USER_PASSWORD_BUFFER_SIZE];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // line先读到\t为止，存到stored_username中，再读到\r或\n为止，存到stored_password中
        if (sscanf(line, "%31[^\t]\t%20[^\r\n]", stored_username, stored_password) == 2)
        {
            copy_string(username, stored_username, username_size);
            copy_string(password, stored_password, password_size);
            return true;
        }
    }

    return false;
}

static bool username_exists(const char *user_file, const char *username)
{
    FILE *file = fopen(user_file, "r"); // 只读
    char stored_username[USER_NAME_LENGTH_MAX];
    char stored_password[USER_PASSWORD_BUFFER_SIZE];

    if (file == NULL)
        return false;

    while (read_user_record(file, stored_username, sizeof(stored_username), stored_password, sizeof(stored_password)))
    {
        if (string_equal(username, stored_username))
        {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

static bool register_user(const char *user_file)
{
    FILE *file;
    char username[USER_NAME_LENGTH_MAX];
    char password[USER_PASSWORD_BUFFER_SIZE];
    char confirmation[USER_PASSWORD_BUFFER_SIZE];

    puts("\n========== 用户注册 ==========");

    if (!read_line("请输入用户名（2-31位字母、数字或下划线）：", username, sizeof(username)))
    {
        puts("读取用户名失败!");
        return false;
    }

    if (!username_is_valid(username))
    {
        puts("用户名格式不正确!");
        return false;
    }

    if (username_exists(user_file, username))
    {
        puts("该用户名已被注册。");
        return false;
    }

    if (!read_line("请输入密码（6-20位，不能包含空格）：", password, sizeof(password)))
    {
        puts("读取密码失败!");
        return false;
    }

    if (!password_is_valid(password))
    {
        puts("密码格式不正确!");
        return false;
    }

    if (!read_line("请再次输入密码：", confirmation, sizeof(confirmation)))
    {
        puts("读取确认密码失败!");
        return false;
    }

    if (!string_equal(password, confirmation))
    {
        puts("两次输入的密码不一致。");
        return false;
    }

    file = fopen(user_file, "a");

    if (file == NULL)
    {
        printf("在访问文件%s时发生了错误，请联系管理员，错误代码001\n", user_file);
        return false;
    }

    if (fprintf(file, "%s\t%s\n", username, password) < 0)
    {
        fclose(file);
        puts("保存注册信息失败，请联系管理员，错误代码002");
        return false;
    }

    if (fclose(file) != 0)
    {
        puts("保存注册信息失败，请联系管理员，错误代码003");
        return false;
    }

    puts("注册成功，现在可以登录。 ");
    return true;
}

static bool login_user(const char *user_file, char *current_user, size_t current_user_size)
{
    FILE *file;
    char username[USER_NAME_LENGTH_MAX];
    char password[USER_PASSWORD_BUFFER_SIZE];
    char stored_username[USER_NAME_LENGTH_MAX];
    char stored_password[USER_PASSWORD_BUFFER_SIZE];

    puts("\n========== 用户登录 ==========");

    if (!read_line("用户名：", username, sizeof(username)) || !read_line("密码：", password, sizeof(password)))
    {
        puts("读取用户名或密码失败，请联系管理员，错误代码004 ");
        return false;
    }

    file = fopen(user_file, "r"); // 只读

    if (file == NULL)
    {
        puts("暂无用户数据，请先注册。 ");
        return false;
    }

    while (read_user_record(file, stored_username, sizeof(stored_username), stored_password, sizeof(stored_password)))
    {
        if (string_equal(username, stored_username) && string_equal(password, stored_password))
        {
            copy_string(current_user, username, current_user_size);
            fclose(file);
            printf("登录成功，欢迎你：%s！\n", current_user);
            return true;
        }
    }

    fclose(file);
    puts("用户名或密码错误。 ");
    return false;
}

void user_show_interface(const char *user_file, char *current_user, size_t current_user_size)
{
    char choice;

    while (true)
    {
        puts("\n========== 用户中心 ==========");

        /* 已登录 */
        if (current_user[0] != '\0')
        {
            printf("当前用户：%s\n", current_user);
            puts("1. 注销登录");
            puts("0. 返回主界面");

            if (!read_choice(&choice))
            {
                puts("输入错误，请重新输入!");
                continue;
            }

            if (choice == '1')
            {
                current_user[0] = '\0';
                puts("已注销登录。 ");
                wait_for_enter();
            }
            else if (choice == '0')
            {
                return;
            }
            else
            {
                puts("无效选项，请重新输入。 ");
            }
        }
        else
        {
            puts("1. 登录");
            puts("2. 注册");
            puts("0. 返回主界面");

            if (!read_choice(&choice))
            {
                puts("输入错误，请重新输入!");
                continue;
            }

            if (choice == '1')
            {
                (void)login_user(user_file, current_user, current_user_size);
                wait_for_enter();
            }
            else if (choice == '2')
            {
                (void)register_user(user_file);
                wait_for_enter();
            }
            else if (choice == '0')
            {
                return;
            }
            else
            {
                puts("无效选项，请重新输入。 ");
            }
        }
    }
}
