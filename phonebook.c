// 用C语言设计出模拟手机通信系统，能实现对手机中的通信录进行添加、修改、查询等功能。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
// 定义人员信息结构体
typedef struct person {
    char name[20]; // 姓名
    char mobile[20]; // 手机号码
    char home[20]; // 家庭电话号码
    char office[20]; // 办公电话
    char email[50]; // 电子邮件
    char group[10]; // 分组
} person;

// 定义通讯录结构体
typedef struct contact {
    person *data; // 存储人员信息的数组
    int size; // 当前人员数量
    int capacity; // 最大容量
} contact;

// 定义文件名常量
#define FILE_NAME "contact.txt" // 文本文件

// 定义菜单选项常量
#define ADD '1'
#define VIEW '2'
#define CALL '3'
#define MODIFY '4'
#define DELETE '5'
#define SEARCH '6'
#define SAVE '7'
#define EXIT '0'

// 定义函数原型
// 初始化通讯录
void init_contact(contact *c);
// 释放通讯录内存
void free_contact(contact *c);
// 从文件中加载通讯录数据
void load_contact(contact *c);
// 向文件中保存通讯录数据
void save_contact(contact *c);
// 检查通讯录是否为空
int is_empty(contact *c);
// 检查通讯录是否已满
int is_full(contact *c);
// 扩展通讯录容量
void expand_contact(contact *c);
// 添加新人员记录
void add_person(contact *c);
// 按分组查找人员
void view_person(contact *c);
// 拔号给某个人员
void call_person(contact *c);
// 修改某个人员记录
void modify_person(contact *c);
// 删除某个人员记录
void delete_person(contact *c);
// 按姓名查找某个人员记录
int search_by_name(contact *c, char *name);
// 按电话号码查找某个人员记录
int search_by_phone(contact *c, char *phone);
// 显示某个人员的详细信息
void show_person(person p);
// 显示欢迎界面
void show_welcome();
// 显示主菜单界面
void show_menu();
// 获取用户输入的选项字符
char get_choice();
// 清空输入缓冲区
void clear_buffer();
// 清屏函数，兼容Windows和Linux系统
void clear_screen();

int main() {
    contact c; // 声明一个通讯录变量
    init_contact(&c); // 初始化通讯录
    load_contact(&c); // 加载通讯录数据
    show_welcome(); // 显示欢迎界面
    char choice; // 声明一个选项变量

    while (1) {
        show_menu(); // 显示主菜单界面

        choice = get_choice(); // 获取用户输入的选项字符

        switch (choice) {
            case ADD: // 添加新人员记录

                add_person(&c);

                break;

            case VIEW: // 按分组查找人员

                view_person(&c);

                break;
            case CALL://拨号给某个人员
				
				call_person(&c);
					
				break;

            case MODIFY: // 修改某个人员记录

                modify_person(&c);

                break;

            case DELETE: // 删除某个人员记录

                delete_person(&c);

                break;

            case SEARCH: // 按姓名或按电话号码查找某人信息

                {
                    printf("検索したい名前または電話番号を入力してください：");
                    char key[20];
                    scanf("%s", key);
                    clear_buffer();
                    int index = search_by_name(&c, key); // 先按姓名查找
                    if (index == -1) { // 如果没找到，再按电话号码查找
                        index = search_by_phone(&c, key);
                    }
                    if (index == -1) { // 如果还没找到，提示用户
                        printf("一致するレコードが見つかりませんでした！\n");
                    } else { // 如果找到了，显示详细信息
                        show_person(c.data[index]);
                    }
                    break;
                }

            case SAVE: // 将当前系统中各类记录存入文件中

                {
                    save_contact(&c);
                    break;
                }

            case EXIT: // 退出系统

                {
                    printf("ご利用いただきありがとうございました。さようなら！\n");
                    free_contact(&c); // 释放通讯录内存
                    return 0; // 结束程序
                }

            default: // 输入错误选项

                printf("正しい選択肢を入力してください。！\n");
        }

        printf("メインメニューに戻るには、Enterキーを押してください。...\n");
        getchar(); // 等待用户输入任意键
        clear_screen(); // 清屏
    }

    return 0;
}

