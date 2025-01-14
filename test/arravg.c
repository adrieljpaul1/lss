#include <stdio.h>
int arr[4];
float avg = 0;
int main(){
    for(int i = 0; i<2;i++){
        arr[i] = 4095;
    }
    for(int i = 0; i<1;i++){
        arr[i] = 4033;
    }
    for(int i = 0; i<1;i++){
        arr[i] = 4043;
    }
    for(int i = 0;i<4;i++){
        printf("%d\n",arr[i]);
    }
    for(int i = 0;i<4;i++){
        avg += arr[i];
    }
    printf("%d\n",avg);
    printf("%d\n",(int)avg);
    avg = (int)(avg/4);
    printf("%d\n",avg);
    return 0;
}
