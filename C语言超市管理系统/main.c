/*
 * @Author: wz
 * @Date: 2021-05-02 15:08:40
 * @LastEditTime: 2021-05-18 14:29:07
 * @LastEditors: Please set LastEditors
 * @Description: 超市商品信息管理系统源文件
 * @FilePath: \Program\main.c
 */
#include <stdio.h>   // 标准输入输出头文件
#include <windows.h> // 声明了大部分windows API
#include <conio.h>   // 控制台输入输出头文件
#include <time.h>    // 日期和时间头文件
// #include "function.h" // 自建函数头文件

#define adminPwd "123" // 管理员密码

struct Goods // 商品结构体
{
    char type[10];  // 类型
    char name[15];  // 名称
    char brand[10]; // 品牌
    int stock;      // 库存
    int sales;      // 销量
    float price;    // 价格
} g[100], *fg[100]; // 商品结构体数组、过滤商品结构体指针数组

struct User // 用户结构体
{
    char name[20];     // 用户名称
    char password[20]; // 账户密码
    char phone[12];    // 手机号码
    char address[20];  // 收货地址
    char regTime[15];  // 注册时间
    int orderNum;      // 用户订单数量（本来想
    struct User *next; // 指向下一个用户结构体变量的指针
} *cU, *uH, *uE;       // 当前用户结构体指针、用户链表头、尾节点指针

struct Item // 购物项结构体
{
    struct Goods goods; // 包含商品
    struct User owner;  // 购物车主人
    int number;         // 商品数量
    float cost;         // 价格小计
} it[10];               // 购物项结构体数组

struct Order // 订单结构体
{
    struct Item item;   // 包含购物项
    char creatTime[15]; // 生成时间
    char status[10];    // 状态
    char address[20];   // 派送地址
    struct Order *next; // 指向下一个订单结构体变量的指针
} **cU_o, *oH, *oE;     // 当前用户订单结构体二级指针、订单结构体链表头、尾节点指针

int goodsNum, // 商品数量
    userNum,  // 用户数量
    itemNum,  // 购物项数量
    orderNum, // 订单数量
    fgNum,    // 过滤商品数量
    cU_oNum,  // 当前用户订单数量
    sortWay;  // 排序方式

// 主函数入口
int main()
{
    initialLoad(); // 初始化加载
    loginMenu();   // 登录界面
}

// 初始化加载
void initialLoad()
{
    uH = (struct User *)calloc(1, sizeof(struct User));   // 开辟用户结构体链表头节点（calloc可进行初始化）
    oH = (struct Order *)calloc(1, sizeof(struct Order)); // 开辟订单结构体链表头节点
    system("mkdir Data");
    FILE *fp;                               // 文件指针
    fp = fopen("Data\\商品信息.txt", "a+"); // 以追加可读写模式打开文本文件并返回文件地址,若不存在则自动创建空文件

    if (getc(fp) && !feof(fp)) // 如果文件不为空
    {
        rewind(fp);                                     // 回到文件头部地址
        struct Goods *gp = g;                           // 商品结构体指针
        for (goodsNum = 0; !feof(fp); gp++, goodsNum++) // 按行获取文件所有数据并依次储存于商品结构体数组
        {
            fscanf(fp, "%s %s %f %d %s %d\n", &gp->type, &gp->name, &gp->price, &gp->stock, &gp->brand, &gp->sales);
            fg[goodsNum] = gp; // 初始化过滤商品列表
        }
        fgNum = goodsNum; // 初始化过滤商品数量
    }

    struct User *up = uH;                    // 用户结构体链表遍历指针
    freopen("Data\\用户信息.txt", "a+", fp); // 关闭fp指向的文件缓冲区并重新打开新的文件缓冲区
    if (getc(fp) && !feof(fp))
    {
        rewind(fp);
        for (userNum = 0; !feof(fp); userNum++) // 按行获取文件所有数据并依次储存于用户结构体链表
        {
            fscanf(fp, "%s %s %s %s %s %d\n", &up->name, &up->password, &up->phone, &up->address, &up->regTime, &up->orderNum);
            up->next = (struct User *)calloc(1, sizeof(*uH)); // 开辟下一个用户链表节点
            up = up->next;                                    // 指针移动到下一个节点
        }
    }
    uE = up; // 尾节点指针指向遍历指针最后指向的空节点（可能是头节点），由于使用了calloc，不需要让该节点的next指针置null，便可用p->next判断是否到尾

    struct Order *op = oH;
    freopen("Data\\订单信息.txt", "a+", fp);
    if (getc(fp) && !feof(fp))
    {
        rewind(fp);
        for (orderNum = 0; !feof(fp); orderNum++)
        {
            fscanf(fp, "%s %s %d %f %s %s %s %s\n", &op->item.owner.name, &op->item.goods.name, &op->item.number, &op->item.cost, &op->creatTime, &op->status, &op->address, op->item.owner.phone);
            op->next = (struct Order *)calloc(1, sizeof(*oH));
            op = op->next;
        }
    }
    oE = op;

    fclose(fp); // 关闭文件指针所指缓冲区
}

// 用户登入加载
void userLoad()
{
    struct Item *p = it; // 购物项结构体指针
    char cartName[40];   // 购物车文件名

    sprintf(cartName, "Data\\%s的购物车.txt", cU->name); // 字符串格式化赋值
    FILE *fp = fopen(cartName, "a+");
    if (getc(fp) && !feof(fp))
    {
        rewind(fp);
        for (itemNum = 0; !feof(fp); p++, itemNum++) // 按行获取文件所有数据并依次储存于购物项结构体数组
        {
            // 格式化扫描文件缓冲区。结尾的\n用于过滤换行符，避免误判文件是否结束
            fscanf(fp, "%s %s %d %f %d\n", &p->owner.name, &p->goods.name, &p->number, &p->cost, &p->goods.stock);
        }
    }
    fclose(fp);

    cU_oNum = 0;                                            // 初始化商品数量
    cU_o = (struct Order **)malloc(cU->orderNum * 8 + 800); // 开辟100+个订单结构体指针连续空间，32位系统为200+个
    for (struct Order *p = oH; p->next; p = p->next)        // 遍历已加载的订单结构体链表
    {
        if (!strcmp(p->item.owner.name, cU->name)) // 若订单主人为当前用户
        {
            cU_o[cU_oNum++] = p; // 将该订单添加到当前用户订单
        }
    }
}

