void help(void);
void hello(void);
void reboot(void);
void bootloader(void);

#define NEW_ADDR ((volatile unsigned char*)(0x60000))
#define OLD_ADDR ((volatile unsigned char*)(0x80000))