// 初始化通讯录
void init_contact(contact *c) {
    c->data = NULL; // 初始化数据指针为空
    c->size = 0; // 初始化人员数量为0
    c->capacity = 0; // 初始化最大容量为0
}

// 释放通讯录内存
void free_contact(contact *c) {
    if (c->data != NULL) { // 如果数据指针不为空，释放内存空间
        free(c->data);
        c->data = NULL;
    }
    c->size = 0; // 重置人员数量为0
    c->capacity = 0; // 重置最大容量为0
}

// 从文件中加载通讯录数据
void load_contact(contact *c) {
    FILE *fp = fopen(FILE_NAME, "r"); // 以文本读模式打开文件
    if (fp == NULL) { // 如果文件打开失败，提示用户并返回
        printf("ファイルを開けませんでした！\n");
        return;
    }
    int n; // 声明一个变量用于存储文件中的人员数量
    fscanf(fp, "%d\n", &n); // 从文件中读取人员数量
    if (n > 0) { // 如果人员数量大于0，分配相应的内存空间，并从文件中读取人员信息
        c->data = (person *)malloc(n * sizeof(person)); // 分配内存空间
        if (c->data == NULL) { // 如果内存分配失败，提示用户并返回
            printf("メモリ割り当てに失敗しました！\n");
            return;
        }
        int i;for ( i = 0; i < n; i++) { // 遍历每个人员信息
            person p; // 声明一个人员信息变量
            fscanf(fp, "%s %s %s %s %s %s\n", p.name, p.mobile, p.home, p.office, p.email, p.group); // 从文件中读取人员信息，以空格分隔各个字段，以换行符结束一条记录
            c->data[i] = p; // 将读取到的人员信息添加到通讯录中
        }
        c->size = n; // 更新通讯录的人员数量
        c->capacity = n; // 更新通讯录的最大容量
    }
    fclose(fp); // 关闭文件
}

// 向文件中保存通讯录数据
void save_contact(contact *c) {
    FILE *fp = fopen(FILE_NAME, "w"); // 以文本写模式打开文件
    if (fp == NULL) { // 如果文件打开失败，提示用户并返回
        printf("ファイルを開けませんでした！\n");
        return;
    }
    fprintf(fp, "%d\n", c->size); // 向文件中写入通讯录的人员数量
    if (c->size > 0) { // 如果人员数量大于0，向文件中写入通讯录的人员信息
        int i;for ( i = 0; i < c->size; i++) { // 遍历每个人员信息
            person p = c->data[i]; // 获取对应的人员信息
            fprintf(fp, "%s %s %s %s %s %s\n", p.name, p.mobile, p.home, p.office, p.email, p.group); // 向文件中写入人员信息，以空格分隔各个字段，以换行符结束一条记录
        }
    }
    fclose(fp); // 关闭文件
    printf("保存に成功しました！\n");
}

// 检查通讯录是否为空
int is_empty(contact *c) {
    return c->size == 0; // 如果人员数量为0，返回1，否则返回0
}

// 检查通讯录是否已满
int is_full(contact *c) {
    return c->size == c->capacity; // 如果人员数量等于最大容量，返回1，否则返回0
}

// 扩展通讯录容量
void expand_contact(contact *c) {
    if (is_full(c)) { // 如果通讯录已满，扩展容量
        int new_capacity = c->capacity + 10; // 新的容量为原来的容量加10
        person *new_data = (person *)realloc(c->data, new_capacity * sizeof(person)); // 重新分配内存空间
        if (new_data == NULL) { // 如果内存分配失败，提示用户并返回
            printf("メモリ割り当てに失敗しました！\n");
            return;
        }
        c->data = new_data; // 更新数据指针
        c->capacity = new_capacity; // 更新最大容量
    }
}

