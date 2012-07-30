#include <stdio.h>
#include <stdlib.h>
#include "merror.h"

void errorhand(char* mess)
{
	fprintf(stderr, mess);
	exit(1);
}
