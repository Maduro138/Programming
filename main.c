#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  void testeone(int vetor[], int n){
   for(int i = n-1; i >=0 ; i--){
     printf("\n %d", vetor[i]);
     
   }
     
  }

int main(){
   int n;
  printf("Tell me the size of the vector: ");
   scanf("%d", &n);
  int v[n];
  for(int i = 0; i < n; i++){
    printf("Tell me the value of the vector int the %d position: ", i+1);
   scanf("%d", &v[i]);
  }
  printf("The vector in reverse order is: ");
  testeone(v, n);
  

   return 0;
}