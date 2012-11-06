#include<iostream>
using namespace std;
long long a[200001],b[9999999];
void fac(long q,long p)
{long i=q,j=p,mid=a[(p+q)/2],t;
while (i<=j)
{while(a[i]<mid)i++;
while(a[j]>mid)j--;
if (a[i]>=a[j] )
{t=a[i];
a[i]=a[j];
a[j]=t;
i++;
j--;}
if (i<p)fac(i,p);
if (q<j)fac(q,j);
      }
     }

int main()
{long n,i;
cin>>n;
for (i=1;i<=n;i++){cin>>a[i];b[a[i]]++;}
fac(1,n);
for (i=1;i<=n;i++){cout<<a[i]<<" "<<b[a[i]]<<endl;i=i+b[a[i]];}
 
return 0;
    }
