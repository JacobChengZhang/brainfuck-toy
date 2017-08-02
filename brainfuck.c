/*
 * A concise brainfuck interpreter implemented by "redraiment"(Not me) (see http://blog.csdn.net/redraiment/article/details/7483062).
 * Modified by JacobChengZhang for further utilization
 * Tokens: "><+-.,[]"
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CODE_SEGMENT_SIZE 32768
#define STACK_SEGMENT_SIZE 1024
#define DATA_SEGMENT_SIZE 32768

#define DEBUG

enum {
	BF_OK = 0,
	BF_FAIL_OPEN_FILE, 
	BF_CODE_SEGMENT_FULL,
	BF_STACK_SEGMENT_FULL
};

volatile int sign = BF_OK;

typedef void (*Callback)(void);

struct {
	char cs[CODE_SEGMENT_SIZE];   /* Code Segment */
	long ip;                      /* Instruction Pointer */

	char ss[STACK_SEGMENT_SIZE];  /* Stack Segment */
	long sp;                      /* Stack Pointer */

	char ds[DATA_SEGMENT_SIZE];   /* Data Segment */
	long bp;                      /* Base Pointer */

	Callback fn[94];              /* Since the biggest ascii among tokens is 93(']') */
} vm;

void vm_forward(){
	vm.bp = (vm.bp + 1) % DATA_SEGMENT_SIZE;
}

void vm_backward() {
	vm.bp = (vm.bp + DATA_SEGMENT_SIZE - 1) % DATA_SEGMENT_SIZE;
}

void vm_increment() {
	vm.ds[vm.bp]++;
}

void vm_decrement() {
	vm.ds[vm.bp]--;
}

#ifdef DEBUG
	void vm_output() {
		printf("[%02u]", (unsigned char)vm.ds[vm.bp]);
	}
#else
	void vm_output() {
		putchar(vm.ds[vm.bp]);
	}
#endif

void vm_input() {
	printf("[Input]:");
	vm.ds[vm.bp] = getchar();
}

void vm_while_entry() {
	if (vm.ds[vm.bp]) {
    	vm.ss[vm.sp] = vm.ip - 1;
		vm.sp++;
		if (vm.sp >> 15 == 1){
			sign = BF_STACK_SEGMENT_FULL;
			printf("Stack segment full! Part of the code may be discarded.\n");
			return;
		}
	}
	else {
		int c = 1;
	    for (vm.ip++; vm.cs[vm.ip] && c; vm.ip++) {
	    	if (vm.cs[vm.ip] == '[') {
	    		c++;
	      	}
			else if (vm.cs[vm.ip] == ']') {
	        	c--;
	      	}
	    }
  	}
}

void vm_while_exit() {
	if (vm.ds[vm.bp]) {
    	vm.sp--;
    	vm.ip = vm.ss[vm.sp];
  	}
}

void setup() {
	int ch;
	int i;

	memset(&vm, 0, sizeof(vm));
	vm.fn['>'] = vm_forward;
	vm.fn['<'] = vm_backward;
	vm.fn['+'] = vm_increment;
	vm.fn['-'] = vm_decrement;
	vm.fn[','] = vm_input;
	vm.fn['.'] = vm_output;
	vm.fn['['] = vm_while_entry;
	vm.fn[']'] = vm_while_exit;
	
	for (i = 0; (ch = getchar()) != EOF; ) {
		if (ch != '/') {
		}
		else {
			ch = getchar();
			if (ch == '/'){
				do {
					ch = getchar();
				} while (ch != '\n' && ch != EOF);
				if (ch != EOF) {
					ch = getchar();
				}
			}
		}
		
		if (vm.fn[ch] != NULL){
			vm.cs[i] = ch;
			i++;
		}
		
		if ((i + 1) >> 15 == 1){
			sign = BF_CODE_SEGMENT_FULL;
			printf("Code segment full! Part of the code may be discarded.\n");
			return;
		}
	}
	freopen("CON","r",stdin); 
}

void run() {
	while (vm.cs[vm.ip]) {
    	vm.fn[vm.cs[vm.ip]]();
    	vm.ip++;
    	if (sign != BF_OK){
    		break;
		}
	}
}

int main(int argc, char* argv[]) {
	if (argc > 1) {
		FILE * fp;
		fp = freopen(argv[1], "r", stdin);
		if (fp == NULL){
			printf("Error opening \"%s\".\n", argv[1]);
			sign = BF_FAIL_OPEN_FILE;
			return sign;
		}
	}
	else {
		printf("___________ brain-fuck ___________ (press 'Ctrl-Z' after input finish)\n");
	}

	setup();
	run();
	
	return sign;
}
