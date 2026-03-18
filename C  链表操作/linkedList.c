#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
/**
 *  @name        : Status InitList(LinkList *L);
 *	@description : initialize an empty linked list with only the head node without value
 *	@param	     : L(the head node)
 *	@return	     : Status
 *  @notice      : None
 */
Status InitList(LinkedList L)
{
    LNode *now = L;
    int length;
    printf("请输入要初始化的链表长度：");
    while (scanf_s("%d", &length) != 1 || length < 1)
    {
        printf("请输入正确长度！\n");
        gets();
    }
    for (int i = 0; i < length; i++)
    {
        LNode *next = malloc(sizeof(LNode));
        if (next != NULL)
        {
            next->data = i;
            now->next = next;
            now = now->next;
        }
    }
    now->next = NULL;
    printf("初始化链表成功！");
    return SUCCESS;
}

/**
 *  @name        : void DestroyList(LinkedList *L)
 *	@description : destroy a linked list, free all the nodes
 *	@param	     : L(the head node)
 *	@return	     : None
 *  @notice      : None
 */
void DestroyList(LinkedList L)
{
    LNode *now, *next;
    now = L->next;
    while (now)
    {
        next = now->next;
        free(now);
        now = next;
    }
    printf("成功删除链表！");
    L->next = NULL;
}

/**
 *  @name        : Status InsertList(LNode *p, LNode *q)
 *	@description : insert node q after node p
 *	@param	     : p, q
 *	@return	     : Status
 * 	@notice      : None
 */
Status InsertList(LinkedList L, ElemType p, LNode *q)
{
    LNode *now = L;
    while (now->next != NULL)
    {
        now = now->next;
        if (now->data == p)
        {
            q->next = now->next;
            now->next = q;
            printf("插入成功！");
            return SUCCESS;
        }
    }
    printf("找不到对应数值！");
    return ERROR;
}

/**
 *  @name        : Status DeleteList(LNode *p, ElemType *e)
 *	@description : delete the first node after the node p and assign its value to e
 *	@param	     : p, e
 *	@return	     : Status
 *  @notice      : None
 */
Status DeleteList(LinkedList L, ElemType p, ElemType *e)
{
    LNode *now = L;
    while (now->next != NULL)
    {
        now = now->next;
        if (now->data == p)
        {
            *e = now->next->data;
            LNode *temp = now;
            now = now->next;
            temp->next = now->next;
            printf("删除成功，被删除的节点数据为：%d", *e);
            return SUCCESS;
        }
    }
    printf("找不到该数字！");
    return ERROR;
}

/**
 *  @name        : void TraverseList(LinkedList L, void (*visit)(ElemType e))
 *	@description : traverse the linked list and call the funtion visit
 *	@param	     : L(the head node), visit
 *	@return	     : None
 *  @notice      : None
 */
void TraverseList(LinkedList L)
{
    LNode *now = L;
    printf("链表数值如下：");
    while (now->next != NULL)
    {
        now = now->next;
        printf("%d  ", now->data);
    }
}

/**
 * 	@name        : Status SearchList(LinkedList L, ElemType e)
 *	@description : find the first node in the linked list according to e
 *	@param	     : L(the head node), e
 *	@return	     : Status
 *  @notice      : None
 */
Status SearchList(LinkedList L, ElemType e)
{
    LNode *now = L;
    int count = 0;
    while (now->next != NULL)
    {
        now = now->next;
        if (now->data == e)
        {
            printf("找到该数值，在%d位置（从0位置开始）", count);
            return SUCCESS;
        }
        count++;
    }
    printf("找不到该数字！");
    return ERROR;
}

/**
 *  @name        : Status ReverseList(LinkedList *L)
 *	@description : reverse the linked list
 *	@param	     : L(the head node)
 *	@return	     : Status
 *  @notice      : None
 */
LinkedList ReverseList(LinkedList L)
{
    LNode *head = L->next;
    LNode *newHead = NULL;
    LNode *node;
    while (head != NULL)
    {
        node = head;
        head = head->next;
        node->next = newHead;
        newHead = node;
    }
    LNode *finalNode = malloc(sizeof(LNode));
    if (finalNode != NULL)
    {
        finalNode->next = newHead;
        printf("反转链表成功！");
        return finalNode;
    }
    else
    {
        printf("反转链表失败！请重试");
        return 0;
    }
}

/**
 *  @name        : Status IsLoopList(LinkedList L)
 *	@description : judge whether the linked list is looped
 *	@param	     : L(the head node)
 *	@return	     : Status
 *  @notice      : None
 */
Status IsLoopList(LinkedList L)
{
    LNode *slow = L;
    LNode *fast = L;
    while (fast->next && fast->next->next)
    {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast)
        {
            printf("该链表有环");
            return SUCCESS;
        }
    }
    printf("该链表无环");
    return ERROR;
}

/**
 * 	@name        : LNode* FindMidNode(LinkedList *L)
 *	@description : find the middle node in the linked list
 *	@param	     : L(the head node)
 *	@return	     : LNode
 *  @notice      : choose to finish
 */
ElemType FindMidNode(LinkedList L)
{
    LNode *slow = L;
    LNode *fast = L;
    while (fast && fast->next)
    {
        slow = slow->next;
        fast = fast->next->next;
    }
    if (slow != NULL)
    {
        ElemType e = slow->data;
        return e;
    }
    return 0;
}
