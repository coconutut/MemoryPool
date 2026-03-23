#pragma once
#include <vector>
#include <mutex>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <algorithm>

class MemoryPool{
public:
    explicit MemoryPool(std::size_t block_size, std::size_t align = 8, std::size_t prealloc = 1024);
    
    ~MemoryPool();

    void* Allocate();

    void Deallocate(void* ptr);

    int GetPools();

private:
    struct FreeBlock{        //空闲块结构（嵌入式链表）
        FreeBlock* next;
    };                       
    
    std::size_t block_size_; //对齐后块大小
    
    std::size_t align_;      //对齐规则
    
    FreeBlock* free_list_;   //空闲块链表头
    
    std::vector<void*> pools_; //记录所有预分配的大内存块（用于析构）
    
    std::mutex mutex_;       //多线程安全锁

private:
    void Preallocate(std::size_t num_blocks); //预分配num_blocks个块

    //显式禁止拷贝构造、赋值构造
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
};