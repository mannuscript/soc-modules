#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#define DRIVER_PATH "/dev/fourMbDriver"
#define MAX_LENGTH 4*1000000

int main(void)
{

	int char_driver = open("/dev/fourMbDriver", O_RDWR, 0644);
	char *FILE_PATH;
	int input = 0;
	
	printf("\nWhich file to use for test: Big(1) or Small(2)\n");
	scanf("%d", &input);
	if(input == 1)
		FILE_PATH = "big.txt";
	else
		FILE_PATH = "small.txt";
	

	FILE *fileToRead = fopen(FILE_PATH, "r");
	char *str;
	char ch;
	int sizeOfFile;

	if( fileToRead == NULL)
	{
		fclose(fileToRead);
		printf("Error opening the %s or %s file", FILE_PATH, DRIVER_PATH);
		return -1;
	}

	//Get size of file	
	fseek(fileToRead, 0, SEEK_END);
	sizeOfFile = ftell(fileToRead);
	fseek(fileToRead, 0, SEEK_SET);
	printf("Size of file going to be written on driver: %d\n\n", sizeOfFile);
	//Assign memory to the string
	str = malloc(sizeof(char*)*sizeOfFile);
	
	//Read data form file
	for(int i=0; i < MAX_LENGTH && (ch = fgetc(fileToRead)) != EOF ; i++)
	{
		*(str + i) = ch;
	} 			
	
	//Copy file to driver
	//Attempt to write whole string to driver!
	printf("Number of byte charcter written to driver: %d\n\n",	write(char_driver, str, sizeOfFile));
	close(char_driver);
  	fclose(fileToRead);
	return 0;
}