// 登录注册菜单
void loginMenu()
{
    system("cls");
    // system("mode con cols=48 lines=25");  // 设置程序窗口大小
    system("title 超市商品信息管理系统"); // 设置程序窗口标题
    puts("\t超市商品管理系统");
    puts("============登录菜单============");
    puts("\t1.管理员登录");
    puts("\t2.用户登录");
    puts("\t3.用户注册");
    puts("\t0.退出系统");
    puts("================================\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xf0);
    puts("请选择您的操作");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0f);
    fflush(stdin);
    switch (getch())
    {
    case '1':
        adminLogin(); // 进入管理员登录页面
        break;
    case '2':
        userLogin(); // 进入用户登录页面
        break;
    case '3':
        userRegist(); // 进入用户注册页面
        break;
    case '0':
        exit(0); // 正常终止系统程序
    default:
        puts("\a输入有误！");
        system("pause");
    }
    loginMenu(); // 继续当前页面
}

// 管理员登录页面
void adminLogin()
{
    system("cls");
    puts("请输入管理员密码：");
    char input[10];
    while (!scanf("%9s", &input)) // 当输入为空，继续接受输入，用于减少密码报错频率
        ;
    if (strcmp(input, adminPwd)) // 若输入密码不匹配
    {
        puts("\a密码错误！");
        keepOrNot(adminLogin); // 询问是否继续登录
        return;                // 返回上一页面，直至登陆注册菜单
    }
    adminMenu(); // 进入管理员菜单
}

// 维持当前页面，参数为当前函数
void keepOrNot(void (*func)())
{
    fflush(stdin); // 清空标准输入缓冲区
    puts("\n请按任意键继续. . .（Esc：返回菜单）");
    if (getch() != 27) // 若按下非Esc键
    {
        system("cls"); // 清屏
        func();        // 继续当前页面
    }
}

// 管理员菜单
void adminMenu()
{
    system("cls");
    // 打印管理员菜单
    puts("\t超市商品管理系统");
    puts("===========管理员菜单===========");
    puts("\t1.编辑商品信息");
    puts("\t2.查找商品信息");
    puts("\t3.统计销售信息");
    puts("\t4.注册用户信息");
    puts("\t5.用户订单信息");
    puts("\t0.注销（保存）");
    puts("================================\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xf0);
    puts("请选择您的操作");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0f);
    fflush(stdin);
    switch (getch())
    {
    case '1':
        goodsEdit(); // 进入商品编辑页面
        break;
    case '2':
        goodsQuery(); // 进入商品查询页面
        break;
    case '3':
        goodsStatis(); // 进入销售数据页面
        break;
    case '4':
        userInfo(); // 进入用户信息页面
        break;
    case '5':
        orderInfo(); // 进入订单信息页面
        break;
    case '0':
        puts("是否保存商品信息？（Enter：保存）");
        if (getch() == 13)
        {
            saveInfo('g'); // 保存商品信息
            puts("保存成功！");
        }
        return; // 返回上一页面，直至登录注册菜单
    default:
        puts("\a请输入正确指令！");
        system("pause");
    }
    adminMenu();
}

// 商品编辑页面
void goodsEdit()
{
    system("cls");
    if (!goodsNum) // 若商品数量为0
    {
        puts("您还没有添加商品！");
        keepOrNot(goodsAdd);
        return; // 返回上一页，直至管理员菜单
    }
    int *pageNum = goodsList(); // 显示商品列表页
    puts("请选择：");
    puts("\t1.添加商品\t2.修改商品\t3.删除商品\t↑/↓.翻页\tEsc.返回菜单");
    switch (getch())
    {
    case 224: // 方向键扫描码（组合键1）
        switch (getch())
        {
        case 72: // 上箭头键值2
            (*pageNum)--;
            break;
        case 80: // 下箭头键值2
            (*pageNum)++;
        }
        break;
    case '1':
        goodsAdd(); // 进入商品添加页面
        break;
    case '2':
        goodsMod(); // 进入商品修改页面
        break;
    case '3':
        goodsDel(); // 进入商品删除页面
        break;
    case 27:
        return; // 返回上一页面，直至管理员菜单
    default:
        puts("\a请输入正确指令！");
        system("pause");
    }
    goodsEdit();
}

// 商品列表页
int *goodsList()
{
    static int pageNum = 1;

    if (pageNum > (fgNum + 9) / 10) // 若页码过大
    {
        pageNum = (fgNum + 9) / 10; // 改为最大页码
    }
    else if (pageNum < 1) // 若页码过小
    {
        pageNum = 1; // 改为最小页码
    }
    // 打印商品列表页
    printf("第%d页 （共%d页）\n", pageNum, (fgNum + 9) / 10);
    puts("=============================================================================");
    printf("%5s |%10s |%15s |%10s |%10s |%10s |%5s\n", "编号", "类型", "名称", "价格", "库存", "品牌", "销量");
    for (int i = (pageNum - 1) * 10; i < fgNum && i < pageNum * 10; i++) // 每页打印10行信息
    {
        printf("%5d |%10s |%15s |%10.2f |%10d |%10s |%5d\n", i, fg[i]->type, fg[i]->name, fg[i]->price, fg[i]->stock, fg[i]->brand, fg[i]->sales);
    }
    puts("=============================================================================");
    return &pageNum;
}

