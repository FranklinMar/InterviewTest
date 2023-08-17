#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*void *get(unsigned char strin[])
{
	void *p = 0;
	memmove(strin, strin+2, strlen(strin));
	unsigned long i = (long) strin;
	return p + i - 20;
}*/

int main(void){
	/*char address[] = "0x0000000000012345";
	void *a = NULL;
	printf("%p\n", a);
	printf("%p\n", a+7);
	char * str;
	str = malloc(sizeof(char) * strlen(address));
	strcpy(str, address);
	printf("%p\n", str);*/
	void *pointer = (void *) 0x12345;
	printf("Pointer address: %p", pointer);
	char first_byte = *((char*) pointer);
	//char second_byte = pointer[1];
	return 0;
}

