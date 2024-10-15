/**
 * 简单的内存管理实现，只对内存申请以及最后集中释放
 */

#include <windows.h>
#define BLOCK_SIZE 50
struct mem_link
{
    struct mem_link *next;
    int cnt;
    void *mem[BLOCK_SIZE];
} *m_head;
int auto_init(void){
    m_head=LocalAlloc(LPTR,sizeof(struct mem_link));
    if (m_head == NULL){
        return -1;
    }
    return 0;
}
/// @brief 所有由此程序申请的堆内存，会在最后由 all_free()释放
/// @param size 
/// @return 返回申请的堆内存地址，返回NULL出错
void *auto_alloc(SIZE_T size)
{
    struct mem_link *head=m_head;
    void *mem;
    while(head->cnt == BLOCK_SIZE){
        head=head->next;
    }
    mem=LocalAlloc(LPTR,size);
    if (mem == NULL){
        return NULL;
    }
    head->mem[head->cnt]=mem;
    head->cnt++;
    if (head->cnt== BLOCK_SIZE){
        struct mem_link *ml;
        ml=LocalAlloc(LPTR,sizeof(struct mem_link));
        if (ml == NULL){
            // 内存表的申请失败，前面的内存申请要撤销
            LocalFree(mem);
            head->cnt=BLOCK_SIZE-1;
            return NULL;
        }
        head->next=ml;
        // 用LPTR参数会把内存全部清零
        //ml->cnt=0;
        //ml->next=0;
    }

    return mem;
}
/// @brief 把内存块管理起来
/// @param ptr 要管理的内存块
/// @return 0 成功 -1 失败
int mem_set(void *ptr){
    struct mem_link *head=m_head;
    while(head->cnt == BLOCK_SIZE){
        head=head->next;
    }
    head->mem[head->cnt]=ptr;
    head->cnt++;
    if (head->cnt== BLOCK_SIZE){
        struct mem_link *ml;
        ml=LocalAlloc(LPTR,sizeof(struct mem_link));
        if (ml == NULL){
            // 内存表的申请失败，前面的内存申请要撤销
            head->cnt=BLOCK_SIZE-1;
            return -1;
        }
        head->next=ml;
        // 用LPTR参数会把内存全部清零
        //ml->cnt=0;
        //ml->next=0;
    }
    return 0;

}
/// @brief 把全部auto_alloc()申请的内存全部释放
void all_free(void)
{
    struct mem_link *head=m_head;
    while(head){
        void *old=head;
        for(int idx=0;idx<head->cnt;idx++){
            LocalFree(head->mem[idx]);
        }
        head=head->next;
        LocalFree(old);
    } 
}