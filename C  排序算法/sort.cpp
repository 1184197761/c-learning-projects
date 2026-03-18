#include "sort.h"
// 冒泡排序
void bubbleSort(int arr[], int len) {
	bool flag = false; //标记（检测是否排序完成）
	for (int i = 0; i < len - 1; i++) {  //遍历次数（len-1）
		for (int j = len-1; j > i; j--) {  //交换次数，每一趟交换次数减一
			flag = false;  //先将flag置为false
			if (arr[j] < arr[j - 1]) {  //判断大小决定是否交换
				int temp = arr[j];
				arr[j] = arr[j - 1];
				arr[j - 1] = temp;
				flag = true;  //若发生交换，flag置true
			}
		}
		if (flag == false) break;  //若为flase，说明本趟没有交换过，直接返回
	}
}

// 选择排序
void selctionSort(int arr[], int len) {
	for (int i = 0; i < len - 1; i++) {  //遍历次数（len-1）
		int min = i;  //储存最小值，先储存第一个数
		for (int j = i + 1; j < len; j++) {  //从第一个数后找最小值
			if (arr[j] < arr[min])  //若找到更小的值则赋值给min
				min = j;
		}
		if (min != i) {  //如果有找到更小的值则交换位置
			int temp = arr[i];
			arr[i] = arr[min];
			arr[min] = temp;
		}
	}
}

/*// 插入排序
void insertionSort(int arr[], int len) {
	for (int i = 1; i < len; i++) { 
		int j, k;
		for (j = i - 1; j >= 0; j--)
			if (arr[j] < arr[i]) break;
		if (j != i - 1) {
			int temp = arr[i];
			for (k = i - 1; k > j; k--) 
				arr[k + 1] = arr[k];
			arr[k + 1] = temp;
		}
	}
}*/

// 插入排序（YY版）
void insertionSort(int arr[], int len) {
	for (int i = 1; i < len; i++) {  //从第二个元素开始往前比较
		int temp = arr[i];  //存储当前元素的值
		for (int j = i - 1; j >= 0; j--) { //
			if (arr[j] > temp) {  //若遇到比temp大的数
				arr[j + 1] = arr[j];  //将其往后挪
				if (j == 0)  //若该数为第一个元素
					arr[j] = temp;  //将temp插入
			}
			else {  //若遇到比temp小的数
				arr[j + 1] = temp;  //将temp插在其后并结束比较
				break;
			}
		}
	}
}


// 归并排序
void mergeSort(int arr[], int first, int last, int temp[]) {
	if (first < last) {  //若该数组不为空
		int middle = (first + last) / 2;  //中间数
		mergeSort(arr, first, middle, temp);   //左边排序
		mergeSort(arr, middle + 1, last, temp);  //右边排序
		mergeArray(arr, first, middle, last, temp);  //合并左右序列
	}
}
void mergeArray(int arr[], int first, int middle, int end, int temp[]) {
	int i = first, j = middle + 1, k = 0;
	int m = middle, n = end;
	for (; i <= m && j <= n;	k++) {  //遍历直到其中一边到底
		if (arr[i] <= arr[j])  //如果左边第一个数小于右边的
			temp[k] = arr[i++];  //将左边第一个数提出并指向第二个数
		else  //同理
			temp[k] = arr[j++];
	}
	while (i <= m) {  //当左边还没到底
		temp[k++] = arr[i++];  //将剩下的元素依次提出
	}
	while (j <= n) {  //同理
		temp[k++] = arr[j++];
	}
	for (int i = 0; i < k; i++) {  //将临时数组赋值给原（部分）数组
		arr[first + i] = temp[i];
	}
}

// 快速排序
void quickSort(int arr[], int lift, int right) {
	if (lift >= right) return;
	int i = lift, j = right, key = arr[lift];  //选择第一个数为key（挖坑）
	while (i < j) {
		while (i < j && arr[j] >= key)  //从右向左找第一个小于key的值
			j--;  //左移位置
		if (i < j) 
			arr[i++] = arr[j];  //“填坑”（i++减少不必要的比较）
		while (i < j && arr[i] < key)  //从左向右找第一个大于key的值
			i++;  //右移位置
		if (i < j) 
			arr[j--] = arr[i];  //“填坑”（拆东墙补西墙）
	}
	// 左边比key大的数都已填到右边，右边同样（中间的坑留给key）
	arr[i] = key;  //“填坑”
	quickSort(arr, lift, i - 1);  //key左边数组递归调用
	quickSort(arr, i + 1, right);  //key右边数组递归调用
}