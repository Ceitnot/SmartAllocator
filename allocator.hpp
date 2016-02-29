#ifndef ALLOCATOR_HPP_INCLUDED
#define ALLOCATOR_HPP_INCLUDED
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

#include<cstdlib>
#include<stdint.h>
#include<algorithm>
#include<cstring>
#include<iostream>

template <class Type> class IPointers;
template <class Type> class ExtendedPointer;
template <class Type> class IMemoryAllocator;

/**
    \defgroup pointers Smart Pointers
    \brief
    @{
*/

/**
    \defgroup abstract Abstract classes
    \ingroup pointers
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

 IPointers(size_t _size = 0, Type *ptr = nullptr)
                        :ptr_(ptr)
                        ,_size(_size)

    {}
public:
   virtual ~IPointers(){ }

/** \brief It safely returns the pointer to the memory location.
 * \warning It is not designed to change either the address of the data or the data itself.
 * \return const Type* const
 */
   virtual const Type* const get() const = 0;
   virtual size_t size() const = 0;
 /** \brief It sets to sero the pointer and the size of the data pointed to by pointer.
 * \return void
 */
   virtual void reset() = 0;

   virtual bool operator!=(const Type* const pointer) const = 0;
   virtual bool operator==(const Type* const pointer) const = 0;

   virtual Type& operator[](size_t i) = 0;
/** \brief Provides access to the object of Pointer by Visitor pattern in order to change the pointer.
 * \param visitor[in] - a pointer to the allocator - object.
 * \param newPtr[in]  - the pointer replaces the old one.
 * \param newSize[in]  - link to the size variable replaces the old one.
 * \warning It is designed for internal use.
 * \return void
 */
   virtual void accept(const IMemoryAllocator<Type> * visitor
                       , const Type* newPtr
                       , const size_t& newSize) const = 0;
   template<typename T> operator T() { return (const T)IPointers<Type>::ptr_; }

};

/*! @} */

template <class Type> class ExtendedPointer: public IPointers<Type>
{
       using IPointers<Type>::ptr_;
       using IPointers<Type>::_size;

       ExtendedPointer(ExtendedPointer<Type>&);
       Type operator*()const{ return *(this->ptr_); }
protected:

/**< auxiliary function */
   void swap( ExtendedPointer<Type> & shp){
        std::swap(this->ptr_, shp.ptr_);
        std::swap(this->isHeap, shp.isHeap);
        std::swap(this->_size, shp._size);
     }

   void reset(){
            this->ptr_ = nullptr;
            _size = 0;
     }
    ExtendedPointer<Type>& operator=(const ExtendedPointer<Type>& ptr){

        if(this != &ptr)
            ExtendedPointer(ptr).swap(*this);
        return *this;
    }
public:
     explicit ExtendedPointer(size_t size = 0, Type *ptr = nullptr, bool isHeap = false)
                                        : IPointers<Type>(size, ptr)

            { }

   ~ExtendedPointer(){
     }

    bool operator!=(const Type* const pointer) const { return this->ptr_ != pointer;}
    bool operator==(const Type* const pointer) const { return this->ptr_ == pointer;}
    Type& operator[](size_t i){
            return ptr_[i];
        }

   const Type* const get()const{ return this->ptr_; }
   size_t size() const { return this->_size;}
    void accept(const IMemoryAllocator<Type> * visitor, const Type* newPtr, const size_t& newSize)const;
};
/*! @} */
template<class Type>
     void ExtendedPointer<Type>::accept(const IMemoryAllocator<Type> * visitor, const Type* newPtr, const size_t& newSize)const{
      visitor->set( ptr_, newPtr);
      visitor->setSize(_size, newSize);
      }

/**
    \defgroup allocator Memory allocator
   \brief This allocator supports two ways to manage memory allocation:
                1. SmartAllocator. Faster then the second one. One can select required number of elements.
                2. PoolAllocator. It allows required number of bytes selection.
    \authors Kashtanova Anna Viktorovna
    \version 0.9.6
    \date 23.02.2016
    @{
*/

/**
    \defgroup interfaces Interfaces
    \ingroup allocator
    @{
*/
template <class Type>
class  IMemoryAllocator{
public:
/*!
* It allocates the first available memory unit (number of elements of PointerType with SmartAllocator and number of bytes with PoolAllocator).
* If size equals to zero and then container will contain zero pointer.
*\throw std::bad_alloc()
*\param[in] size - the size of the requested memory in bytes.
*\return IPointers<PointerType>& - a reference to pointer container
*\warning This method may allocate more memory than one needs. It's always multiple of 8 bytes and never less than that.
*/
        virtual IPointers<Type>&  alloc(size_t _size) = 0;
/*!
*It reallocates the first available units in the pool with new size.
*The memory is not initialized.
*If pointer equals zero behaves like alloc(_size).
*If size == 0 && pointer != 0 behaves like free(pointer).
*\param[in] pointer - reference to container with pointer to memory to be reallocated
*\param[in] size - the size of the requested memory
*/
        virtual void realloc(IPointers<Type>& pointer, size_t _size) = 0;
/*!
*It frees the memory of certain size.
*If pointer contains zero, then the method does nothing.
*\param[in] pointer memory to be freed
*/
        virtual void free(IPointers<Type>& pointer) = 0;
        virtual ~IMemoryAllocator(){}
/*!
* It implements memory compaction algorithm.
* It can be used manually, but it is performed automatically every time it is necessary.
*\returns void
*/
        virtual void defragmentation() = 0;
        virtual void set(Type*& oldPtr, const Type*& newPtr) const = 0;
/*!
* It is designed as an auxiliary method for changing the pointer to memory block.
* Can be implemented by descendant class.
*\returns void
*/
        virtual void setSize(size_t& oldSize,const size_t& newSize) const = 0;
/*!
*\returns bool - If memory in SmartAllocator has been gotten dynamically.
*/
        virtual bool heap() const = 0;
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
    bool isHeap;
    unsigned pointerCounter;
    ExtendedPointer<PointerType>* queueHead;

    void createExtendedPointer();
    PoolAllocator(PoolAllocator<PointerType, MemoryType>&);
    PoolAllocator<PointerType, MemoryType>& operator = (const PoolAllocator<PointerType, MemoryType>&);
protected:
    size_t bytes(IPointers<PointerType>& pointer);
    void setSize(size_t& oldSize, const size_t& newSize) const;
public:
/** \brief The first initialization of allocator.
 *
 * \param [in] block - memory of MemoryType.(v0.9.6 doesn't support types of STL containers)
 * \param [in] number of elements in memory of MemoryType.
 * \param [in] heap - If this memory has been gotten dynamically.
 */
    PoolAllocator(MemoryType *block, size_t elements, bool heap = false);
    ~PoolAllocator();
    void set(PointerType*& oldPtr,const PointerType*& newPtr) const;
    bool heap() const;
    IPointers<PointerType>&  alloc(size_t _size);
    void realloc(IPointers<PointerType>& pointer, size_t size) ;
    void free(IPointers<PointerType>& Pointer);
    void defragmentation();

};
/*! @} */
#include"allocator.cpp"

#endif // ALLOCATOR_HPP_INCLUDED
