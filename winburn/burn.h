#ifndef _BURN_H_
#define JTAG_PROTOCOL           0x00
#define C2_PROTOCOL             0x01
#define IRAM 					1
#define XRAM 					2
#define FLASH 					3
#define INTACT					4
#define BURNIT					5
#define GUI						6
struct data {
    char* str;
    const wchar_t *wstr;
    unsigned int size;
	int action;
    union {
        unsigned int value;
        struct {
            int error           :1;
            int erase           :1;
            int disableDialog   :1;
            int lock            :1;
            int goRun           :1;
        } bits;
    } flag;
};

#define DBGMSG(fmt,...) wprintf(L"%ls(%d)" fmt,TEXT(__FILE__),__LINE__,__VA_ARGS__)
#endif
