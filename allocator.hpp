#ifndef ALLOCATOR_HPP_INCLUDED
#define ALLOCATOR_HPP_INCLUDED
#include<cstdlib>
/** \file
 *  \brief Header with class declaration


*/

/** \brief The allocator class supports a more efficient memory management.
    Supports a dynamic memory pool in order to map limited memory block;
    \authors Kashtanova Anna Viktorovna
    \version 0.1
    \date 12.01.2016
    \warning here is no defragmentation module. Status: in the pipeline.
    \todo Needed to add memory compaction
 */


/**
    \defgroup smart_allocator Memory allocator
    \brief Efficient memory management
    @{
*/



template <class Type> class SharedPtr;



struct Counter
{
    private:
    size_t count;

    public:
    size_t getCounter(){
        return count;
    }
    void Add()
    {
        ++count;
    }

    int Clean()
    {
        return --count;
    }
};

class Visitor {
    public:

        virtual void* VisitSharedPointer(SharedPtr<void> const * shared) = 0;
        virtual ~Visitor() { }
};



 class Pointers{
 public:
    virtual void accept(Visitor * vis) = 0;

};



template <class Type> class SharedPtr: public Pointers
{

     Counter      *cr;
     Type  *ptr_;

protected:
    void swap(SharedPtr& shp){

        Counter  * const tmp = cr;
        cr = shp.cr;
        shp.cr = tmp;
        Type * const tmp_ptr_ = ptr_;
        ptr_ = shp.ptr_;
        shp.ptr_ = tmp_ptr_;

     }

public:

     explicit SharedPtr(Type *ptr = 0):ptr_(ptr){

           cr = new Counter();
           cr->Add();

     }

    SharedPtr(const SharedPtr& ptr) {

        ptr_= ptr.get();

            ptr.cr->Add();
        cr = ptr.cr;

    }
   ~SharedPtr(){
        if(cr->Clean()==0){
            delete cr;

            delete ptr_;
        }
     }

    SharedPtr& operator=(const SharedPtr& ptr){

        if(this != &ptr)
            SharedPtr(ptr).swap(*this);

        return *this;
    }
     Type* get() const{
        return ptr_;
     }
     void reset(Type *ptr = 0){
            if(!cr->Clean())
                delete ptr_;

            ptr_ = ptr;
            cr->Add();

     }
//     Type& operator*() const{
    //    return *ptr_;
    // }
     Type* operator->() const{
        return ptr_;
     }
     void accept(Visitor *vis){ vis->VisitSharedPointer(this);}
};


class SmartAllocator: public Visitor {

private:

     struct Pool{
        Pool():next(0){}
        Pool *next;
    };

    Pool *PoolHead;
    size_t size; // in bytes
    char *memory;
    unsigned segments;

protected:
    size_t bytes(void *pointer);

public:
  void* VisitSharedPointer(SharedPtr<void> const* shared){
       return shared->get();
    }
    /*!
    Initializes Smart Allocator by a linked list
    \param[in] block - available memory
    \param[in] bytes - memory capacity

    Simple use case:
   \code

    int A[848576];
    SmartAllocator A2(&A, 848576);

   \endcode
    */

        SmartAllocator(void *block, size_t bytes);

    /*!
    Allocates the first available units in the pool.
    \param[in] size the size of the requested memory
    */

        void *Alloc(size_t size);

    /*!
    Reallocates the first available units in the pool with new size.
    if memory block gets bigger new memory segments won't be initialized.
    if pointer equals zero behaves like Alloc(size)
    if size == 0 && pointer != 0 behaves like Free(pointer)
    \param[in] pointer memory to be reallocated
    \param[in] size the size of the requested memory
    */

        void *ReAlloc(void *pointer, size_t size);

    /*!
    Frees the units in the pool until it reaches
     another allocated block or unallocated memory.
    \param[in] pointer memory to be freed
    */
/*! @} */
        void Free(void *pointer);

        virtual ~SmartAllocator();



};


#endif // ALLOCATOR_HPP_INCLUDED
