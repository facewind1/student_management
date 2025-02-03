#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mariadb/mysql.h>
#include <unistd.h>  // 用于 sleep 函数
#include <termios.h> // 用于隐藏密码输入
#include "format_print.h"

#define DB_HOST "www.familywang.top"
#define DB_USER "wyk"
#define DB_NAME "student_db"

char DB_PASS[50]; // 动态存储密码
MYSQL *conn = NULL; // 全局数据库连接指针

void execute_query(const char *query);
void display_menu();
void add_student();
void display_all();
void search_student();
void delete_student();
int init_db_connection();
void close_db_connection();
void set_password_input_visibility(int visible);

int main() {
    int choice;

    // 设置密码输入不可见
    set_password_input_visibility(0);

    // 连接数据库
    do {
        printf("请输入数据库密码：");
        scanf("%s", DB_PASS);

        if (init_db_connection()) {
            printf("数据库连接成功！\n");
            break;
        } else {
            printf("密码错误，2 秒后重试...\n");
            sleep(2);
        }
    } while (1);

    // 恢复密码输入可见
    set_password_input_visibility(1);

    // 主菜单循环
    do {
        display_menu();
        printf("\n请输入选项：");
        scanf("%d", &choice);

        switch(choice) {
            case 1: add_student(); break;
            case 2: display_all(); break;
            case 3: search_student(); break;
            case 4: delete_student(); break;
            case 5: printf("感谢使用！\n"); break;
            default: printf("无效选项，请重新输入！\n");
        }
    } while(choice != 5);

    // 退出前关闭数据库连接
    close_db_connection();

    return 0;
}

// 初始化数据库连接
int init_db_connection() {
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) {
        mysql_close(conn);
        conn = NULL;
        return 0; // 连接失败
    }
    return 1; // 连接成功
}

// 关闭数据库连接
void close_db_connection() {
    if (conn) {
        mysql_close(conn);
        conn = NULL;
    }
}

// 设置密码输入是否可见
void set_password_input_visibility(int visible) {
    struct termios tty;
    tcgetattr(fileno(stdin), &tty);
    if (visible) {
        tty.c_lflag |= ECHO; // 恢复显示
    } else {
        tty.c_lflag &= ~ECHO; // 隐藏输入
    }
    tcsetattr(fileno(stdin), TCSANOW, &tty);
}

// 执行 SQL 查询
void execute_query(const char *query) {
    if (!conn) {
        fprintf(stderr, "数据库连接未初始化！\n");
        exit(1);
    }

    if (mysql_query(conn, query)) {
        fprintf(stderr, "SQL 执行失败: %s\n", mysql_error(conn));
    }
}

// 显示菜单
void display_menu() {
    printf("\n==== 学生管理系统 ====\n");
    printf("1. 添加学生\n");
    printf("2. 显示所有学生\n");
    printf("3. 查找学生\n");
    printf("4. 删除学生\n");
    printf("5. 退出系统\n");
}

// 添加学生
void add_student() {
    int id, age;
    char name[50];
    float score;

    printf("请输入学号：");
    scanf("%d", &id);
    printf("请输入姓名：");
    scanf("%s", name);
    printf("请输入年龄：");
    scanf("%d", &age);
    printf("请输入成绩：");
    scanf("%f", &score);

    char query[256];
    sprintf(query, "INSERT INTO students (id, name, age, score) VALUES (%d, '%s', %d, %.2f)", id, name, age, score);
    execute_query(query);
    printf("学生添加成功！\n");
}

// 显示所有学生
void display_all() {
    if (!conn) {
        fprintf(stderr, "数据库连接未初始化！\n");
        return;
    }

    if (mysql_query(conn, "SELECT * FROM students")) {
        fprintf(stderr, "查询失败: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row;

    int width_all[] = {15, 15, 10, 10};
    write_row(4, width_all, "学号", "姓名", "年龄", "成绩");

    while ((row = mysql_fetch_row(res))) {
        write_row(4, width_all, row[0], row[1], row[2], row[3]);
    }

    mysql_free_result(res);
}

// 查找学生
void search_student() {
    int id;
    printf("请输入要查找的学号：");
    scanf("%d", &id);

    if (!conn) {
        fprintf(stderr, "数据库连接未初始化！\n");
        return;
    }

    char query[256];
    sprintf(query, "SELECT * FROM students WHERE id = %d", id);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "查询失败: %s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row;
    if ((row = mysql_fetch_row(res))) {
        printf("\n学号：%s\n姓名：%s\n年龄：%s\n成绩：%s\n", row[0], row[1], row[2], row[3]);
    } else {
        printf("未找到学号为 %d 的学生！\n", id);
    }

    mysql_free_result(res);
}

// 删除学生
void delete_student() {
    int id;
    printf("请输入要删除的学号：");
    scanf("%d", &id);

    char query[256];
    sprintf(query, "DELETE FROM students WHERE id = %d", id);
    execute_query(query);
    printf("学生删除成功！\n");
}