// 商品添加页面
void goodsAdd()
{
    system("cls");
    struct Goods t; // 临时商品结构体
    if (goodsNum)
    {
        goodsList();
    }
    puts("请依次输入要添加的商品信息：（空格或换行隔开）");
    puts("顺序：类别 名称 价格 库存量 品牌");
    // 若商品输入不规范
    if (scanf("%9s %14s %f %d %9s", &t.type, &t.name, &t.price, &t.stock, &t.brand) < 5)
    {
        puts("\a请输入正确格式！");
        keepOrNot(goodsAdd);
        return;
    }
    for (int i = 0; i < goodsNum; i++)
    {
        if (!strcmp(g[i].name, t.name)) // 若输入商品名称已存在
        {
            puts("\a商品名与以下商品重复！请输入新的商品名称：");
            printf("%10s |%15s |%10s |%10s |%10s |%5s\n", "类别", "名称", "价格", "库存量", "品牌", "销量");
            printf("%10s |%15s |%10.2f |%10d |%10s |%5d\n", g[i].type, g[i].name, g[i].price, g[i].stock, g[i].brand, g[i].sales);
            while (!scanf("%14s", t.name)) // 若输入为空则继续输入。
            {
                puts("\a输入为空！（Enter：继续输入）");
                if (getch() != 13)
                {
                    return;
                }
                puts("请重新输入商品名称：");
            }
            i = -1; // 初始化循环控制变量
        }
    }
    memcpy(&g[goodsNum], &t, sizeof(t)); // 将临时结构体拷贝添加到商品结构体数组
    g[goodsNum].sales = 0;               // 初始化商品销量
    fg[fgNum++] = &g[goodsNum++];        // 更新过滤商品结构体指针数组
    puts("添加成功！");
    system("pause");
}

void goodsDel()
{
    system("cls");
    goodsList();
    int gId = 0; // 商品编号
    puts("请输入要删除的商品编号：");
    if (!scanf("%d", &gId) || gId < 0 || gId >= goodsNum) // 若输入非法或没有对应编号
    {
        puts("\a没有该编号！");
        fflush(stdin);
        keepOrNot(goodsDel);
        return;
    }
    puts("要删除的商品信息如下，按回车键（Enter)确认");
    printf("%10s |%15s |%10s |%10s |%10s\n", "类别", "名称", "价格", "库存量", "品牌");
    printf("%10s |%15s |%10.2f |%10d |%10s\n", g[gId].type, g[gId].name, g[gId].price, g[gId].stock, g[gId].brand);
    if (getch() != 13) // 若按下非回车键
    {
        puts("已取消删除！");
        keepOrNot(goodsDel);
        return;
    }
    // 删除结构体数组变量并自动补位
    for (int i = gId; i < goodsNum - 1; i++)
    {
        memcpy(&g[i], &g[i + 1], sizeof(*g));
    }
    // 更新商品数量
    goodsNum--;
    fgNum--;
    puts("删除成功！");
    system("pause");
}

// 商品修改页面
void goodsMod()
{
    system("cls");
    goodsList();
    static int gId = 0; // 商品编号
    puts("请输入要修改的商品编号：");
    if (!scanf("%d", &gId) || gId < 0 || gId >= goodsNum)
    {
        puts("\a没有该编号！");
        fflush(stdin);
        keepOrNot(goodsMod);
        return;
    }
    puts("请输入新的商品信息：（空格或换行隔开）");
    puts("顺序：类别->名称->价格->库存量->品牌");
    struct Goods t;
    while (scanf("%9s %14s %f %d %9s", &t.type, &t.name, &t.price, &t.stock, &t.brand) < 5)
    {
        puts("\a格式错误！（Enter：继续输入）");
        fflush(stdin);
        if (getch() != 13)
        {
            return;
        }
        puts("请重新输入商品信息：");
    }
    if (t.name != g[gId].name) // 若商品名称被修改
    {
        for (int i = 0; i < goodsNum; i++) // 遍历所有商品信息
        {
            if (!strcmp(g[i].name, t.name)) // 若商品名称重复
            {
                puts("\a商品名已存在！请输入新的商品名称：");
                fflush(stdin);
                while (!scanf("%14s", t.name))
                {
                    puts("\a输入为空！（Enter：继续输入）");
                    if (getch() != 13)
                    {
                        return;
                    }
                    puts("请重新输入商品名称：");
                }
                i = -1;
            }
        }
    }

    memcpy(&g[gId], &t, sizeof(t));
    puts("修改成功！");
    system("pause");
}

// 商品查询页面
void goodsQuery()
{
    system("cls");
    puts("商品信息如下：");
    if (!goodsNum)
    {
        puts("空空如也~");
        system("pause");
        return;
    }
    int *pageNum = goodsList();
    puts("请选择商品查找方式：");
    puts("\t1.按类型\t2.按名称\t3.按品牌\t↑/↓.翻页\tEsc.返回菜单");
    switch (getch())
    {
    case 224:
        switch (getch())
        {
        case 72:
            (*pageNum)--;
            break;
        case 80:
            (*pageNum)++;
        }
        break;
    case '1':
        puts("请输入类型：");
        goodsSearch(g->type); // 按类型搜索
        break;
    case '2':
        puts("请输入名称：");
        goodsSearch(g->name); // 按名称搜索
        break;
    case '3':
        puts("请输入品牌：");
        goodsSearch(g->brand); // 按品牌搜索
        break;
    case 27:
        return;
    default:
        puts("\a请输入正确指令！");
        system("pause");
    }
    goodsQuery();
}

// 商品搜索模块，参数为商品结构体数组首元素的对应成员地址
void goodsSearch(char *attr)
{
    int found = 0; // 是否找到商品
    char s[20];
    while (!scanf("%19s", s))
        ;
    // 遍历商品结构体数组元素的对应成员
    for (int i = 0; i < goodsNum; i++, attr += sizeof(*g))
    {
        if (strstr(attr, s)) // 若包含输入字符串
        {
            if (!found) // 若第一次找到匹配商品
            {
                puts("找到匹配商品信息如下：");
                puts("=============================================================================");
                printf("%5s |%10s |%15s |%10s |%10s |%10s |%5s\n", "编号", "类别", "名称", "价格", "库存量", "品牌", "销量");
            }
            printf("%5d |%10s |%15s |%10.2f |%10d |%10s |%5d\n", i, g[i].type, g[i].name, g[i].price, g[i].stock, g[i].brand, g[i].sales);
            found = 1;
        }
    }
    puts("=============================================================================");
    if (!found)
    {
        puts("没有找到匹配的商品！");
    }
    system("pause");
}

