#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "user.h"
#include "config.h"
#include "ui.h"
#include "utils.h"

int main(void)
{
    ui_initialize_console();
    run_game_system();

    return 0;
}
