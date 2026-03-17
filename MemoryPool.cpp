#include "MemoryPool.h"

inline size_t AlignUp(size_t size, size_t align){
    return (size + align - 1) & ~(align - 1);
}

MemoryPool::MemoryPool(std::size_t block_size, std::size_t align, std::size_t prealloc){
    free_list_ = nullptr;
    align_ = align;
    block_size_ = AlignUp(block_size, align_);
    if(prealloc > 0){
        Preallocate(prealloc);
    }
}

MemoryPool::~MemoryPool(){
    for(void* pool : pools_){
        std::free(pool);
    }
}

void* MemoryPool::Allocate(){
    std::lock_guard<std::mutex> lock(mutex_);
    //动态扩容
    if(!free_list_){
        size_t expandNum = pools_.empty() ? 1024 : pools_.size() * 1024;
        Preallocate(expandNum);
    }
    FreeBlock* block = free_list_;
    free_list_ = free_list_->next;
    return block;
}

void MemoryPool::Deallocate(void* ptr){
    if(!ptr) return;
    std::lock_guard<std::mutex> lock(mutex_);
    FreeBlock* block = static_cast<FreeBlock*>(ptr);
    block->next = free_list_;
    free_list_ = block;
}

void MemoryPool::Preallocate(size_t num_blocks){
    size_t total_size = num_blocks * block_size_;
    void* pool = std::malloc(total_size);
    if(!pool) 
        throw std::bad_alloc();
    
    pools_.push_back(pool);

    FreeBlock* start = static_cast<FreeBlock*>(pool);
    FreeBlock* current = start;
    for(size_t i = 0; i < num_blocks-1; i++){
        current->next = reinterpret_cast<FreeBlock*>(reinterpret_cast<char*>(current) + block_size_);
        current = current->next;
    }
    
    if(free_list_){
        current->next = free_list_;
    }else{
        current->next = nullptr;
    }

    free_list_ = start;
    std::cout << "Preallocated " << num_blocks << " blocks(size: " << 
        block_size_ << "B), total pools: " << pools_.size() << std::endl;

}


