#include <hellfire.h>

void task(void) {
	printf("hello world!\n");
}

void app_main(void){
	//putchar('!');
	//putchar('\n');
	hf_spawn(task, 0, 0, 0, "task", 2048);
}