// 添加新人员记录
void add_person(contact *c) {
    expand_contact(c); // 扩展通讯录容量

    person p; // 声明一个人员信息变量

    printf("お名前を入力してください：");
    scanf("%s", p.name); // 输入姓名
    clear_buffer();

    printf("携帯電話番号を入力してください：");
    scanf("%s", p.mobile); // 输入手机号码
    clear_buffer();

    printf("ご自宅の電話番号を入力してください:");
    scanf("%s", p.home); // 输入家庭电话号码
    clear_buffer();

    printf("オフィスの電話番号を入力してください：");
    scanf("%s", p.office); // 输入办公电话
    clear_buffer();

    printf("メールアドレスを入力してください：");
    scanf("%s", p.email); // 输入电子邮件
    clear_buffer();

    printf("グループに参加してください：");
    scanf("%s", p.group); // 输入分组
    clear_buffer();

    if (strlen(p.mobile) == 0 && strlen(p.home) == 0 && strlen(p.office) == 0) { // 如果手机号码、家庭电话号码、办公电话都为空，提示用户并返回
        printf("携帯電話番号、自宅電話番号、会社の電話番号は、すべて同時に空にすることはできません！\n");
        return;
    }

    c->data[c->size] = p; // 将新的人员信息添加到通讯录中

    c->size++; // 更新通讯录的人员数量

    printf("正常に追加されました！\n");
}

// 按分组查找人员
void view_person(contact *c) {if (is_empty(c)) { // 如果通讯录为空，提示用户并返回
        printf("アドレス帳が空っぽです！\n");
        return;
    }
    printf("グループに参加してください\n");
    char q[10],e[20]={};
    int w=1;
    scanf("%s",q);
    clear_buffer();
    int i;for ( i = 0; i<c->size; i++){
    	if(!strcmp(c->data[i].group,q)){
    		if(w){
    			printf("名前%12s電話番号%12s自宅の電話番号%12sオフィス電話\n",e,e,e); // 打印表头
    			w=0;
    		}
    		person p = c->data[i];
        	printf("%-16s%-20s%-24s%-20s\n", p.name, p.mobile, p.home, p.office);
    	}
    }
    if(w)
    	printf("そのグループは見つかりませんでした。\n");
}

// 拔号给某个人员
void call_person(contact *c) {
    if (is_empty(c)) { // 如果通讯录为空，提示用户并返回
        printf("アドレス帳は空です！\n");
        return;
    }
    printf("電話をかけたい相手の名前を選択してください：\n"); // 提示用户选择要拔号的人员姓名
    int i;for ( i = 0; i < c->size; i++) { // 遍历通讯录中的每个人员信息，并打印出姓名和编号
        person p = c->data[i];
        printf("%d. %s\n", i + 1, p.name);
    }
    int index; // 声明一个变量用于存储用户输入的编号
    scanf("%d", &index); // 获取用户输入的编号
    clear_buffer();
    if (index < 1 || index > c->size) { // 如果编号不在有效范围内，提示用户并返回
        printf("正しい数字を入力してください！\n");
        return;
    }
    person p = c->data[index - 1]; // 根据编号获取对应的人员信息
    char *phone; // 声明一个指针用于存储要拔号的电话号码
    if (strlen(p.mobile) > 0) { // 如果手机号码不为空，优先选择手机号码
        phone = p.mobile;
    } else if (strlen(p.home) > 0) { // 如果家庭电话号码不为空，其次选择家庭电话号码
        phone = p.home;
    } else { // 否则选择办公电话
        phone = p.office;
    }
    printf("ダイヤルして%s...\n", p.name); // 提示用户正在拔号
	for ( i = 0; i < strlen(phone); i++) { // 遍历电话号码中的每个数字，并模拟打字机的效果显示出来
        printf("%c", phone[i]);
        fflush(stdout); // 刷新输出缓冲区，使得每个数字能立即显示出来

        // 暂停1秒
		usleep(100000); // 使用usleep函数暂停1秒
    }
    printf("\n"); // 换行
}