// 已注册用户信息
void userInfo()
{
    printf("已注册用户数量：%d\n", userNum);
    if (!userNum)
    {
        puts("空空如也！");
        system("pause");
        return;
    }
    puts("================================================================================");

    printf("%15s |%20s |%12s |%15s |%10s\n", "用户名称", "收货地址", "手机号", "注册时间", "订单数");
    for (struct User *p = uH; p->next; p = p->next)
    {
        printf("%15s |%20s |%12s |%15s |%10d\n", p->name, p->address, p->phone, p->regTime, p->orderNum);
    }
    puts("================================================================================");
    system("pause");
    // puts("请输入用户名：");
    // char s[10];
    // int findOrNot = 0;
    // gets(s);
    // for (int i = 0; i < userNum; i++)
    // {
    //    if (!strcmp(s, u[i].name))
    //    {
    //        findOrNot = 1;
    //        puts("找到用户信息如下：");
    //        printf("%10s |%20s |%12s |%15s\n", "用户名", "收货地址", "手机号", "注册时间");
    //        printf("%10s |%20s |%12s |%15s\n", u[i].name, u[i].address, u[i].phone, u[i].regTime);
    //    }
    // }
    // if (!findOrNot)
    // {
    //    puts("未找到该用户！");
    // }
}

// 订单详细信息
void orderInfo()
{
    static int pageNum = 1;

    system("cls");
    printf("已提交订单数量：%d\n", orderNum);
    if (!orderNum)
    {
        puts("空空如也~");
        system("pause");
        return;
    }
    printf("第%d页（共%d页）\n", pageNum, (orderNum + 9) / 10);
    puts("======================================================================================================================");
    printf("%5s |%15s |%5s |%10s |%15s |%10s |%10s |%20s |%12s\n", "编号", "商品名称", "数量", "总价格", "下单时间", "订单状态", "收件人", "派送地址", "联系号码");
    struct Order *p = oH;
    for (int i = (pageNum - 1) * 10; i < orderNum && i < pageNum * 10 && p->next; i++)
    {
        printf("%5d |%15s |%5d |%10.2f |%15s |%10s |%10s |%20s |%12s\n", i, p->item.goods.name, p->item.number, p->item.cost, p->creatTime, p->status, p->item.owner.name, p->address, p->item.owner.phone);
        p = p->next;
    }
    puts("======================================================================================================================");
    puts("请选择：");
    puts("\t1.查找订单\t2.修改订单状态\t↑↓.翻页\tEsc.返回菜单");
    switch (getch())
    {
    case '1':
        orderSearch();
        break;
    case '2':
        orderModify();
        break;
    case 224:
        switch (getch())
        {
        case 72:
            if (pageNum > 1) // 若页码大于最小页码
                pageNum--;
            break;
        case 80:
            if (pageNum < (orderNum + 9) / 10) // 若页码小于最大页码
                pageNum++;
            break;
        }
        break;
    case 27:
        return;
    default:
        puts("\a输入有误！");
        system("pause");
    }
    orderInfo();
}

void orderSearch()
{
    int found = 0; // 是否找到订单
    char s[20];
    puts("请输入订单包含的商品名关键字：");
    while (!scanf("%19s", s))
        ;
    // 遍历订单结构体链表元素的包含商品名称
    for (struct Order *p = oH; p->next; p = p->next)
    {
        if (strstr(p->item.goods.name, s)) // 若包含输入字符串
        {
            if (!found) // 若第一次找到匹配订单
            {
                puts("找到匹配订单信息如下：");
                puts("===============================================================================================================");
                printf("%15s |%5s |%10s |%15s |%10s |%10s |%20s |%12s\n", "商品名称", "数量", "总价格", "下单时间", "订单状态", "收件人", "派送地址", "联系号码");
            }
            printf("%15s |%5d |%10.2f |%15s |%10s |%10s |%20s |%12s\n", p->item.goods.name, p->item.number, p->item.cost, p->creatTime, p->status, p->item.owner.name, p->address, p->item.owner.phone);
            found = 1;
        }
    }
    puts("===============================================================================================================");
    if (!found)
    {
        puts("没有找到匹配的订单！");
    }
    system("pause");
}

void orderModify()
{
    puts("请输入订单编号：");
    int oId = 0; // 订单编号
    while (!scanf("%d", &oId) || oId < 0 || oId >= orderNum)
    {
        puts("\a没有该编号！（Enter：继续输入）");
        fflush(stdin);
        if (getch() != 13)
        {
            return;
        }
        puts("请重新输入编号：");
    }
    char status[][10] = {"已发货", "已到货", "已收货", "未发货", "暂时缺货"}; // 订单状态
    puts("请选择状态类型：");
    printf("\t1.%s\t2.%s\t3.%s\t4.%s\t5.%s\n", status[0], status[1], status[2], status[3], status[4]);
    char press;
    while ((press = getch()) < '1' || press > '5')
        ;
    struct Order *p = oH;
    for (int i = 0; i < oId; i++)
    {
        p = p->next;
    }
    strcpy(p->status, status[press - '1']);
    FILE *fp = fopen("Data\\订单信息.txt", "r+"); // 以可读写不清空方式打开订单文件
    fseek(fp, oId * 106 + 60, 0);                 // 移动文件指针到订单状态的位置
    fprintf(fp, "%10s", p->status);               // 格式化打印新的订单状态
    fclose(fp);
    puts("修改成功！");
    system("pause");
}

// 销售数据页面
void goodsStatis()
{
    system("cls");
    puts("销售信息如下：");
    if (!goodsNum)
    {
        puts("空空如也~");
        system("pause");
        return;
    }
    static int pageNum = 1; // 销售数据页码
    printf("第%d/%d页\n", pageNum, (goodsNum + 9) / 10);
    puts("===================================================");

    printf("%15s |%10s |%10s |%10s\n", "商品名称", "销量", "销售额", "库存");
    for (int i = (pageNum - 1) * 10; i < goodsNum && i < pageNum * 10; i++)
    {
        printf("%15s |%10d |%10.2f |%10d\n", g[i].name, g[i].sales, g[i].sales * g[i].price, g[i].stock);
    }
    puts("===================================================");

    puts("请选择排序方式：");
    puts("\t1.按销售量\t2.按销售额\t↑/↓.翻页\tEsc.返回菜单");
    switch (getch())
    {
    case 224:
        switch (getch())
        {
        case 72:
            if (pageNum > 1) // 若页码大于最小页码
                pageNum--;
            break;
        case 80:
            if (pageNum < (goodsNum + 9) / 10) // 若页码小于最大页码
                pageNum++;
            break;
        }
        break;
    case '1':
        sortWay = 0; // 排序方式为销售量
        // 快速排序，按排序方式对g数组的fgNum个元素进行排序
        qsort(&g->sales, fgNum, sizeof(*g), compare);
        break;
    case '2':
        sortWay = 1; // 排序方式为销售额
        qsort(&g->sales, fgNum, sizeof(*g), compare);
        break;
    case 27:
        return;
    default:
        puts("\a请输入正确指令！");
        system("pause");
    }
    goodsStatis();
}

