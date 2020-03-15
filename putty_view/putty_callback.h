#ifndef PUTTY_CALLBACK_H
#define PUTTY_CALLBACK_H

struct init_config {
	int width;
	int height;
	bool maximized;
	bool alwaysontop;
};

init_config* process_init();
void process_fini();


#endif /* PUTTY_CALLBACK_H */
