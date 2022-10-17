struct Timeout{
    char msg[20];
    void (*callback)(char *);
    long cval;
}event[20];
void add_timer(char *data, void (*func)(char*),int);

