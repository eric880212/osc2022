#include "node.h"
#include "math.h"

void fm_init(){
    extern frame *fm_head[17];
    extern frame fm[0x10000];	

	fm[0].index = 0;
    fm[0].value = 16;
	fm[0].next = (frame *)0;
	fm[0].prev = &fm[0];

    for(int i=1;i<0x10000; i++){
		fm[i].index = i;
        fm[i].value = -5;
		fm[i].next = (frame *)0;
		fm[i].prev = (frame *)0;
    }

    for(int i=0; i<17; i++){
        fm_head[i] = (frame *)0;
	}
	fm_head[16] = &fm[0];
    
    return;
} 

void fm_add(int exp, int index){
	extern frame *fm_head[17];
    extern frame fm[0x10000];
	
	fm[index].value = exp;
	for(int i = index + 1; i < index + pow(2, exp); i++){
		fm[i].value = -5;
	}

	if(!fm_head[exp]){
		fm_head[exp] = &fm[index];
		fm_head[exp] -> next = (frame *)0;
		fm_head[exp] -> prev = &fm[index];
	}
	else{
		fm[index].prev = fm_head[exp] -> prev;
		fm_head[exp] -> prev -> next = &fm[index];
		fm_head[exp] -> prev =  &fm[index];
		fm[index].next = (frame *)0;
	}
	return;

}

void fm_delete(int exp, int index){  
	extern frame fm[0x10000];
    extern frame *fm_head[17];

	if(fm[index].next != (frame *)0 && fm_head[exp] != &fm[index]){
		//in the middle of linked list
		fm[index].next -> prev = fm[index].prev;
		fm[index].prev -> next = fm[index].next;
		
	}
	else if(fm_head[exp] == &fm[index]){
		//the only node of the linked list
		fm_head[exp] = fm[index].next;

		//the first node of the linked list
		if(fm[index].next)
			fm[index].next -> prev = fm[index].prev;
	}
	else{
		//in the end of the index
		fm[index].prev -> next = (frame *)0;
		fm_head[exp] -> prev = fm[index].prev;
	}
	fm[index].value = -5;
	fm[index].next = (frame *)0;
	fm[index].prev = (frame *)0;

	return;
}
void fs_init(){
    extern fs *fs_head[5];
    for(int i=0; i<5; i++){
        fs_head[i] = (fs*)0;
    }
    return;
}
void fs_add(int lk, fs *item){
    extern fs *fs_head[5];
    if(!fs_head[lk]){
        fs_head[lk] = item;
        fs_head[lk]->next = (fs *)0;
        fs_head[lk]->prev = item;
    }
    else{
        fs *temp = fs_head[lk];
        while(temp->next){
            temp = temp->next;
        }
        temp->next = item;
        item->prev = temp;
        item->next = (fs *)0;
    }
    return;
}
void rsv_add(int start, int end){
    extern rsv *rsv_head;
    extern rsv rsv_pool[];
    extern int rsv_pool_p;
    rsv_pool[rsv_pool_p].start_idx = start;
    rsv_pool[rsv_pool_p].end_idx = end;
    rsv_pool[rsv_pool_p].next = (rsv *)0;
    if(!rsv_head){    
        rsv_head = &rsv_pool[rsv_pool_p++];
        return;
    }
    else{
        rsv *temp = rsv_head;
        while(temp->next)
            temp = temp->next;
        temp->next = &rsv_pool[rsv_pool_p++];
    }
}
