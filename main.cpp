#include "MemoryPool.h"
using namespace std;

class MemoryPool_int{
public:
    static MemoryPool& GetPool(){
        //单例模式
        static MemoryPool pool(sizeof(int), 8, 1024);
        return pool;
    }

    void* operator new(size_t size){
        if(size != sizeof(int)){
            return std::malloc(size);
        }
        return GetPool().Allocate();
    }

    void operator delete(void* ptr){
        if(!ptr) return;
        GetPool().Deallocate(ptr);
    }

    int id_;
};

int main(){
    MemoryPool_int* p1 = new MemoryPool_int();
    MemoryPool_int* p2 = new MemoryPool_int();
    MemoryPool_int* p3 = new MemoryPool_int();
    p1->id_ = 1;
    p2->id_ = 2;
    p3->id_ = 3;
    cout << "id: " << p1->id_ << " address: " << p1 << endl;
    cout << "id: " << p2->id_ << " address: " << p2 << endl;
    cout << "id: " << p3->id_ << " address: " << p3 << endl;
    delete p1;
    delete p2;
    delete p3;
    std::vector<MemoryPool_int*> nums;
    nums.reserve(4000);
    for(int i = 0; i < 4000; i++){
        nums.push_back(new MemoryPool_int());
        nums.back()->id_ = i;
    }
    for(MemoryPool_int* x : nums) delete x;
    return 0;
}