// 排序方式，sqort的回调函数
int compare(const void *a, const void *b)
{
    if (!sortWay)
        return *(int *)b - *(int *)a; // 销售量降序排列
    else
    {
        float c = *(float *)((int *)a + 1), // 对a所指int地址的下一个float地址取值，即销量所属结构体变量的价格
            d = *(float *)((int *)b + 1);
        return *(int *)b * d - *(int *)a * c; // 销售额降序排列
    }
}

// void quickSort(struct Goods *arr[], int lift, int right)
// {
//     if (lift >= right)
//         return;
//     int i = lift, j = right, key = arr[lift]->sales; // 选择第一个数为key（挖坑）
//     while (i < j)
//     {
//         while (i < j && arr[j]->sales >= key) // 从右向左找第一个小于key的值
//             j--;                              // 左移位置
//         if (i < j)
//             arr[i++]->sales = arr[j]->sales; // “填坑”（i++减少不必要的比较）
//         while (i < j && arr[i]->sales < key) // 从左向右找第一个大于key的值
//             i++;                             // 右移位置
//         if (i < j)
//             arr[j--]->sales = arr[i]->sales; // “填坑”（拆东墙补西墙）
//     }
//     // 左边比key大的数都已填到右边，右边同样（中间的坑留给key）
//     arr[i]->sales = key;          // “填坑”
//     quickSort(arr, lift, i - 1);  // key左边数组递归调用
//     quickSort(arr, i + 1, right); // key右边数组递归调用
// }

// 保存确认，参数为要保存的信息
void saveInfo(char info)
{
    FILE *fp;
    if (info == 'g')
    {
        fp = fopen("Data\\商品信息.txt", "w");
        for (int i = 0; i < goodsNum; i++)
        {
            fprintf(fp, "%10s %20s %10.2f %5d %10s %5d\n", g[i].type, g[i].name, g[i].price, g[i].stock, g[i].brand, g[i].sales);
        }
    }
    else if (info == 'i')
    {
        char cartName[40];
        sprintf(cartName, "Data\\%s的购物车.txt", cU->name);
        freopen(cartName, "w", fp);
        for (int i = 0; i < itemNum; i++)
        {
            fprintf(fp, "%20s %20s %5d %10.2f %5d\n", it[i].goods.name, it[i].goods.name, it[i].number, it[i].cost, it[i].goods.stock);
        }
    }
    fclose(fp);
}

void userLogin()
{
    system("cls");
    char input1[10],
        input2[10];
    printf("请输入用户名：");
    while (!scanf("%19s", input1))
        ;
    fflush(stdin);
    printf("请输入用户密码：");
    while (!scanf("%9s", input2))
        ;
    int found = 0;
    for (struct User *p = uH; p->next; p = p->next)
    {
        if (!strcmp(input1, p->name) && !strcmp(input2, p->password)) // 若账号密码都正确
        {
            found = 1;
            cU = p; // 设置为当前用户
            puts("登录成功！");
            system("pause");
            userLoad(); // 加载用户信息
            userMenu(); // 进入用户菜单
            break;
        }
    }
    if (!found)
    {
        puts("\a账号或密码错误！");
        keepOrNot(userLogin);
    }
}

void userRegist()
{
    system("cls");
    puts("请输入注册信息");
    struct User t;
    puts("用户名：（一旦填写将无法更改）");
    while (!scanf("%19s", t.name))
        ;
    fflush(stdin);
    for (struct User *p = uH; p->next; p = p->next)
    {
        if (!strcmp(t.name, p->name))
        {
            puts("用户名已被注册，换一个吧！");
            keepOrNot(userRegist);
            return;
        }
    }
    printf("用户密码：");
    while (!scanf("%14s", t.password))
        ;
    printf("手机号码：");
    fflush(stdin);
    while (!scanf("%14[0-9]", t.phone) || strlen(t.phone) != 11)
    {
        puts("\a手机号码应为11位数字，请重新输入：");
        fflush(stdin);
    }
    memcpy(uE, &t, sizeof(t));
    strcpy(uE->address, "尚未填写");
    uE->orderNum = 0;
    strcpy(uE->regTime, getTime());
    FILE *fp = fopen("Data\\用户信息.txt", "a");
    fprintf(fp, "%20s %20s %12s %20s %15s\n", uE->name, uE->password, uE->phone, uE->address, uE->regTime);
    fclose(fp);
    puts("注册成功！");
    userNum++;
    uE = uE->next = (struct User *)calloc(1, sizeof(*uH));
    system("pause");
}

// 时间获取函数
char *getTime()
{

    time_t rawTime;      // 原始时间
    struct tm *timeInfo; // 本地时间
    static char buf[15]; // 静态时间缓冲区

    time(&rawTime);                                // 获取原始时间并储存于rawTime
    timeInfo = localtime(&rawTime);                // 本地化原始时间并储存于timeInfo
    strftime(buf, 15, "%m-%d/%H:%M:%S", timeInfo); // 格式化本地时间并储存于buf
    return buf;
}

