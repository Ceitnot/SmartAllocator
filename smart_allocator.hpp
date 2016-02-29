#ifndef SMART_ALLOCATOR_HPP_INCLUDED
#define SMART_ALLOCATOR_HPP_INCLUDED

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


#include "allocator.hpp"
#include "algorithms.hpp"

/*! \addtogroup allocator Memory allocator
    @{
*/
#pragma once
template <class PointerType, class MemoryType>
    class SmartAllocator: public IMemoryAllocator<PointerType>{
private:
    char *memory;
    size_t size;
    bool isHeap;
    IPointers<PointerType>*  peak;
    Heap<PointerType,
    const IPointers<PointerType>*, MemoryType >
                                         BinaryHeap;///< Binary heap is designed to work with certain classes like SmartAllocator.
    int rest;
    void createExtendedPointer();
    SmartAllocator(SmartAllocator<PointerType, MemoryType>&);
    SmartAllocator<PointerType, MemoryType>& operator=
                                        (const SmartAllocator<PointerType
                                                            , MemoryType>&);
protected:
    void setSize(size_t& oldSize,const size_t& newSize) const;
public:
    /** \brief The first initialization of allocator.
 * \param [in] block - memory of MemoryType.(v0.9.6 doesn't support types of STL containers)
 * \param [in] number of elements in memory of MemoryType.
 * \param [in] heap - If this memory has been gotten dynamically.
 */
    SmartAllocator(MemoryType *block, size_t elements, bool heap = false);
    virtual ~SmartAllocator();
    IPointers<PointerType>&  alloc(size_t _size);
    void realloc(IPointers<PointerType>& pointer, size_t _size);
    void free(IPointers<PointerType>& Pointer);
    void defragmentation();

    void setHeap(const IPointers<PointerType>**& oldPtr,const IPointers<PointerType>*const*& newPtr) const;
    void set(PointerType*& oldPtr,const PointerType*& newPtr) const;
    bool heap() const;
    };
/*! @} */

#include"smart_allocator.cpp"
#endif // SMART_ALLOCATOR_HPP_INCLUDED
