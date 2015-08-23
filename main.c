#include <stdlib.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    char flags[3];
    flags[0] = flags[1] = flags[2] = 0;

    int i;
    for(i = 0; i < argc; i++)
    {
        if(strcmp(argv[i],"-l") == 0)
            flags[0] = 1;

        if(strcmp(argv[i],"-a") == 0)
            flags[1] = 1;

        if(strcmp(argv[i],"-v") == 0)
            flags[2] = 1;
    }

	system("gcc -o scanner scanner.c");
    if(flags[0] == 1)
        system("./scanner -l");
    else
        system("./scanner");

	system("gcc -o parser parser.c");
    if(flags[1] == 1)
        system("./parser -a");
    else
        system("./parser");
	
	system("gcc -o vm vm.c");
    if(flags[2] == 1)
        system("./vm -v");
    else
        system("./vm");

	return 0;
}
