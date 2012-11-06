#include<iostream>
using namespace std;
int main()
 {
  int N,k,i,a[3*N],b[N+1],head,tail;
  cin>>N;
   head=1;tail=N+1;k=1;
      for(i=1;i<=N;i++) a[i]=i;
      while(1)
      {b[a[head]]=k;k++;
      if(k>N) break;
      head++;
      a[tail]=a[head];tail++;head++;
      a[tail]=a[head];tail++;head++;}
      for(i=1;i<=N;i++) cout<<b[i]<<" ";
}
