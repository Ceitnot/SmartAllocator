
/** \file
 *  \brief
 \copyright Copyright 2016 Kashtanova Anna Viktorovna
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

#ifndef ALLOCATOR_HPP_INCLUDED
#define ALLOCATOR_HPP_INCLUDED
#include<cstdlib>
#include<stdint.h>
#include<algorithm>
#include<cstring>
#include<iostream>

/** \brief This allocator supports two ways to manage memory allocation:
                1. PoolAllocator. It allows required number of bytes selection.
                2. SmartAllocator. Faster then the first one. One can select required number of elements.

    \authors Kashtanova Anna Viktorovna
    \version 1.0
    \date 23.02.2016
 */

template <class Type> class IPointers;
template <class Type> class DataPointer;
template <class Type> class IMemoryAllocator;

template<class Type> IPointers<Type> * const findAddr(IPointers<Type> * const dataPointer,  const Type* const  ptr, unsigned number){
    IPointers<Type> *tmp = dataPointer;
    for(int i = 0; i < number; ++i){
       if(*tmp == ptr)
            return tmp;
       tmp = dataPointer + 1;
    }
    return NULL;
}

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
/**
    \defgroup pointers Smart Pointers
    \brief
    @{
*/
template <class Type>
 class IPointers{
private:
    IPointers(const IPointers<Type>&);
protected:
    mutable size_t _size;
    mutable Type  *ptr_;
    bool         isHeap;

 IPointers(size_t _size = 0, Type *ptr = NULL, bool Heap = 0)
                        :ptr_(ptr)
                        ,_size(_size)
                        ,isHeap(Heap)

    {}
public:
   virtual ~IPointers(){ }

   virtual const Type* get() const = 0;
   virtual size_t size() const = 0;
   virtual bool heap() const = 0;
   virtual void reset(Type *ptr = NULL, size_t newSize = 0) = 0;

   virtual bool operator!=(const Type* const pointer) const = 0;//{   return this->ptr_ != pointer;}
   virtual bool operator==(const Type* const pointer) const = 0;//{   return this->ptr_ == pointer;}

   virtual Type& operator[](size_t i) = 0;//{ return this->ptr_[i];}
   virtual Type operator*() const = 0;

   virtual void accept(const IMemoryAllocator<Type> * visitor
                       , const Type* newPtr
                       , const size_t& newSize) const = 0;
   template<typename T> operator T() { return (const T)IPointers<Type>::ptr_; }

};



template <class Type> class DataPointer: public IPointers<Type>
{
       using IPointers<Type>::ptr_;
       using IPointers<Type>::isHeap;
       using IPointers<Type>::_size;

       DataPointer(DataPointer<Type>&);
protected:

/**< auxiliary function */
   void swap( DataPointer<Type> & shp){
        std::swap(this->ptr_, shp.ptr_);
        std::swap(this->isHeap, shp.isHeap);
        std::swap(this->_size, shp._size);
     }

public:
     explicit DataPointer(size_t size = 0, Type *ptr = NULL, bool isHeap = false)
                                        : IPointers<Type>(size, ptr, isHeap)

            { }

   ~DataPointer(){
        if(this->isHeap){
            delete this->ptr_;
        }
     }

    bool operator!=(const Type* const pointer) const {   return this->ptr_ != pointer;}
    bool operator==(const Type* const pointer) const {   return this->ptr_ == pointer;}
    Type& operator[](size_t i){
        if(i < this->_size)
            return ptr_[i];
        else
            return ptr_[i - _size];
        }
    DataPointer<Type>& operator=(const DataPointer<Type>& ptr){

        if(this != &ptr)
            DataPointer(ptr).swap(*this);
        return *this;
    }

   Type operator*()const{return *(this->ptr_);}

   const  Type* get()const{return this->ptr_;}

   size_t size() const {return this->_size;}

   void reset(Type *ptr = NULL, size_t newSize = 0){
            if(this->isHeap)
                delete this->ptr_;
            this->ptr_ = ptr;
            _size = newSize;
     }

     bool heap()const{
        return this->isHeap;
     }
void accept(const IMemoryAllocator<Type> * visitor, const Type* newPtr, const size_t& newSize)const;
};
/** \brief Provides access to the object of DataPointer by Visitor pattern in order to change pointer
 * \param visitor - a reference to the allocator - object
 * \param newPtr  - pointer replaces old pointer
 * \return void
 */


