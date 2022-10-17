typedef struct Frame{
    int index;
	int value;
    struct Frame *next;
    struct Frame *prev;
}frame;

typedef struct free_size{
    void *addr;
    struct free_size *next;
    struct free_size *prev;
}fs;
typedef struct reserve_address{
    int start_idx;
    int end_idx;
    struct reserve_address *next;
}rsv;


void fm_init();
void fm_add(int exp, int index);
void fm_delete(int exp, int index);
void fs_init();
void fs_add(int lk, fs *item);
void rsv_add(int start, int end);
