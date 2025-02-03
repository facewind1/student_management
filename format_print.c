#include <stdio.h>
#include <stdarg.h>
#include "format_print.h"

// 统计一个字符串中中文字符的数量
int countChineseCharacters(const char *str) {
    int count = 0;
    while (*str) {
        unsigned char c = (unsigned char)(*str);
        if ((c & 0xF0) == 0xE0) { // UTF-8 汉字以 0xE0 - 0xEF 开头
            count++;
            str += 3; // 跳过汉字的 3 个字节
        } else {
            str++; // 其他字符逐字遍历
        }
    }
    return count;
}

// 制定宽度输出
void align_print(int width, const char* content)
{
    int chinses_num = countChineseCharacters(content);
    int real_width = width + chinses_num;
    printf("%-*s", real_width, content);
}

// 写一行
void write_row(int count, int* width_all, ...)
{
    va_list args;
    va_start(args, count);  // 初始化变长参数

    for(int i = 0; i < count; i++) {
        const char *str = va_arg(args, const char*);
        if (str == NULL) {
            printf("[NULL]");  // 处理 NULL 保护
        } else {
            align_print(width_all[i], str);
        }
    }
    va_end(args);
    printf("\n");
}