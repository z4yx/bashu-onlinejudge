#include<iostream>
#include <cstdio>
#include <cstring>
using namespace std;
long long f[1001][1001];
int l[1001],m[1001];
long long max(long long a,long long b,long long c)
{
  long long ans=a;
  if(b>ans) ans=b;
  if(c>ans) ans=c;
  return ans;     
}
int main()
{     
       int n,lo,hi,i,j,w;
       cin>>n>>lo>>hi;
       memset(f,0,sizeof(f));
       for(i=1;i<=n;i++){scanf("%d",&l[i]);l[i]+=l[i-1];}
       for(i=1;i<=n;i++){scanf("%d",&m[i]);m[i]+=m[i-1];}
       for(i=1;i<=n;i++)
          for(j=i;j<=n;j++)
          {
            w=0;
       if(l[j]-l[i-1]>=lo&&l[j]-l[i-1]<=hi) w=m[j]-m[i-1];
            f[i][j]=max(f[i-1][j],f[i][j-1],f[i-1][j-1]+w);
          }
      cout<<f[n][n];
}