// 修改某个人员记录
void modify_person(contact *c) {
    if (is_empty(c)) { // 如果通讯录为空，提示用户并返回
        printf("アドレス帳は空です！\n");
        return;
    }
    printf("変更したい人物の名前を選択してください。\n"); // 提示用户选择要修改的人员姓名
    int i;for ( i = 0; i < c->size; i++) { // 遍历通讯录中的每个人员信息，并打印出姓名和编号
        person p = c->data[i];
        printf("%d. %s\n", i + 1, p.name);
    }
    int index; // 声明一个变量用于存储用户输入的编号
    scanf("%d", &index); // 获取用户输入的编号
    clear_buffer();
    if (index < 1 || index > c->size) { // 如果编号不在有效范围内，提示用户并返回
        printf("正しい数字を入力してください！\n");
        return;
    }
    person *p = &c->data[index - 1]; // 根据编号获取对应的人员信息的指针
    char name[20]; // 声明一个变量用于存储新的姓名
    printf("新しい名前を入力してください。（元の名前は%s）：", p->name);
    scanf("%s", name); // 输入新的姓名
    clear_buffer();
    if (strlen(name) > 0) { // 如果新的姓名不为空，更新人员信息中的姓名
        strcpy(p->name, name);
    }
    char mobile[20]; // 声明一个变量用于存储新的手机号码
    printf("新しい携帯電話番号を入力してください。（元の携帯電話番号は%s）：", p->mobile);
    scanf("%s", mobile); // 输入新的手机号码
    clear_buffer();
    if (strlen(mobile) > 0) { // 如果新的手机号码不为空，更新人员信息中的手机号码
        strcpy(p->mobile, mobile);
    }
    char home[20]; // 声明一个变量用于存储新的家庭电话号码
    printf("新しい自宅の電話番号を入力してください。（元の自宅電話番号は%s）：", p->home);
    scanf("%s", home); // 输入新的家庭电话号码
    clear_buffer();
    if (strlen(home) > 0) { // 如果新的家庭电话号码不为空，更新人员信息中的家庭电话号码
        strcpy(p->home, home);
    }
    char office[20]; // 声明一个变量用于存储新的办公电话
    printf("新しいオフィスの電話番号を入力してください。（元のオフィスの電話番号は%s）：", p->office);
    scanf("%s", office); // 输入新的办公电话
    clear_buffer();
if (strlen(office) > 0) { // 如果新的办公电话不为空，更新人员信息中的办公电话
        strcpy(p->office, office);
    }
    char email[50]; // 声明一个变量用于存储新的电子邮件
    printf("新しいメールアドレスを入力してください。（元のメールは%s）：", p->email);
    scanf("%s", email); // 输入新的电子邮件
    clear_buffer();
    if (strlen(email) > 0) { // 如果新的电子邮件不为空，更新人员信息中的电子邮件
        strcpy(p->email, email);
    }
    char group[10]; // 声明一个变量用于存储新的分组
    printf("新しいグループに参加してください。（元のグループは%s）：", p->group);
    scanf("%s", group); // 输入新的分组
    clear_buffer();
    if (strlen(group) > 0) { // 如果新的分组不为空，更新人员信息中的分组
        strcpy(p->group, group);
    }
    if (strlen(p->mobile) == 0 && strlen(p->home) == 0 && strlen(p->office) == 0) { // 如果修改后手机号码、家庭电话号码、办公电话都为空，提示用户并返回
        printf("携帯電話番号、自宅電話番号、会社の電話番号は、すべて同時に空にすることはできません！\n");
        return;
    }
    printf("変更に成功しました！\n");
}

