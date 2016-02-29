#ifndef ALGORITHMS_HPP_INCLUDED
#define ALGORITHMS_HPP_INCLUDED
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


#include <assert.h>
#include <cmath>
#include <iostream>
#include "smart_allocator.hpp"

template <class PointerType, class MemoryType>
    class SmartAllocator;
/**
    \defgroup auxiliary_algorithms Auxiliary algorithms
    \ingroup allocator
    @{
*/
namespace {

/** \brief Binary heap. Utility for SmartAllocator
 */

    template<class KeyType, class ElementType, class MemoryType>
    class Heap{

        size_t       size;
        ElementType* elements;

    public:
        Heap()
        :size(0)
        ,elements(nullptr)
        {}
        Heap(ElementType *_elements)
        :size(0)
        ,elements(_elements)
        {}

    Heap(size_t _size, ElementType *_elements)
        : size(_size)
        {
            for(int i = 0; i < _size; ++i){
                push(elements[i]);
            }
        }

    Heap(const Heap <KeyType, ElementType, MemoryType>& heap)
        {
            size = heap.size;
            memcpy(elements, heap.elements, size);
        }

        void push(ElementType element){

            increaseSize();
            elements[size - 1] = element;
            if(elements[size - 1]->get() < elements[parent(size - 1)]->get())
                SiftUp(size - 1);
            if(elements[size - 1]->get() > elements[leftChild(size - 1)]->get()
                            && elements[size - 1]->get() > elements[rightChild(size - 1)]->get())
                SiftDown(size - 1);

        }
        ~Heap(){}
        Heap<KeyType, ElementType, MemoryType> & operator=(Heap<KeyType
                                                            , ElementType
                                                            , MemoryType> & ptr)
        {
            elements = ptr.elements;
            size = ptr.size;
        }
        const ElementType * operator&() const { return elements; }
        size_t getSize() const { return size; }

        ElementType operator[](size_t i) const {
            if(i < size)
                return elements[i];
            else
                return elements[i - size];
        }

        void SiftDown(size_t i = 0){

            if(size > i + 1){
                size_t child = 0;
                size_t indexOfLChild = leftChild(i);

                if(indexOfLChild >= size) return;

                size_t indexOfRChild = rightChild(i);

                if(indexOfRChild>= size) child = indexOfLChild;
                else
                    child  = (elements[indexOfRChild]->get() < elements[indexOfLChild]->get() ) ?
                                    indexOfRChild : indexOfLChild ;
                while((child < size)&&(elements[i]->get() > elements[child]->get() ))
                {
                    std::swap(elements[i], elements[child]);
                    i = child;
                    indexOfRChild = rightChild(i);
                    indexOfLChild = leftChild(i);
                    if(indexOfLChild >= size)
                        child = size;
                    if(indexOfRChild>= size) child = indexOfLChild;
                    else
                        child = (elements[indexOfRChild]->get()  < elements[indexOfLChild]->get() ) ?
                                    indexOfRChild : indexOfLChild ;
                }
            }
        }
        void SiftUp(size_t i = 0){

            size_t indexOfParent = parent(i);

            while(elements[indexOfParent]->get() > elements[i]->get()){
                std::swap(elements[indexOfParent], elements[i]);
                i = indexOfParent;
                indexOfParent = parent(indexOfParent);
            }
        }

        void sort()
        {
            int i, j;
            ElementType key;

            for (i = 1; i < size; i++)
            {
                key = elements[i];
                j = i-1;

            /* Move elements of A[0..i-1], that are greater than key, to one
                position ahead of their current position.
                This loop will run at most k times */
                while (j >= 0 && elements[j]->get() > key->get())
                {
                    elements[j+1] = elements[j];
                    --j;
                }
                elements[j+1] = key;
            }
        }
        void acceptLocationChange(const SmartAllocator<KeyType, MemoryType>* const allocator, const ElementType * newLocation)
        {
            assert(newLocation && newLocation < elements);
            allocator->setHeap(elements, newLocation);
        }
    protected:
        void increaseSize(){
         /**< increase size of keys array*/
            ++size;
        }

        int leftChild(size_t i){
            int child = i*2 + 1;
            return  (size > child) ? child : i;
        }

        int rightChild(size_t i){
            int child = i*2 + 2;
            return  (size > child) ? child : i;
        }


        void decreaseSize(){ --size; }

        int parent(size_t children){

            if(children > 0)

            return (children % 2) ? children/2 : children/2 - 1;

            else
                return 0;
        }

        void set(KeyType*& oldKey, const KeyType*& newKey)const{
            oldKey = const_cast<KeyType*>(newKey);
        }

        bool isEmpty(){
            return (size > 0) ? false : true;
        }
    };

}

/** \brief Search for pointer in the unsorted array.
The running time is O(n)
 */
template<class Type> IPointers<Type> * const findAddr(IPointers<Type> * const pointer,  const Type* const  ptr, unsigned number){
    IPointers<Type> *tmp = pointer;
    for(int i = 0; i < number; ++i){
       if(*tmp == ptr)
            return tmp;
       tmp = pointer + 1;
    }
    return nullptr;
}
/** \brief Merge sort for pointers.
 */
template<class Type> void  SortPointers(IPointers<Type> ** const sorted, unsigned len){

    if (len < 2) return;

    unsigned  mid =  len/2;
    IPointers<Type> *left[mid];
    IPointers<Type> *right[len - mid];

    for (int i = 0; i < mid; ++i)
        left[i] = sorted[i];
    for (int j = mid; j < len; ++j)
        right[j - mid] = sorted[j];

    SortPointers(left, mid);
    SortPointers(right, len - mid);
    MergePointers(sorted, left, mid, right, len - mid);
}
template<class Type> void  MergePointers(IPointers<Type>** const sorted
                                        , IPointers<Type>** const left
                                        , unsigned sizeL
                                        , IPointers<Type> ** const right
                                        , unsigned sizeR){
    unsigned i = 0, j = 0, k = 0;

    while(i < sizeL && j < sizeR )
    {
        if(left[i]->get() <= right[j]->get())
        {
            sorted[k] = left[i];
             ++i;
        }
        else
        {
            sorted[k] = right[j];
             ++j;
        }
        ++k;
    }
    while(i < sizeL)
    {
        sorted[k] = left[i];
        ++i; ++k;
    }
    while(j < sizeR)
    {
        sorted[k] = right[j];
        ++j; ++k;
    }
}
/*! @} */
#endif
