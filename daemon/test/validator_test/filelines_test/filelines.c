#include <stdio.h>
#include <stdlib.h>

long filelines(FILE* stream)
{
    fseek(stream, 0, SEEK_SET);
    int c, tmp = 0, sum = 0, non_empty = 0;
    while(c = getc(stream)){
        if(c == EOF)
            break;
        if(c == '\n'){
            tmp++;
        }else{
            sum += tmp;
            tmp = 0;
            non_empty = 1;
        }
    }
    return sum + non_empty;
}

#define should(left, op, right) \
    if(!(left op right)){ \
        puts("Failed!!!" #left #op #right);\
        printf("%s = %d, %s = %d\n\n", #left, left, #right, right); \
    }else{\
        puts("Passed!!!" #left #op #right);\
    }

void test_blank()
{
    {
        system("echo \"\n\n\" > blank.txt");
        FILE* fp = fopen("blank.txt", "r");
        should(filelines(fp), ==, 0);
    }

    {
        system("echo \"\" > blank.txt");
        FILE* fp = fopen("blank.txt", "r");
        should(filelines(fp), ==, 0);
    }
}

void test_ordinary()
{
    {
        system("echo \"123\" > blank.txt");
        FILE* fp = fopen("blank.txt", "r");
        should(filelines(fp), ==, 1);
    }
    {
        system("echo \"123\n123\n\n\n\" > blank.txt");
        FILE* fp = fopen("blank.txt", "r");
        should(filelines(fp), ==, 2);
    }
    {
        system("echo \"123\n\n123\" > blank.txt");
        FILE* fp = fopen("blank.txt", "r");
        should(filelines(fp), ==, 3);
    }
    {
        system("echo \"haha\" > blank.txt");
        FILE* fp = fopen("blank.txt", "r");
        should(filelines(fp), ==, 1);
    }
}

void test_invalid()
{
    FILE* fp = fopen("blank.txt", "w");
    fprintf(fp, "123");
    fclose(fp);
    {
        FILE* fp = fopen("blank.txt", "r");
        should(filelines(fp), ==, 1);
    }
}

int main()
{
    test_blank();
    test_ordinary();
    test_invalid();
    return 0;
}
