#include <stdio.h>
int main(){
	int arr[] = {4555,7688,88368,88465};
	int a;
	for(int i = 0; i <4;i++){
		a += *(arr + i);
	}
	printf("%d\n",a);
}
