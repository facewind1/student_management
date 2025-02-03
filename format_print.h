// my_library.h
#ifndef FORMAT_PRINT_H
#define FORMAT_PRINT_H

// 统计一个字符串中中文字符的数量
int countChineseCharacters(const char *str);

// 制定宽度输出
void align_print(int width, const char* content);

// 写一行
void write_row(int count, int* width_all, ...);

#endif // MY_LIBRARY_H