// 删除某个人员记录
void delete_person(contact *c) {
    if (is_empty(c)) { // 如果通讯录为空，提示用户并返回
        printf("アドレス帳は空です！\n");
        return;
    }
    printf("削除する人物の名前を選択してください：\n"); // 提示用户选择要删除的人员姓名
    int i;for ( i = 0; i < c->size; i++) { // 遍历通讯录中的每个人员信息，并打印出姓名和编号
        person p = c->data[i];
        printf("%d. %s\n", i + 1, p.name);
    }
    int index; // 声明一个变量用于存储用户输入的编号
    scanf("%d", &index); // 获取用户输入的编号
    clear_buffer();
    if (index < 1 || index > c->size) { // 如果编号不在有效范围内，提示用户并返回
        printf("正しい数字を入力してください！\n");
        return;
    }
    for (i = index - 1; i < c->size - 1; i++) { // 从要删除的人员信息开始，将后续的人员信息依次向前移动一位，覆盖掉要删除的人员信息
        c->data[i] = c->data[i + 1];
    }
    c->size--; // 更新通讯录的人员数量
    printf("削除に成功しました！\n");
}

// 按姓名查找某个人员记录
int search_by_name(contact *c, char *name) {
    int i;for ( i = 0; i < c->size; i++) { // 遍历通讯录中的每个人员信息，比较姓名是否匹配
        person p = c->data[i];
        if (strcmp(p.name, name) == 0) { // 如果姓名匹配，返回对应的索引
            return i;
        }
}
    return -1; // 如果没有找到匹配的姓名，返回-1
}

// 按电话号码查找某个人员记录
int search_by_phone(contact *c, char *phone) {
    int i;for ( i = 0; i < c->size; i++) { // 遍历通讯录中的每个人员信息，比较电话号码是否匹配
        person p = c->data[i];
        if (strcmp(p.mobile, phone) == 0 || strcmp(p.home, phone) == 0 || strcmp(p.office, phone) == 0) { // 如果手机号码、家庭电话号码、办公电话中有一个匹配，返回对应的索引
            return i;
        }
    }
    return -1; // 如果没有找到匹配的电话号码，返回-1
}

// 显示某个人员的详细信息
void show_person(person p) {
    printf("名前：%s\n", p.name); // 打印姓名
    printf("電話番号：%s\n", p.mobile); // 打印手机号码
    printf("自宅の電話番号：%s\n", p.home); // 打印家庭电话号码
    printf("オフィス電話：%s\n", p.office); // 打印办公电话
    printf("メール：%s\n", p.email); // 打印电子邮件
    printf("グループ分け：%s\n", p.group); // 打印分组
}

// 显示欢迎界面
void show_welcome() {
    printf("アナログ携帯電話通信システムへようこそ！\n(Enterキーを押してメイン画面に入ります)\n");
    clear_buffer();
    clear_screen() ;
}

// 显示主菜单界面
void show_menu() {
	printf("実行したい操作を選択してください。：\n");
    printf("1. 新しい人事記録を追加する\n");
    printf("2. グループ別に人材を検索する\n");
    printf("3. 特定の人に電話をかける\n");
    printf("4. 個人の記録を修正する\n");
    printf("5. 個人の記録を削除する\n");
    printf("6. 個人の記録を検索する\n");
    printf("7. 連絡先データをファイルに保存する\n");
    printf("0. 終了システム\n");
	}

// 获取用户输入的选项字符
char get_choice() {
    char choice; // 声明一个变量用于存储用户输入的选项字符
    scanf("%c", &choice); // 获取用户输入的选项字符
    clear_buffer();
    return choice; // 返回选项字符
}

// 清空输入缓冲区
void clear_buffer() {
    while (getchar() != '\n'); // 循环读取输入缓冲区中的字符，直到遇到换行符为止，从而清空输入缓冲区
}

// 清屏函数，兼容Windows和Linux系统
void clear_screen() {
#ifdef _WIN32 // 如果是Windows系统，使用system("cls")命令清屏
    system("cls");
#else // 否则，使用ANSI转义序列清屏
    printf("\033[2J\033[1;1H");
#endif
}
