#ifndef ALLOCATOR_CPP_INCLUDED
#define ALLOCATOR_CPP_INCLUDED
/** \file
 *  \brief
 Copyright 2016 Kashtanova Anna Viktorovna

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include"allocator.hpp"
 /*!
    Initializes Smart Allocator with a linked list
    \param[in] block - available memory
    \param[in] bytes - memory capacity

    */

template <class PointerType, class MemoryType>  PoolAllocator<PointerType, MemoryType>::PoolAllocator(MemoryType *block, size_t elements, bool heap)
                            :memory(reinterpret_cast<char*>(block))
                            ,size(elements*sizeof(MemoryType))
                            ,segments(size/sizeof(Pool)
                                      - sizeof(ExtendedPointer<PointerType>)/sizeof(Pool))// количество кусков по 8 байт
                            ,isHeap(heap)
                            ,pointerCounter(1)
                        {
            Pool * PoolSecond = nullptr;
            Pool PoolObject;
            for(size_t i = 0 ; i < segments ; ++i)
            {
                PoolHead    = reinterpret_cast<Pool*>(memory) + i + 1;
                PoolSecond  = reinterpret_cast<Pool*>(memory) + i;
                memmove(PoolSecond, &PoolObject, sizeof(PoolObject));
                PoolSecond->next = PoolHead;
            }
            PoolHead = (Pool*)memory;
            ExtendedPointer<PointerType> SharedObject;
            memmove(PoolSecond->next, &SharedObject, sizeof(SharedObject));
            queueHead = reinterpret_cast< ExtendedPointer<PointerType>* >(PoolSecond->next);
            PoolSecond->next = nullptr;
        }
template <class PointerType, class MemoryType>  PoolAllocator<PointerType, MemoryType>::~PoolAllocator(){
       if(heap())
        delete [] memory;
}

template<class PointerType, class MemoryType> void  PoolAllocator<PointerType, MemoryType>::set(PointerType*& oldPtr
                                                                                    ,const PointerType*& newPtr)const
{
    oldPtr = const_cast<PointerType*>(newPtr);
}
template<class PointerType, class MemoryType> void  PoolAllocator<PointerType, MemoryType>::setSize(size_t& oldSize
                                                                                                    , const size_t& newSize)const
{
    oldSize = newSize;
}

template<class PointerType, class MemoryType> bool  PoolAllocator<PointerType, MemoryType>::heap()const
{
    return (isHeap) ? true : false;
}


template<class PointerType, class MemoryType>  IPointers<PointerType>&  PoolAllocator<PointerType, MemoryType>::alloc(size_t _size) {

    size_t realSize = (_size%sizeof(Pool)) ?  _size/sizeof(Pool) + 1 : _size/sizeof(Pool) ;
    IPointers<PointerType>* top = queueHead;
    IPointers<PointerType>* vacant = findAddr(queueHead, reinterpret_cast<PointerType*>(NULL), pointerCounter);

        if(vacant){
            top = vacant;
            }else{
                createExtendedPointer();
                top = queueHead;
            }

        if( segments > realSize && _size != 0  ){

            Pool *ptrFirst  = PoolHead;
            Pool *ptrSecond = PoolHead;

            int counter = 0, genCounter = 0;
            bool segJump = false;

            while(counter < realSize && ptrFirst->next){
                if(ptrFirst->next - ptrFirst == 1)
                    ++counter;
                else
                    counter = 0;
                    ptrFirst = ptrFirst->next;
                    ++genCounter;
                }
            if(counter != genCounter)
                    segJump = 1;

            if(counter != realSize){
                defragmentation();
                //reset
                segJump = 0;
                ptrSecond = PoolHead;
                ptrFirst = ptrSecond + realSize;
                }

            if(segJump){
                for(int i = 0; i <= genCounter - counter - 1; ++i)
                    ptrSecond = ptrSecond->next;//run until the block before the first block
                    top->accept(this, reinterpret_cast<PointerType *>(ptrSecond->next), _size/sizeof(PointerType));
                    ptrSecond->next = ptrFirst;//copy the one's after last block address to the "next" field.
            }else{
                    top->accept(this, reinterpret_cast<PointerType *>(ptrSecond), _size/sizeof(PointerType));
                    PoolHead = ptrFirst;
                    }
            }
             return  *top;

}

