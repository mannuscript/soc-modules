#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>  
//needed for IO things. Attention that this is different from kernel mode 
int lcd; 
#define SCULL_IOC_MAGIC  'k' 
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC,   1)
#define SCULL_WRITE _IOW(SCULL_IOC_MAGIC, 2, char*)
#define SCULL_READ _IOR(SCULL_IOC_MAGIC, 3, char*)
#define SCULL_READ_WRITE _IOWR(SCULL_IOC_MAGIC, 4, char*)

char *usr_msg[1000]; 

void test() 
{ 
  int k, i, sum; 
  char s[3]; 
 
  memset(s, '2', sizeof(s)); 
  printf("test begin!\n"); 
 
  k = write(lcd, s, sizeof(s)); 
  printf("written = %d\n", k); 
  k = ioctl(lcd, SCULL_HELLO); 
  printf("result = %d\n", k); 
  
	printf("Calling ioctl write function with string: %s\n", "this is a test string");
	ioctl(lcd, SCULL_WRITE, "this is a test string");

	printf("Calling ioctl read function\n\n");
	ioctl(lcd, SCULL_READ, usr_msg);
	printf("Output from: ioctl read usr_msg:: %s\n", usr_msg);

	strcpy(usr_msg, "This is to test read_write message");

	printf("Calling ioctl read_write function with argument: %s\n", usr_msg);
	ioctl(lcd, SCULL_READ_WRITE, usr_msg);
	printf("Output from ioctl read_write: %s\n", usr_msg);
   
}
int main(int argc, char **argv) 
{ 
  lcd = open("/dev/fourMbDriver", O_RDWR); 
  if (lcd == -1) { 
      perror("unable to open driver"); 
      exit(EXIT_FAILURE); 
  } 
 
 
test(); 
 
  close(lcd); 
  return 0; 
} 
