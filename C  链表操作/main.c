#include "linkedList.h"
#include <stdio.h>
#include <Windows.h>

int main()
{
	LinkedList list = (LinkedList)malloc(sizeof(LNode)); // 申请内存空间存放头节点
	// 若申请失败，自动退出
	if (list == NULL)
	{
		printf("内存申请失败！");
		return 0;
	}
	list->next = NULL; // 初始化头节点

	while (1)
	{
		printf("************************************************************\n");
		printf("\t\t\t  单链表ADT\n");
		printf("************************************************************\n");
		printf("\t\t\t1.初始化链表\n");
		printf("\t\t\t2.删除链表\n");
		printf("\t\t\t3.插入节点\n");
		printf("\t\t\t4.删除节点\n");
		printf("\t\t\t5.遍历链表\n");
		printf("\t\t\t6.查找数值\n");
		printf("\t\t\t7.反转链表\n");
		printf("\t\t\t8.判断是否有环\n");
		printf("\t\t\t9.查找中间节点\n");
		printf("************************************************************\n");
		printf("请输入选择功能，按字母退出\n");
		// 获取缓冲区数据
		int input;
		if (scanf_s("%d", &input) != 1)
		{
			printf("ヾ(￣▽￣)Bye~Bye~");
			return 0;
		}
		// 若未初始化则无法执行其他命令
		if (list->next == NULL && input != 1)
		{
			char ch;
			while ((ch = getchar()) != '\n' && ch != EOF)
				;
			printf("链表为空，请先初始化\n按回车键继续......");
			if (getchar() == '\n')
			{
				system("cls");
			}
			continue;
		}

		switch (input)
		{
		case 1:
			InitList(list);
			break;
		case 2:
			DestroyList(list);
			break;
		case 3:
			printf("请输入插入位置的前一个节点数据（默认插入第一个匹配节点后）：");
			ElemType site;
			scanf_s("%d", &site);
			printf("请给要插入的节点赋值：");
			LNode *value = malloc(sizeof(LNode));
			scanf_s("%d", value);
			InsertList(list, site, value);
			break;
		case 4:
			printf("请输入要删除节点的前一个结点数据：");
			scanf_s("%d", &site);
			DeleteList(list, site, &value);
			break;
		case 5:
			TraverseList(list);
			break;
		case 6:
			printf("请输入要查找的数值：");
			int num;
			scanf_s("%d", &num);
			SearchList(list, num);
			break;
		case 7:
			list = ReverseList(list);
			break;
		case 8:
			IsLoopList(list);
			break;
		case 9:
			num = FindMidNode(list);
			printf("中间节点数字为：%d", num);
			break;
		default:
			printf("请输入正确指令！");
		}
		// 清空缓冲区
		char temp = '0';
		while ((temp = getchar()) != '\n' && temp != EOF)
			;
		printf("\n按回车键继续......");
		if (getchar())
		{
			system("cls"); // 清屏
		}
	}
	// 释放链表
	DestroyList(list);
	free(list);
	return 0;
}