template<class PointerType, class MemoryType> void PoolAllocator<PointerType, MemoryType>::createExtendedPointer(){
    if(segments >= sizeof(ExtendedPointer<PointerType>)/sizeof(Pool)){
        ExtendedPointer<PointerType> tmp;
        --queueHead;

        memmove(queueHead, &tmp, sizeof(tmp));

        Pool* end = nullptr;
        ++pointerCounter;
        uint8_t reminder = (sizeof(tmp)*pointerCounter)%sizeof(Pool);

        if(!reminder)
            end = reinterpret_cast<Pool *>(queueHead) - 1;
        else
            end = reinterpret_cast<Pool *>(reinterpret_cast<char *>(queueHead) - reminder + sizeof(Pool));
        end->next = nullptr;

        segments -= sizeof(tmp)/sizeof(Pool);
   }else
        throw std::bad_alloc();
}

 template<class PointerType, class MemoryType> inline size_t PoolAllocator<PointerType, MemoryType>::bytes(IPointers<PointerType>& pointer){
            return pointer.size()*sizeof(PointerType);
        }

 template<class PointerType, class MemoryType>
    void PoolAllocator<PointerType, MemoryType>::realloc(IPointers<PointerType>& pointer, size_t _size) {
        if(pointer == nullptr){
            pointer = alloc(_size);
            return;
        }
        else
            if(_size == 0 && pointer != nullptr){
                free(pointer);
                return;
            }
            IPointers<PointerType>& ptr = alloc(_size);
            for(size_t i = 0; i < _size/sizeof(PointerType) && i < pointer.size(); ++i)
                ptr[i] = pointer[i];
            free(pointer);
            pointer = ptr;

        }

template<class PointerType, class MemoryType> void PoolAllocator<PointerType, MemoryType>::free(IPointers<PointerType>& pointer) {
        if(pointer != nullptr){
            Pool* ptr = static_cast<Pool*>(pointer), *ptr_end = nullptr, *start = PoolHead;
            Pool PoolObject;
            size_t blockSize = (bytes(pointer)%sizeof(Pool)) ?
            bytes(pointer)/sizeof(Pool) + 1 : bytes(pointer)/sizeof(Pool);
            for(size_t i = 0 ; i < blockSize; ++i)
            {
                memmove(ptr, &PoolObject, sizeof(PoolObject));
                ptr_end = ptr++;
                ptr_end->next = ptr;
            }
            if(PoolHead > static_cast<Pool*>(pointer))
            {
                ptr_end->next = PoolHead;
                PoolHead = static_cast<Pool*>(pointer);
            }else{
                while(start && start->next < static_cast<Pool*>(pointer))
                    start = start->next;
                ptr_end->next = start->next;
                start->next = static_cast<Pool*>(pointer);
                }
                pointer.reset();
                segments += blockSize;
            }
        }

template<class PointerType, class MemoryType> void PoolAllocator<PointerType, MemoryType>::defragmentation() {
        IPointers<PointerType>* sortedPointers[pointerCounter];
        for(int i = 0; i < pointerCounter; ++i)
            sortedPointers[i] = queueHead + i;
        SortPointers(sortedPointers, pointerCounter);
        char *memoryPointer = memory;
        for(unsigned i = 0; i < pointerCounter; ++i)
        {
           const PointerType *ptr = sortedPointers[i]->get();
            if(ptr){
            sortedPointers[i]->accept(this,reinterpret_cast<const PointerType *>(memoryPointer), sortedPointers[i]->size());
            size_t blockSize = bytes(*sortedPointers[i]);
            memmove(memoryPointer, ptr, blockSize);
            memoryPointer += blockSize;

            }
        }

        Pool poolObject;
        Pool *poolPtr = reinterpret_cast<Pool*>(memoryPointer);
        PoolHead = poolPtr;
        while(poolPtr - reinterpret_cast<Pool*>(queueHead)){
            memmove(poolPtr, &poolObject, sizeof(Pool));
            ++poolPtr;
            (poolPtr - 1)->next = poolPtr;
        }
        (poolPtr - 1)->next = nullptr;
}
#endif
