struct bar
{
    int var[10];
};

int main()
{
    int a[10][20], i, j;
    struct bar b[10];
    for(i=0; i<10;i=i+1)
    {
        for(j=0; j<10;j=j+1)
        {
            b[i].var[j] = i;
        }
    }

    for(i=0; i<10;i=i+1)
    {
        for(j=0; j<10;j=j+1)
        {
            printf("%d ",b[i].var[j]);
        }
        printf("\n");
    }

    return 0;
}
