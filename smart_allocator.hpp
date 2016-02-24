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

#ifndef SMART_ALLOCATOR_HPP_INCLUDED
#define SMART_ALLOCATOR_HPP_INCLUDED
#include "algorithms.hpp"
#include "allocator.hpp"
/*! \addtogroup allocator Memory allocator
    @{
*/
template <class PointerType, class MemoryType>
    class SmartAllocator: public IMemoryAllocator<PointerType>{
private:
    char *memory;
    size_t size;
    IPointers<PointerType>*  peak;
    PointerHeap<PointerType,const IPointers<PointerType>*, MemoryType > heap;
    int rest;
    /// _size - number of elements of PointerType
    void createDataPointer();

    SmartAllocator(SmartAllocator<PointerType, MemoryType>&);
    SmartAllocator<PointerType, MemoryType>& operator=
                                        (const SmartAllocator<PointerType
                                                            , MemoryType>&);
protected:
public:
    SmartAllocator(MemoryType *block, size_t elements);

    IPointers<PointerType>&  alloc(size_t _size);
    void realloc(IPointers<PointerType>& pointer, size_t _size);
    void free(IPointers<PointerType>& Pointer);
    void setHeap(const IPointers<PointerType>**& oldPtr,const IPointers<PointerType>*const*& newPtr) const;
    void set(PointerType*& oldPtr,const PointerType*& newPtr) const;
    void setSize(size_t& oldSize,const size_t& newSize) const;
    void defragmentation();
    virtual ~SmartAllocator(){}
    };
/*! @} */
template <class PointerType, class MemoryType>  SmartAllocator<PointerType, MemoryType>::SmartAllocator(MemoryType *block, size_t elements)
                            :memory(reinterpret_cast<char*>(block))
                            ,size(elements*sizeof(MemoryType))
    {
        ///initializes the first empty smart pointer with zero to return by default
            DataPointer<PointerType> SmartObject;

            peak = reinterpret_cast< DataPointer<PointerType>* >(memory + size - sizeof(SmartObject));
            PointerHeap<PointerType,const IPointers<PointerType>*, MemoryType >
                                        tempHeap(reinterpret_cast<const IPointers<PointerType>** >(peak) - 2);
            heap = tempHeap;
            memmove(peak, &SmartObject, sizeof(SmartObject));
            heap.push(peak);
            rest = reinterpret_cast<char *>(const_cast<IPointers<PointerType> **>(&heap)) - memory;
    }

template<class PointerType, class MemoryType>  IPointers<PointerType>&  SmartAllocator<PointerType, MemoryType>::alloc(size_t _size) {
    size_t sizeInBytes = _size * sizeof(PointerType);
    const PointerType * endOfData = heap[heap.getSize() - 1]->get() + heap[heap.getSize() - 1]->size();
    if(heap[0]->get()){
        //если не нашлось нулевого поинтера, то нужно создать умника с ним
        try{
            createDataPointer();
        }
        catch(uint16_t e)
        {
            std::cout<<"ERROR: Not enough memory to allocate."<<std::endl;
            std::cout<<"At least "<<e<<" bytes required."<<std::endl;
            throw std::bad_alloc();
        }
        if(reinterpret_cast<const PointerType *>(&heap) - endOfData < heap.getSize() + 1)
             defragmentation();

        heap.push(peak);
    }
    const IPointers<PointerType>* pointer= heap[0];

    if(rest < static_cast<int> (sizeInBytes))
            throw std::bad_alloc();

    if(_size)
    {
        if(&heap - reinterpret_cast<const IPointers<PointerType>* const*>(endOfData)
           <= sizeInBytes/sizeof(IPointers<PointerType>*))
            defragmentation();

        if(!endOfData)
            pointer->accept(this, reinterpret_cast<PointerType*>(memory), _size);
        else
            pointer->accept(this, endOfData, _size);
        heap.SiftDown(0);
    }
    rest -= _size * sizeof(PointerType);
    return *const_cast<IPointers<PointerType> *>(pointer);
}

/** \brief Creates new empty smart pointer.
 * Calls defragmentation if space is not enough to gain heap.
 * Changes heap location if there's not enough spase to create smart pointer and to gain heap
 * \param
 * \param
 * \return
 *
 */

template<class PointerType, class MemoryType>  void  SmartAllocator<PointerType, MemoryType>::createDataPointer(){

        const char* endOfHeap = reinterpret_cast<const char *>(&heap + heap.getSize());
        const IPointers<PointerType> *const* endOfData = reinterpret_cast<const IPointers<PointerType>*const*>(heap[heap.getSize() - 1]->get() + heap[heap.getSize() - 1]->size());
        DataPointer<PointerType> pointerObject;
        uint16_t  sizeOfPointerBlock = sizeof(IPointers<PointerType>*) + sizeof(pointerObject);

        if(rest < sizeOfPointerBlock)
          throw sizeOfPointerBlock;
        if((reinterpret_cast<const char *>(peak) - endOfHeap) < sizeOfPointerBlock)
          {
            //1. copy heap pointer
            const IPointers<PointerType> *const* newHeapPtr = &heap - (2*sizeOfPointerBlock)/sizeof(&heap);
            rest -= (&heap - newHeapPtr)*sizeof(IPointers<PointerType> *);
            heap.acceptLocationChange(this, newHeapPtr);

          }
        --peak;
        memmove(peak, &pointerObject, sizeof(pointerObject));
          }


template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::set(PointerType*& oldPtr
                                                                                    ,const PointerType*& newPtr)const
{
    oldPtr = const_cast<PointerType*>(newPtr);
}
template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::setSize(size_t& oldSize,const size_t& newSize) const{
    oldSize = newSize;
}

template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::setHeap(const IPointers<PointerType>**& oldPtr
                                                                                                     ,const IPointers<PointerType>*const*& newPtr)const
{
    const IPointers<PointerType>** tempPtr = oldPtr;
    oldPtr = const_cast<const IPointers<PointerType>**>(newPtr);
    memmove(oldPtr,tempPtr, heap.getSize()*sizeof(IPointers<PointerType>*));

}
template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::defragmentation(){

    char *memoryPointer = memory;
    heap.sort();
    for(int i = 0; i < heap.getSize(); ++i)
    {
        size_t blockSize = heap[i]->size()*sizeof(PointerType);
        memmove(memoryPointer, heap[i]->get(), blockSize);
        heap[i]->accept(this, reinterpret_cast<PointerType *>(memoryPointer), heap[i]->size());
        memoryPointer += blockSize;
    }
}
template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::realloc(IPointers<PointerType>& pointer, size_t _size){
    if(pointer == NULL){
        pointer = alloc(_size);
        return;
    }
    else
        if(_size == 0 && pointer != NULL){
            free(pointer);
             return;
        }

    IPointers<PointerType>& ptr = alloc(_size);

    for(int i = 0; i < pointer.size() && i < ptr.size(); ++i)
        ptr[i] = pointer[i];
    pointer = ptr;
    free(ptr);
}

template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::free(IPointers<PointerType>& pointer){
    //put zero to smart pointer
    rest += pointer.size()*sizeof(PointerType);
    pointer.reset();
    heap.sort();

}

#endif // SMART_ALLOCATOR_HPP_INCLUDED
