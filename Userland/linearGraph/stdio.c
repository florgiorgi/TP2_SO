#include <stdarg.h>
#include "string.h"
#include "types.h"
#include "stdlib.h"
#include "stdio.h"

extern void int80(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);

void putchar(unsigned char c){
	int80(1, c, 1, 0, 0, 0);
}

void backspace(){
	char c = '\b';
	int80(1, 0 , (qword) &c, 1 ,0, 0);
}

void printf(const char * str,...){
	char num[12];
	int state = 0;
	int x;
	int index;
	int length = strlen(str);
	va_list arguments;
	va_start(arguments, str);

	for(x = 0; x < length; x++){
		if(state == 0){
			if(str[x] != '%')
				putchar(str[x]);
			else
				state = 1;
		} else {
			switch(str[x]){
				case 'd':
					intToString(va_arg(arguments, int), num);
					index = 0;
					while(num[index] != 0)
						putchar(num[index++]);
					state = 0;
					break;
				case 'c':
					putchar((char)(va_arg(arguments, int)));
					state = 0;
					break;
				case 's':
					printf(va_arg(arguments, char *));
					state = 0;
					break;
				default:	
					putchar('%');
					putchar(str[x]);
					state = 0;
					break;
			}
		}
	}

	va_end(arguments);
}

void clear_screen(){
	int80(2, 1, 0, 0, 0, 0);
}

void draw_pixel(int x, int y){
	int80(8, x, y, 0, 0, 0);
}

char getchar(){
	unsigned char c[2];
	int80(3, 0, (qword) c, 2, 0, 0);

	if(c[0] == 0 ){
		return EOF;
	}

	return c[0];
}

