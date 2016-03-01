//#include <ctime>
#include "smart_allocator.hpp"
#include <cstddef>

template <typename T> void print(IPointers<T>& p){
    for(int i = 0; i < p.size(); ++i)
        std::cout<<p[i];
    std::cout<<std::endl;
}
int main(){
    static int Arr[200];
    //It works even if initial memory type is different from desired.
    //So, it's not need to manually cast anything.
    SmartAllocator<char, int> At(Arr, 200);
    IPointers<char>& p = At.alloc(4);

    for(int i = 0 ; i < 4; ++i)
        p[i] = 'A' + i;
    print(p);//the first output
    At.realloc(p, 20);
    for(int i = 4 ; i < 20; ++i)
        p[i] = 'A' + i;

    print(p); //the second output

    At.free(p);
}
