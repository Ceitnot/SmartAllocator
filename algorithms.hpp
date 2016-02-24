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

#ifndef ALGORITHMS_HPP_INCLUDED
#define ALGORITHMS_HPP_INCLUDED
#include <assert.h>
#include <cmath>
#include <iostream>
//#include <cstring>
#include "smart_allocator.hpp"
template <class PointerType, class MemoryType>
    class SmartAllocator;
/**
    \defgroup auxiliary_algorithms Auxiliary algorithms
    \ingroup allocator
    @{
*/
template<class KeyType, class ElementType, class MemoryType>
class HeapOfPointers{

    size_t       size;
    ElementType* elements;

public:
   HeapOfPointers()
    :size(0)
    ,elements(NULL)
    {}
    HeapOfPointers(ElementType *_elements)
    :size(0)
    ,elements(_elements)
    {}

   HeapOfPointers(size_t _size, ElementType *_elements)
    : size(_size)
    {
        for(int i = 0; i < _size; ++i){
            push(elements[i]);
        }
    }

HeapOfPointers(const HeapOfPointers <KeyType, ElementType, MemoryType>& heap)
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
    ~HeapOfPointers(){}
     HeapOfPointers<KeyType, ElementType, MemoryType> & operator=(HeapOfPointers<KeyType
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


    void decrease(){ --size; }

    ElementType extractMinKeyObj(){//O(Log(n))

        ElementType MinKeyObj = elements[0];
        elements[0]     = elements[size-1];

        SiftDown();
        decrease();
        return MinKeyObj;
    }

  int parent(size_t children){

    if(children > 0)

        return (children % 2) ? children/2 : children/2 - 1;

    else
        return 0;
}

    size_t findIndex(KeyType* elem){
        size_t counter = 0;
        for(size_t i = 0; i < size; ++i)
            if(&elements[i] == elem)
                counter = i;

    return counter;
    }

    void deleteObj(KeyType* key){
        size_t tempKey = findIndex(key);
        elements[tempKey] = elements[size-1];
        SiftDown(tempKey);
        decrease();
    }

    void changeMinKey(KeyType* newKey){
        elements[0]->accept(*this, newKey);
        SiftDown(0);
    }

    void set(KeyType*& oldKey, const KeyType*& newKey)const{
        oldKey = const_cast<KeyType*>(newKey);
    }

    void printHeap(){
        for(int i = 1; i <=size; i*=2)
        {
            for(int t = 0; t < std::log2(size - i*2); ++t)
                std::cout<<"\t";
            for(int j = i - 1; (j < (i-1)*2+1)&&(j<size) ; ++j)
                std::cout<<"("<<elements[j]->get()<<") \t\t";

            std::cout<<std::endl;
            std::cout<<std::endl;
        }
    }
    void printHeap2(){
        for(int i = 0; i < size; ++i)
            std::cout<<elements[i]->get()<<" ";
        std::cout<<std::endl;
    }
    bool isEmpty(){
        return (size > 0) ? false : true;
    }
};
/*! @} */
#endif