// 用户菜单
void userMenu()
{
    system("cls");
    puts("\t超市商品订购系统");
    puts("============用户菜单============");
    puts("\t1.购物界面");
    puts("\t2.我的购物车");
    puts("\t3.我的订单");
    puts("\t4.个人信息");
    // puts("\t5.账户余额");
    puts("\t0.注销身份");
    puts("================================\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xf0);
    puts("请选择您的操作");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0f);
    fflush(stdin);
    switch (getch())
    {
    case '1':
        shopping(); // 进入购物界面
        break;
    case '2':
        myCart(); // 进入我的购物车
        break;
    case '3':
        myOrder(); // 进入我的订单
        break;
    case '4':
        myInfo(); // 进入个人信息
        break;
    case '5':
        // myMoney();
        break;
    case '0':
        return;
    default:
        puts("\a输入有误！");
        system("pause");
    }
    userMenu();
}

// 购物界面
void shopping()
{
    char filtInfo[][20] = {"未设置", "未设置", "关闭"}; // 筛选信息
    while (1)
    {
        system("cls");
        puts("在售商品信息：");
        if (!goodsNum)
        {
            puts("抱歉！我们啥也没有. . .");
            system("pause");
            return;
        }
        puts("筛选条件：");
        printf("\t关键字：%s\t价格：%s\t仅看有货：%s\n", filtInfo[0], filtInfo[1], filtInfo[2]);
        int *pageNum = goodsList();
        puts("请选择：");
        puts("\t1.商品筛选\t2.撤销筛选\t3.商品加购\t↑/↓.翻页\tEsc.返回菜单");
        switch (getch())
        {
        case 224:
            switch (getch())
            {
            case 72:
                (*pageNum)--;
                break;
            case 80:
                (*pageNum)++;
            }
            break;
        case '1':
            goodsFilter(filtInfo); // 调用商品筛选模块
            break;
        case '2':
            for (fgNum = 0; fgNum < goodsNum; fgNum++) // 还原商品列表
            {
                fg[fgNum] = &g[fgNum];
            }
            shopping();
        case '3':
            addToCart(); // 进入商品加购页面
            break;
        case 27:
            for (fgNum = 0; fgNum < goodsNum; fgNum++) // 还原商品列表
            {
                fg[fgNum] = &g[fgNum];
            }
            return;
        default:
            puts("\a请输入正确指令！");
            system("pause");
        }
    }
}

// 商品筛选模块
void goodsFilter(char (*filtInfo)[20])
{
    puts("请选择筛选条件：");
    puts("\t1.按名称\t2.按价格\t3.仅看有货\tEsc.取消筛选");
    int newfgNum = 0; // 过滤后商品数量
    char press;       // 按键对应ASCII码
    // 忽略无效输入
    while (((press = getch()) < '1' || press > '3') && press != 27)
        ;
    switch (press)
    {
    case '1':
        puts("请输入商品关键字：");
        char input[15];
        while (!scanf("%14s", input))
            ;
        // 遍历过滤商品信息（可多次过滤）
        for (int i = 0; i < fgNum; i++)
        {
            if (strstr(fg[i]->name, input)) // 若商品名称包含输入字符串
            {
                fg[newfgNum++] = fg[i];     // 将其添加到过滤后商品列表
                strcpy(filtInfo[0], input); // 拷贝名称筛选信息
            }
        }
        break;
    case '2':
        puts("请输入价格区间：（格式：x.xx-x.xx）");
        float min, // 最低价格
            max;   // 最高价格
        while (scanf("%f-%f", &min, &max) < 2)
        {
            puts("\a格式错误！（Enter：继续输入）");
            fflush(stdin);
            if (getch() != 13)
            {
                return;
            }
            puts("请重新输入价格区间：");
        }
        for (int i = 0; i < fgNum; i++)
        {
            if (fg[i]->price >= min && fg[i]->price <= max) // 若商品价格在价格区间
            {
                fg[newfgNum++] = fg[i];
                sprintf(filtInfo[1], "%.2f-%.2f", min, max); // 格式化拷贝价格筛选信息
            }
        }
        break;
    case '3':
        for (int i = 0; i < fgNum; i++)
        {
            if (fg[i]->stock > 0) // 若商品有货
            {
                fg[newfgNum++] = fg[i];
                strcpy(filtInfo[2], "开启");
            }
        }
        break;
    case 27:
        return;
    }
    if (newfgNum) // 若有筛选结果
    {
        puts("筛选成功！");
        fgNum = newfgNum; // 更新过滤商品数量
    }
    else
    {
        puts("无匹配结果！筛选将不生效");
    }
    system("pause");
}

// 商品加购页面
void addToCart()
{
    system("cls");
    goodsList();
    puts("请输入要加购的商品编号：");
    int gId;
    if (!scanf("%d", &gId) || gId < 0 || gId >= fgNum)
    {
        puts("\a没有该编号！");
        fflush(stdin);
        keepOrNot(addToCart);
        return;
    }
    if (!fg[gId]->stock)
    {
        puts("商品暂时缺货！");
        keepOrNot(addToCart);
        return;
    }
    int exist = 0, // 商品是否已加购
        itId;      // 购物项编号

    // 遍历购物车所有购物项
    for (int i = 0; i < itemNum; i++)
    {
        if (!strcmp(it[i].goods.name, fg[gId]->name)) // 若购物项中含有该商品
        {
            printf("购物车已存在该商品%d件！", it[i].number);
            puts("按回车键（Enter）可继续添加");
            if (getch() != 13)
            {
                keepOrNot(addToCart);
                return;
            }
            exist = 1;
            itId = i; // 保存购物项编号
            break;
        }
    }
    puts("请输入购买数量：");
    int num = 0,                                                         // 购物数量
        max = exist ? fg[gId]->stock - it[itId].number : fg[gId]->stock; // 限购数量
    while (!scanf("%d", &num) || num > max || num < 1)
    {
        fflush(stdin);
        if (num < 1) // 若输入不为数字或小于1
        {
            printf("\a输入必须为正整数！");
        }
        else
        {
            printf("\a库存仅剩%d件！", fg[gId]->stock);
        }
        puts("请重新输入购买数量：");
    }
    if (exist)
    {
        it[itId].number += num;
        it[itId].cost += fg[gId]->price * num;
    }
    else
    {
        // 添加并初始化购物项信息
        memcpy(&it[itemNum].goods, fg[gId], sizeof(*g));
        memcpy(&it[itemNum].owner, cU, sizeof(*uH));
        it[itemNum].number = num;
        it[itemNum].cost = fg[gId]->price * num;
        itemNum++;
    }
    puts("已添加到购物车！");
    system("pause");
}