template<class Type>
     void DataPointer<Type>::accept(const IMemoryAllocator<Type> * visitor, const Type* newPtr, const size_t& newSize)const{
      visitor->set( ptr_, newPtr);
      visitor->setSize(_size, newSize);
      }
/*! @} */
/**
    \defgroup allocator Memory allocator
    \brief
    @{
*/

/**
    \defgroup interfaces Interfaces
    \ingroup allocator
    @{
*/
template <class Type>
class  IMemoryAllocator{/**< visitor */
public:
        virtual IPointers<Type>&  alloc(size_t _size) = 0;
        virtual void realloc(IPointers<Type>& pointer, size_t _size) = 0;
        virtual void free(IPointers<Type>& pointer) = 0;
        virtual ~IMemoryAllocator(){}
        virtual void defragmentation() = 0;
        virtual void set(Type*& oldPtr, const Type*& newPtr) const = 0;
        virtual void setSize(size_t& oldSize,const size_t& newSize) const = 0;
};
/*! @} */

template <class PointerType, class MemoryType>
    class PoolAllocator: public IMemoryAllocator<PointerType> {

private:

     struct Pool{
        Pool():next(0){}
        Pool *next;
    };

    Pool      *PoolHead;
    char      *memory;

    size_t size;
    int segments;
    unsigned pointerCounter;
    DataPointer<PointerType>* queueHead;
    void createDataPointer();


    PoolAllocator(PoolAllocator<PointerType, MemoryType>&);
    PoolAllocator<PointerType, MemoryType>& operator = (const PoolAllocator<PointerType, MemoryType>&);
protected:
    size_t bytes(IPointers<PointerType>& pointer);
public:

    void set(PointerType*& oldPtr,const PointerType*& newPtr) const;
    void setSize(size_t& oldSize, const size_t& newSize) const ;
    PoolAllocator(MemoryType *block, size_t elements);

    /*!
    Initializes Smart Allocator by a linked list
    \param[in] block - available memory
    \param[in] bytes - memory capacity

    Simple use case:
   \code

    int A[848576];
    PoolAllocator A2(&A, 848576);

   \endcode
    */


    /*!
    Allocates the first available units in the pool.
    \param[in] size the size of the requested memory
    */

        IPointers<PointerType>&  alloc(size_t Size);

    /*!
    Reallocates the first available units in the pool with new size.
    if memory block gets bigger new memory segments won't be initialized.
    if pointer equals zero behaves like Alloc(size)
    if size == 0 && pointer != 0 behaves like Free(pointer)
    \param[in] pointer memory to be reallocated
    \param[in] size the size of the requested memory
    */

      void realloc(IPointers<PointerType>& pointer, size_t size) ;

    /*!
    Frees the units in the pool until it reaches
     another allocated block or unallocated memory.
    \param[in] pointer memory to be freed
    */

       void free(IPointers<PointerType>& Pointer);
       void defragmentation();
       virtual ~PoolAllocator(){}

    int checkMemory()const{
        int counter = 0;
        Pool *first = PoolHead;
        while(first){
        ++counter;
        first = first->next;
        }
        return (counter)*sizeof(Pool);
    }
};
/*! @} */
template <class PointerType, class MemoryType>  PoolAllocator<PointerType, MemoryType>::PoolAllocator(MemoryType *block, size_t elements)
                            :memory(reinterpret_cast<char*>(block))
                            ,size(elements*sizeof(MemoryType))
                            ,segments(size/sizeof(Pool)
                                      - sizeof(DataPointer<PointerType>)/sizeof(Pool))// количество кусков по 8 байт
                            ,pointerCounter(1)
                        {
            Pool * PoolSecond = NULL;
            Pool PoolObject;
            for(size_t i = 0 ; i < segments ; ++i)
            {
                PoolHead    = reinterpret_cast<Pool*>(memory) + i + 1;
                PoolSecond  = reinterpret_cast<Pool*>(memory) + i;
                memmove(PoolSecond, &PoolObject, sizeof(PoolObject));
                PoolSecond->next = PoolHead;
            }
            PoolHead = (Pool*)memory;
            DataPointer<PointerType> SharedObject;
            memmove(PoolSecond->next, &SharedObject, sizeof(SharedObject));
            queueHead = reinterpret_cast< DataPointer<PointerType>* >(PoolSecond->next);
            PoolSecond->next = NULL;
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

template<class PointerType, class MemoryType>  IPointers<PointerType>&  PoolAllocator<PointerType, MemoryType>::alloc(size_t Size) {

            size_t realSize = (Size%sizeof(Pool)) ?  Size/sizeof(Pool) + 1 : Size/sizeof(Pool) ;
            IPointers<PointerType>* top = queueHead;
            IPointers<PointerType>* vacant = findAddr(queueHead, reinterpret_cast<PointerType*>(NULL), pointerCounter);

                if(vacant){
                    top = vacant;
                }else{
                    createDataPointer();
                    top = queueHead;
                }

            if( segments > realSize && Size != 0  ){

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
                        top->accept(this, reinterpret_cast<PointerType *>(ptrSecond->next), Size/sizeof(PointerType));
                        ptrSecond->next = ptrFirst;//copy the one's after last block address to the "next" field.
                }else{
                        top->accept(this, reinterpret_cast<PointerType *>(ptrSecond), Size/sizeof(PointerType));
                        PoolHead = ptrFirst;
                    }
          }
             return  *top;

     }

