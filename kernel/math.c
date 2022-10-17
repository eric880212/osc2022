int  pow(int base, int  exp){
    int num = 1;
    while(exp-- !=0){
        num *= base;
    }
    return num;
}

int min(int x, int y){
    if(x < y)
        return x;
    return y;
}

int log(int base, int nat){
    int exp = 0;
    while(nat != 1){
        nat /= base;
        exp++;
    }
    return exp;
}
