// 排序.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "sort.h"
#include <iostream>
#include <conio.h>
#include <time.h>

// 清空缓冲区
void clearCache() {
    char ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

/* 本来想做一个计时函数，但是不会操作先留着
void stopWatch(void (*sort)()) {
    int begin = clock();
    sort();
    int end = clock();
} */

int main(){
    int* arr = (int*)malloc(sizeof(int*) * 5000);
    int* temp = (int*)malloc(sizeof(int*) * 5000);  //临时数组
    while (1) {
        system("cls");
        printf("******************************************\n");
        printf("\t\t    排序\n");
        printf("******************************************\n");
        printf("\t\t1.冒泡排序\n");
        printf("\t\t2.选择排序\n");
        printf("\t\t3.插入排序\n");
        printf("\t\t4.归并排序\n");
        printf("\t\t5.快速排序\n");
        printf("******************************************\n");
        int length = 0;
        printf("请输入要排序的数字（空格隔开）：\n"); 
        char probe; //字符探针
        while ((probe = getchar()) != '\n') {
            if (isdigit(probe)) {  //如果该字符为数字
                ungetc(probe,stdin);  //退回输入流
                scanf_s("%d", &arr[length++]);  //读取
            }
            else if (probe != ' ') { //若出现奇怪的东西
                length = -1;  //赋一个不可能的值
                break;
            }
        }
        int way;  //保存排序方式指令
        if (length ==1 || length ==-1) {
            if (length == 1) 
                printf("一个数不需要排序哦！");
            else {
                printf("好像有什么奇怪的东西乱入了！\n");
                clearCache();
            }
            if (_getch()) //检测到按键则继续
                continue;
        }
        else if (length == 0) {
            printf("懒得输入？按7试试\n");
            if (_getch() == '7') {
                printf("你要随机生成多少个数字（不大于5000）？\n");
                int num;
                while (!scanf_s("%d", &num) || num >5000) {
                    if (num > 5000)
                        printf("最多只能生成5000个哦！\n");
                    else
                        printf("好像有什么奇怪的东西乱入了！\n");
                    num = 0;
                    clearCache();
                }

                srand(time(NULL));  //随机种子
                for (; length < num; length++) {
                    // 生成0-num的随机数赋值并输出
                    printf("%6d", arr[length] = rand() % num+1); 
                }
                printf("\n已帮你生成%d个随机数！\n", num);
            }
            else continue;
        }

        printf("请选择排序方式：");
        while (1) {
            // 循环直至输入正确
            while (!scanf_s("%d", &way)) {
                printf("请输入数字指令！\n");
                clearCache();
            }
            int begin = clock(), end;  //取当前时间为begin
            switch (way) {
            case 1:
                bubbleSort(arr, length);
                end = clock();
                break;
            case 2:
                selctionSort(arr, length);
                end = clock();
                break;
            case 3:
                insertionSort(arr, length);
                end = clock();
                break;
            case 4:
                mergeSort(arr, 0, length - 1, temp); 
                end = clock();
                break;
            case 5:
                quickSort(arr, 0, length - 1);
                end = clock();
                break;
            default:
                printf("没有该指令，请重新输入！\n");
                clearCache();
                continue;
            }
            printf("排序结果如下：\n");
            for (int i = 0; i < length; i++) {
                printf("%d  ", arr[i]);
            }
            printf("\n\n排序耗时：%dms\n", end - begin);
            break;
        }
        clearCache();
        printf("按任意键继续......");
        if (_getch());
    }
    free(arr);
    free(temp);
}

