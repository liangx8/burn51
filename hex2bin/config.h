#pragma once
struct PAD_DATA{
    int offset,count;
};
struct CONFIG{
    const char *src,*dst;
    int pad_count;
    struct PAD_DATA *pad;
};