// 用户购物车
void myCart()
{
    system("cls");
    puts("您的购物车商品清单：");
    if (!itemNum)
    {
        puts("空空如也~");
        system("pause");
        return;
    }
    itemList(); // 打印购物车列表
    puts("请选择：");
    puts("\t1.增改商品\t2.删除商品\t3.清空购物车\t4.下单/结算\t5.保存购物车\tEsc.返回菜单");
    switch (getch())
    {
    case '1':
        itemModify(); // 进入购物项编辑页面
        break;
    case '2':
        itemDelete(); // 进入购物项删除页面
        break;
    case '3':
        itemNum = 0;
        break;
    case '4':
        placeOrder(); // 进入下单页面
        break;
    case '5':
        puts("是否保存购物车信息？（Enter：保存）");
        if (getch() == 13)
        {
            saveInfo('i'); // 保存购物项信息
            puts("保存成功！");
        }
        break;
    case 27:
        return;
    default:
        puts("\a请输入正确指令！");
        system("pause");
    }
    myCart();
}

// 购物车列表
void itemList()
{
    puts("================================================");
    printf("%5s |%15s |%5s |%10s |%5s\n", "编号", "商品名称", "数量", "小计", "库存");
    float total = 0; // 购物车商品总价
    for (int i = 0; i < itemNum; i++)
    {
        printf("%5d |%15s |%5d |%10.2f |%5d\n", i, it[i].goods.name, it[i].number, it[i].cost, it->goods.stock);
        total += it[i].cost;
    }
    puts("================================================");
    printf("总价为：%.2f元\n", total);
}

// 购物项编辑页面
void itemModify()
{
    system("cls");
    itemList();
    puts("请输入要修改的商品编号：");
    int itId; // 购物项编号
    while (!scanf("%d", &itId) || itId < 0 || itId >= itemNum)
    {
        puts("\a没有该编号！请重新输入：");
        fflush(stdin);
    }
    puts("请输入要购买的数量：");
    int num = 0;
    while (!scanf("%d", &num) || num < 1 || num > it[itId].goods.stock)
    {
        if (num < 1)
        {
            puts("\a请输入正整数！");
            fflush(stdin);
        }
        else
        {
            printf("\a库存仅剩%d件！", it[itId].goods.stock);
        }
        puts("Enter：继续购买. . .");
        if (getch() != 13)
        {
            return;
        }
        puts("请重新输入购买数量：");
    }
    it[itId].number = num; // 更新商品数量
    puts("修改成功！");
    system("pause");
}

// 购物项删除页面
void itemDelete()
{
    system("cls");
    itemList();
    puts("请输入要删除的商品编号：");
    int itId;
    while (!scanf("%d", &itId) || itId < 0 || itId >= itemNum)
    {
        puts("\a没有该编号！请重新输入：");
        system("pause");
        fflush(stdin);
    }
    puts("要删除的商品信息如下，按回车键（Enter)确认删除");
    printf("%15s |%5s |%10s |%5s\n", "名称", "数量", "小计", "库存");
    printf("%15s |%5d |%10.2f |%5d\n", it[itId].goods.name, it[itId].number, it[itId].cost, it->goods.stock);
    if (getch() != 13)
    {
        puts("已取消删除！");
        keepOrNot(itemDelete);
        return;
    }
    for (int i = itId; i < itemNum - 1; i++) // 删除购物项并自动补位
    {
        memcpy(&it[i], &it[i + 1], sizeof(*it));
    }
    itemNum--;
    puts("删除成功！");
}

// 下单/结算页面
void placeOrder()
{
    if (strcmp(cU->address, "尚未填写")) // 若用户未填写收获地址
    {
        puts("请确认您的收货地址，按退格键（Backspace）可修改");
        puts(cU->address);
        if (getch() == '\b') // 若按下退格键
        {
            puts("请输入新的收货地址：");
            while (!scanf("%19s", cU->address))
            {
                puts("输入为空！（Esc：不买了）");
                if (getch() == 27)
                {
                    return;
                }
            }
        }
    }
    else
    {
        puts("请输入默认收货地址：");
        while (!scanf("%19s", cU->address))
        {
            puts("输入为空！（Esc：不买了）");
            if (getch() == 27)
            {
                return;
            }
        }
    }
    FILE *fp = fopen("Data\\订单信息.txt", "a"); // 以追加模式打开订单文件
    for (int i = 0; i < itemNum; i++)
    {
        // 初始化订单信息
        memcpy(&oE->item, &it[i], sizeof(*it));
        strcpy(oE->creatTime, getTime());
        strcpy(oE->status, "未发货");
        strcpy(oE->address, cU->address);
        // 将订单信息打印到订单文件
        fprintf(fp, "%10s %15s %5d %10.2f %15s %10s %20s %12s\n", it[i].owner.name, it[i].goods.name, it[i].number, it[i].cost, &oE->creatTime, &oE->status, &oE->address, it[i].owner.phone);
        cU_o[cU_oNum++] = oE; // 更新当前用户订单和全部订单数量
        orderNum++;
        cU->orderNum++;
        oE = oE->next = (struct Order *)calloc(1, sizeof(*oE)); // 开辟下一个节点作为尾节点
        for (int j = 0; j < goodsNum; j++)
        {
            if (!strcmp(g[j].name, it[i].goods.name))
            {
                g[j].stock -= it[i].number;
            }
        }
        saveInfo('g');
    }
    fclose(fp);
    puts("下单成功！");
    itemNum = 0; // 清空购物车
    char cartName[40];
    sprintf(cartName, "Data\\%s的购物车.txt", cU->name);
    fclose(fp = fopen(cartName, "w")); // 清空并关闭购物车文件
    system("pause");
}

// void cartEmpty()
// {
//     printf("确定清空购物车？按回车键确认");
//     if (getch() == 13)
//     {
//         itemNum = 0;
//         char s[32];
//         sprintf(s, "Data\\%s的购物车.txt", cU->name);
//         FILE *fp = fopen(s, "w");
//         fclose(fp);
//         puts("已清空购物车！");
//     }
// }

