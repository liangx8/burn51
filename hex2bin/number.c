#include <stdio.h>
#include <stdlib.h>

int hexnumber(const char *str,int len)
{
    if(len >2){
        if(*str!='0'){
            return -1;
        }
        if(*(str+1) == 'x' || *(str+1) == 'X'){
            int val=0;
            for (int ix=2;ix<len;ix++){
                val = val << 4;
                int adv;
                int c=str[ix];
                if(c>='0' && c <='9'){
                    adv = (c -'0');
                } else {
                    if(c >= 'A' && c <= 'F'){
                        c += 0x20;
                    }
                    if(c >= 'a' && c<= 'f'){
                        adv = 10 + c - 'a';
                    } else {
                        return -1;
                    }
                }
                val += adv;
            }
            return val;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

int str2int(const char *str,int len)
{
    char strn[16];
    for(int ix=0;ix<len;ix++){
        strn[ix]=str[ix];
    }
    strn[len]='\0';
    int val=atoi(strn);
    if(val==0){
        return hexnumber(str,len);
    }
    return val;
}