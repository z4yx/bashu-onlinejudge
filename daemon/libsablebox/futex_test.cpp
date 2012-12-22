#include<cstdio>
#include<cstring>
#define clear(x) memset((x),0,sizeof(x))
#define N 64
struct _e{
	int to,next;
}e[512],re[512];
int n,top[N],head[N],rhead[N],belong[N],oppo[N],SCC,cnt,tot;
bool flag[N],slt[N];
void add(int a,int b){
	e[++tot].to=b;
	e[tot].next=head[a];
	head[a]=tot;
	
	re[tot].to=a;
	re[tot].next=rhead[b];
	rhead[b]=tot;
}
void dfs(int v){
	flag[v]=1;
	for(int i=head[v];i;i=e[i].next)
		if(!flag[e[i].to])
			dfs(e[i].to);
	top[++cnt]=v;
}
void shrink(int v){
	belong[v]=SCC;
	for(int i=rhead[v];i;i=re[i].next)
		if(!belong[re[i].to])
			shrink(re[i].to);
}
void kosaraju(){
	clear(flag);
	clear(belong);
	SCC=cnt=0;
	for(int i=1;i<=n;i++)
		if(!flag[i])
			dfs(i);
	for(int i=cnt;i;i--)
		if(!belong[top[i]]){
			SCC++;
			shrink(top[i]);
		}
}
int other(int i){
	return (i&1)?i+1:i-1;
}
int main(){
	int m;
	while(scanf("%d%d",&n,&m),n){
		tot=0;clear(head);clear(rhead);clear(slt);
		add(2,1);
		while(m--){
			int x,y;char sx,sy;
			scanf("%d%c%d%c",&x,&sx,&y,&sy);
			x++;y++;
			add(x*2-int(sx=='h'),y*2-int(sy=='w'));
			add(y*2-int(sy=='h'),x*2-int(sx=='w'));
		}
		n=n*2;
		kosaraju();
		for(int i=1;i<=n;i+=2)
			if(belong[i]==belong[i+1]){
				puts("bad luck");goto over;
			}
		for(int i=1;i<=n;i++)
			oppo[belong[i]]=belong[other(i)];
		for(int i=SCC;i;i--)
			if(!slt[oppo[i]])
				slt[i]=1;
		for(int i=3;i<=n;i+=2){
			printf("%d%c ",i>>1,slt[belong[i]]?'w':'h');
		}
		puts("");
over:;
	}
}