// 我的订单
void myOrder()
{
    system("cls");
    puts("您的订单信息：");
    if (!orderNum)
    {
        puts("空空如也~");
        system("pause");
        return;
    }
    orderList(); // 打印订单列表
    puts("请选择：");
    puts("\t1.修改订单\t2.取消订单\tEsc.返回菜单");
    switch (getch())
    {
    case '1':
        addressMod(); // 修改订单派送地址
        break;
    case '2':
        orderCancel(); // 取消未收货订单
        break;
    case 27:
        return;
    default:
        puts("\a请输入正确指令！");
        system("pause");
    }
    myOrder();
}

// 订单列表模块
void orderList()
{
    puts("============================================================================================");
    printf("%5s |%15s |%5s |%10s |%15s |%10s |%20s\n", "编号", "商品名称", "数量", "总价格", "下单时间", "状态", "收货地址");
    for (int i = 0; i < cU_oNum; i++)
    {
        printf("%5d |%15s |%5d |%10.2f |%15s |%10s |%20s\n", i, cU_o[i]->item.goods.name, cU_o[i]->item.number, cU_o[i]->item.cost, cU_o[i]->creatTime, cU_o[i]->status, cU_o[i]->address);
    }
    puts("============================================================================================");
}

// 修改订单收货地址
void addressMod()
{
    system("cls");
    orderList();
    puts("请输入要修改的订单编号：");
    int oId = 0;
    if (!scanf("%d", &oId) || oId < 0 || oId >= orderNum)
    {
        puts("\a没有该编号！");
        fflush(stdin);
        keepOrNot(addressMod);
    }
    puts("请输入新的收货地址：");
    char newAddress[20];
    while (!scanf("%19s", newAddress))
    {
        puts("输入为空！（Esc：不改了）");
        if (getch() == 27)
        {
            return;
        }
    }
    strcpy(cU_o[oId]->address, newAddress);
    FILE *fp = fopen("Data\\订单信息.txt", "r+");
    fseek(fp, oId * 106 + 71, 0); // 移动文件指针到订单的收货地址位置
    fprintf(fp, "%20s", newAddress);
    fclose(fp);
    system("pause");
}

// 取消未到货订单
void orderCancel()
{
    system("cls");
    orderList();
    puts("请输入要取消的订单编号：");
    int oId = 0;
    while (!scanf("%d", &oId) || oId < 0 || oId >= orderNum)
    {
        puts("\a没有该编号！请重新输入：");
        fflush(stdin);
    }
    puts("订单信息如下，按回车键（Enter）确认取消");
    printf("%15s |%5s |%10s |%15s |%10s |%20s\n", "商品名称", "数量", "总价格", "下单时间", "状态", "收货地址");
    printf("%15s |%5d |%10.2f |%15s |%10s |%20s\n", cU_o[oId]->item.goods.name, cU_o[oId]->item.number, cU_o[oId]->item.cost, cU_o[oId]->creatTime, cU_o[oId]->status, cU_o[oId]->address);
    if (getch() != 13)
    {
        puts("已放弃取消！");
        keepOrNot(goodsAdd);
        return;
    }
    strcpy(cU_o[oId]->status, "已取消"); // 修改订单状态，不删除订单
    FILE *fp = fopen("Data\\订单信息.txt", "r+");
    fseek(fp, oId * 106 + 60, 0);
    fprintf(fp, "%10s", cU_o[oId]->status);
    fclose(fp);
    for (int j = 0; j < goodsNum; j++)
    {
        if (!strcmp(g[j].name, cU_o[oId]->item.goods.name))
        {
            g[j].stock -= cU_o[oId]->item.number;
        }
    }
    saveInfo('g');
    system("pause");
}

// 用户个人信息
void myInfo()
{
    system("cls");
    puts("您的个人信息如下：");
    puts("==========================================================");
    printf("用户名：%s\n", cU->name);
    printf("联系号码：%s\n", cU->phone);
    printf("收货地址：%s\n", cU->address);
    puts("==========================================================");
    puts("可选择修改：");
    puts("\t1.联系号码\t2.收货地址\t3.账户密码\tEsc.返回菜单");
    char oldPwd[20]; // 用户原密码
    int changed = 0; // 信息是否有改动
    switch (getch())
    {
    case '1':
        puts("请输入新的联系号码：");
        while (!scanf("%14[0-9]", cU->phone) || strlen(cU->phone) != 11)
        {
            puts("\a手机号码应为11位数字，请重新输入：");
            fflush(stdin);
        }
        puts("修改成功！");
        changed = 1;
        break;
    case '2':
        puts("请输入新的收货地址：");
        while (!scanf("%19s", cU->address))
        {
            puts("输入为空！（Esc：不改了）");
            if (getch() == 27)
            {
                return;
            }
        }
        puts("修改成功！");
        changed = 1;
        break;
    case '3':
        puts("请输入原密码：");
        while (!scanf("%19s", oldPwd) || strcmp(cU->password, oldPwd)) // 循环直到密码正确
        {
            if (strlen(oldPwd)) // 若输入不为空
            {
                puts("\a密码错误！（Enter：继续输入）");
                if (getch() == 27) // 若按下Esc键
                {
                    myInfo();
                    return;
                }
                puts("请输入原密码：");
            }
        }
        puts("请输入新密码：");
        while (!scanf("%19s", cU->password))
        {
            puts("输入为空！（Esc：不改了）");
            if (getch() == 27)
            {
                return;
            }
        }
        puts("修改成功！");
        changed = 1;
        break;
    case 27:
        return;
    default:
        puts("\a请输入正确指令！");
    }
    if (changed)
    {
        FILE *fp = fopen("Data\\用户信息.txt", "w");
        // fseek(fp, (cU - uH) * 96, 0); // 将文件指针移动到当前用户信息行首
        // fprintf(fp, "%20s %20s %15s %20s %15s\n", cU->name, cU->password, cU->phone, cU->address, cU->regTime);
        for (struct User *p = uH; p->next; p = p->next)
        {
            fprintf(fp, "%20s %20s %15s %20s %15s\n", p->name, p->password, p->phone, p->address, p->regTime);
        }
        fclose(fp);
    }
    system("pause");
    myInfo();
}
