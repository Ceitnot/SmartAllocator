#ifndef SMART_ALLOCATOR_CPP_INCLUDED
#define SMART_ALLOCATOR_CPP_INCLUDED
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
#include"smart_allocator.hpp"

template <class PointerType, class MemoryType>  SmartAllocator<PointerType, MemoryType>::SmartAllocator(MemoryType *block, size_t elements, bool heap)
                            :memory(reinterpret_cast<char*>(block))
                            ,size(elements*sizeof(MemoryType))
                            ,isHeap(heap)
    {
        ///initializes the first empty smart pointer with zero to return by default
        ExtendedPointer<PointerType> SmartObject;

        peak = reinterpret_cast< ExtendedPointer<PointerType>* >(memory + size - sizeof(SmartObject));
        Heap<PointerType,const IPointers<PointerType>*, MemoryType >
                                        tempHeap(reinterpret_cast<const IPointers<PointerType>** >(peak) - 2);
        BinaryHeap = tempHeap;
        memmove(peak, &SmartObject, sizeof(SmartObject));
        BinaryHeap.push(peak);
        rest = reinterpret_cast<const char *>(&BinaryHeap) - memory;
    }

template <class PointerType, class MemoryType>  SmartAllocator<PointerType, MemoryType>::~SmartAllocator()
{
    if(heap())
        delete [] memory;
}
/** \brief
 * \throw std::bad_alloc
 * \param[in] _size - number of PointerType elements needs to alloc
 * \return IPointers<PointerType>& - a reference to the pointer container
 *
 */

template<class PointerType, class MemoryType>  IPointers<PointerType>&  SmartAllocator<PointerType, MemoryType>::alloc(size_t _size) {
    size_t sizeInBytes = _size * sizeof(PointerType);
    const PointerType * endOfData = BinaryHeap[BinaryHeap.getSize() - 1]->get() + BinaryHeap[BinaryHeap.getSize() - 1]->size();
    if(BinaryHeap[0]->get()){
        //если не нашлось нулевого поинтера, то нужно создать умника с ним
        try{
            createExtendedPointer();
        }
        catch(uint16_t e)
        {
            std::cout<<"ERROR: Not enough memory to allocate."<<std::endl;
            std::cout<<"At least "<<e<<" bytes required."<<std::endl;
            throw std::bad_alloc();
        }
        if(reinterpret_cast<const PointerType *>(&BinaryHeap) - endOfData < BinaryHeap.getSize() + 1)
             defragmentation();

        BinaryHeap.push(peak);
    }
    const IPointers<PointerType>* pointer = BinaryHeap[0];

    if(rest < static_cast<int> (sizeInBytes))
            throw std::bad_alloc();

    if(_size)
    {
        if(&BinaryHeap - reinterpret_cast<const IPointers<PointerType>* const*>(endOfData)
           <= sizeInBytes/sizeof(IPointers<PointerType>*))
            defragmentation();

        if(!endOfData)
            pointer->accept(this, reinterpret_cast<PointerType*>(memory), _size);
        else
            pointer->accept(this, endOfData, _size);
        BinaryHeap.SiftDown();
    }
    rest -= _size * sizeof(PointerType);
    return *const_cast<IPointers<PointerType> *>(pointer);
}

/** \brief Creates new empty smart pointer.
 * Calls defragmentation if space is not enough to gain heap.
 * Changes heap location if there's not enough spase to create smart pointer and to gain heap
 * \throw uint16_t
 * \return void
 */
template<class PointerType, class MemoryType>  void  SmartAllocator<PointerType, MemoryType>::createExtendedPointer(){

        const char* endOfHeap = reinterpret_cast<const char *>(&BinaryHeap + BinaryHeap.getSize());
        const IPointers<PointerType> *const* endOfData = reinterpret_cast<const IPointers<PointerType>*const*>(BinaryHeap[BinaryHeap.getSize() - 1]->get() + BinaryHeap[BinaryHeap.getSize() - 1]->size());
        ExtendedPointer<PointerType> pointerObject;
        uint16_t  sizeOfPointerBlock = sizeof(IPointers<PointerType>*) + sizeof(pointerObject);

        if(rest < sizeOfPointerBlock)
          throw sizeOfPointerBlock;
        if((reinterpret_cast<const char *>(peak) - endOfHeap) < sizeOfPointerBlock)
          {
            const IPointers<PointerType> *const* newHeapPtr = &BinaryHeap - (2*sizeOfPointerBlock)/sizeof(&BinaryHeap);
            rest -= (&BinaryHeap - newHeapPtr)*sizeof(IPointers<PointerType> *);
            BinaryHeap.acceptLocationChange(this, newHeapPtr);
          }
        --peak;
        memmove(peak, &pointerObject, sizeof(pointerObject));
          }

/** \brief Method to set the the pointer in the container
 * \param[out] oldPtr
 * \param[in] newPtr
 * \return void
 * \warning It is not designed for the user code. Used only inside of pointer container.
 */

template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::set(PointerType*& oldPtr
                                                                                    ,const PointerType*& newPtr)const
{
    oldPtr = const_cast<PointerType*>(newPtr);
}

/** \brief Method to set size in pointer container
 * \param[out] oldSize - number of PointerType elements
 * \param[in] newSize - number of PointerType elements
 * \return void
 * \warning It is not designed for the user code. Used only inside of pointer container.
 */

template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::setSize(size_t& oldSize,const size_t& newSize) const{
    oldSize = newSize;
}
/** \brief Method to set the pointer to the heap
 * \param[out] oldPtr
 * \param[in] newPtr
 * \return void
 * \warning It is not designed for the user code. Used only inside of pointer container.
 */
template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::setHeap(const IPointers<PointerType>**& oldPtr
                                                                                                     ,const IPointers<PointerType>*const*& newPtr)const
{
    const IPointers<PointerType>** tempPtr = oldPtr;
    oldPtr = const_cast<const IPointers<PointerType>**>(newPtr);
    memmove(oldPtr,tempPtr, BinaryHeap.getSize()*sizeof(IPointers<PointerType>*));
}
template<class PointerType, class MemoryType> bool  SmartAllocator<PointerType, MemoryType>::heap()const

{
    return (isHeap) ? true : false;
}

template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::defragmentation(){

    char *memoryPointer = memory;
    BinaryHeap.sort();
    for(int i = 0; i < BinaryHeap.getSize(); ++i)
    {
        size_t blockSize = BinaryHeap[i]->size()*sizeof(PointerType);
        memmove(memoryPointer, BinaryHeap[i]->get(), blockSize);
        BinaryHeap[i]->accept(this, reinterpret_cast<PointerType *>(memoryPointer), BinaryHeap[i]->size());
        memoryPointer += blockSize;
    }
}
template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::realloc(IPointers<PointerType>& pointer, size_t _size){
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

    for(int i = 0; i < pointer.size() && i < ptr.size(); ++i)
        ptr[i] = pointer[i];
    pointer = ptr;
    free(ptr);
}

template<class PointerType, class MemoryType> void  SmartAllocator<PointerType, MemoryType>::free(IPointers<PointerType>& pointer){
    rest += pointer.size()*sizeof(PointerType);
    pointer.reset();
    BinaryHeap.sort();
}
#endif // SMART_ALLOCATOR_CPP_INCLUDED
