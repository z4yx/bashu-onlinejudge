#include <cstdlib>
int main()

{
	for(;;);
	int *p = new int[5000*5000];
	p[2500] = 1;
	delete[] p;
	return 0;

}
