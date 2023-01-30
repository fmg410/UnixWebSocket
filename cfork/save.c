#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
    if(argc > 0)
    {
        FILE* f = fopen(argv[0], "w");
        for(int i = 1; i < argc; i++)
            fprintf(f, "%s\n", argv[i]);
        fclose(f);
    }

    return 0;
}