template<class PointerType, class MemoryType> void PoolAllocator<PointerType, MemoryType>::createDataPointer(){
    if(segments >= sizeof(DataPointer<PointerType>)/sizeof(Pool)){
        DataPointer<PointerType> tmp;
        --queueHead;
        memmove(queueHead, &tmp, sizeof(tmp));
        Pool* end = nullptr;
        ++pointerCounter;
        uint8_t reminder = (sizeof(tmp)*pointerCounter)%sizeof(Pool);
        if(!reminder)
            end = reinterpret_cast<Pool *>(queueHead) - 1;
        else
            end = reinterpret_cast<Pool *>(reinterpret_cast<char *>(queueHead) - reminder + sizeof(Pool));
        end->next = NULL;

        segments -= sizeof(tmp)/sizeof(Pool);
   }else{
    throw std::bad_alloc();
   }
}

 template<class PointerType, class MemoryType> inline size_t PoolAllocator<PointerType, MemoryType>::bytes(IPointers<PointerType>& pointer){

            return pointer.size()*sizeof(PointerType);
        }

 template<class PointerType, class MemoryType>
    void PoolAllocator<PointerType, MemoryType>::realloc(IPointers<PointerType>& pointer, size_t _size) {

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

            for(size_t i = 0; i < _size/sizeof(PointerType) && i < pointer.size(); ++i)
                ptr[i] = pointer[i];

            free(pointer);
            pointer = ptr;

        }

    template<class PointerType, class MemoryType> void PoolAllocator<PointerType, MemoryType>::free(IPointers<PointerType>& pointer) {
            if(pointer != NULL){
                Pool* ptr = static_cast<Pool*>(pointer);
                Pool* ptr_end = NULL;

                Pool* start = PoolHead;
                Pool PoolObject;
                size_t blockSize = (bytes(pointer)%sizeof(Pool)) ?
                  bytes(pointer)/sizeof(Pool) + 1 : bytes(pointer)/sizeof(Pool);

                  for(unsigned i = 0 ; i < blockSize; ++i)
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
                    int i = 0;

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
        unsigned count1 = 0;
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
        (poolPtr - 1)->next = NULL;
}

#endif // ALLOCATOR_HPP_INCLUDED
