#ifndef ALLOCATOR_HPP_INCLUDED
#define ALLOCATOR_HPP_INCLUDED
#include<cstdlib>
#include <typeinfo>
#include<stdint.h>
#include<algorithm>
#include<cstring>
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


template <class Type> class IPointers;
template <class Type> class SharedPtr;
template <class Type> class IMemoryAllocator;



template <class Type>
 class IPointers{
protected:
    Type         *ptr_;
    bool         isHeap;
public:
   IPointers(Type *ptr = NULL, bool Heap = 0)
                :ptr_(ptr)
                ,isHeap(isHeap){}

   virtual ~IPointers(){ }

   virtual Type* get(size_t i) const = 0;
   virtual bool heap() const = 0;
   virtual void reset(Type *ptr = NULL) = 0;

   virtual bool operator!=(Type* pointer) const = 0;//{   return this->ptr_ != pointer;}
   virtual bool operator==(Type* pointer) const = 0;//{   return this->ptr_ == pointer;}

   virtual Type& operator[](uint8_t i) = 0;//{ return this->ptr_[i];}
   virtual Type operator*() const = 0;

   template<typename T> operator T() { return (T)IPointers<Type>::ptr_; }

};



template <class Type> class SharedPtr: public IPointers<Type>
{
       using IPointers<Type>::ptr_;
       using IPointers<Type>::isHeap;
       uint8_t  cr;
      /**< возвращает указатель на ячейку под объект умного указателя в памяти  */

protected:
   void swap( SharedPtr<Type> & shp){

        std::swap(this->ptr_, shp.ptr_);
        std::swap(this->isHeap, shp.isHeap);
        std::swap(this->cr, shp.cr);
     }

    void PlusPointer()
    {
        ++cr;
    }

    int Clean()
    {
        return --cr;
    }

    int getCounter()const{ return cr;}
    void setCounter(int NewCr){ cr = NewCr; }

public:


     explicit SharedPtr(Type *ptr = NULL, bool isHeap = 0)
                                        :IPointers<Type>(ptr, isHeap)
                                        ,cr (1){
            PlusPointer();
     }

  /*  SharedPtr(const SharedPtr<Type>& ptr) {

        this->ptr_ = ptr.ptr_;

        const_cast<SharedPtr<Type>&>(ptr).PlusPointer();

        cr = ptr.getCounter();
        this->isHeap = ptr.isHeap;

    }*/
   ~SharedPtr(){
        if((--cr)==0 && this->isHeap){

            delete this->ptr_;
        }
     }


    bool operator!=(Type* pointer) const {   return this->ptr_ != pointer;}
    bool operator==(Type* pointer) const {   return this->ptr_ == pointer;}
    Type& operator[](uint8_t i){ return ptr_[i];}
    SharedPtr<Type>& operator=(const SharedPtr<Type>& ptr){

        if(this != &ptr)
            SharedPtr(ptr).swap(*this);

        return *this;
    }

   void operator=(Type* pointer) { this->ptr_ = pointer;}

   Type operator*()const{return *(this->ptr_);}


    Type* operator->(){ return this->ptr_;}

     Type* get(size_t i)const{return this->ptr_ + i;}

     void reset(Type *ptr = NULL){
            if(!(--cr) && this->isHeap)
                delete this->ptr_;

            this->ptr_ = ptr;
            ++cr;

     }

     bool heap()const{
        return this->isHeap;
     }

};


   template <class FirstType, class SecondType> bool operator==(FirstType val, IPointers<SecondType>& ptr) {
            return val == *ptr;
    }


template <class Type>
class  IMemoryAllocator{/**< visitor */
    public:

        virtual SharedPtr<Type>&  Alloc(size_t Size) = 0;
        virtual SharedPtr<Type>& ReAlloc(IPointers<Type>& pointer, size_t size) = 0;
        virtual void Free(IPointers<Type>& Pointer) = 0;
        virtual ~IMemoryAllocator(){}
        virtual bool Defragmentation() const = 0;
        virtual int checkMemory() const = 0;
};


template <class Type>
    class PoolAllocator: public IMemoryAllocator<Type> {

private:

     struct Pool{
        Pool():next(0){}
        Pool *next;
    };

    Pool      *PoolHead;
    char      *memory;

    size_t size; // in bytes
    unsigned segments;
    unsigned pointerCounter;

    SharedPtr<Type>* queueHead;

    bool IncreasePointerVector();
    size_t offset();

    PoolAllocator(PoolAllocator<Type>&);
protected:
    size_t bytes(Type *pointer);

public:
PoolAllocator(Type *block, size_t bytes);
// static  PoolAllocator& CreateInstance(Type *block, size_t bytes){
//        static PoolAllocator allocator(block, bytes);
//        return allocator;
//  }



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

        SharedPtr<Type>&  Alloc(size_t Size);

    /*!
    Reallocates the first available units in the pool with new size.
    if memory block gets bigger new memory segments won't be initialized.
    if pointer equals zero behaves like Alloc(size)
    if size == 0 && pointer != 0 behaves like Free(pointer)
    \param[in] pointer memory to be reallocated
    \param[in] size the size of the requested memory
    */

     SharedPtr<Type>& ReAlloc(IPointers<Type>& pointer, size_t size) ;

    /*!
    Frees the units in the pool until it reaches
     another allocated block or unallocated memory.
    \param[in] pointer memory to be freed
    */
/*! @} */
        void Free(IPointers<Type>& Pointer);
        bool Defragmentation() const;
        virtual ~PoolAllocator(){}

    int checkMemory()const{

        int counter = 0;

        Pool *first = PoolHead;

        while(first){

        ++counter;
        first = first->next;

        }
        std::cout<<"counter = "<<counter<<std::endl;
        return (counter)*sizeof(Pool);
    }
};



template<class Type>  PoolAllocator<Type> ::PoolAllocator(Type *block, size_t elements)
                            :memory((char*)block)

                            ,size(elements*sizeof(Type))
                            ,segments(size
                                  /sizeof(Pool))// количество кусков по 8 байт
                            ,pointerCounter(0)
                            ,queueHead( (SharedPtr<Type>*)(memory + size) )
                        {
std::cout<< "PoolAllocator(Type *block, size_t elements) "<<std::endl;
            Pool * PoolSecond = NULL;
            Pool PoolObject;
            for(size_t i = 0 ; i < segments; ++i)
            {
                PoolHead    = (Pool*)memory + i + 1;
                PoolSecond  = (Pool*)memory + i;
                memmove(PoolSecond, &PoolObject, sizeof(PoolObject));
                PoolSecond->next = PoolHead;
            }
            PoolHead = (Pool*)memory;
            PoolSecond->next = NULL;
  std::cout<< "Памяти осталось "<<this->checkMemory()<<" байт. "<<std::endl;
        }

  //template<class Type>  PoolAllocator<Type>::~PoolAllocator(){}

template<class Type> size_t  PoolAllocator<Type>::offset(){

    return 0;

}

  template<class Type>  SharedPtr<Type>&  PoolAllocator<Type>::Alloc(size_t Size) {
    std::cout<< "Alloc() "<<std::endl;
    std::cout<< "Памяти осталось start"<<this->checkMemory()<<" байт. "<<std::endl;
            IncreasePointerVector();

                size_t real_size = 0; // вычислим реальное число блоков с учетом размера каждой структуры в 8 байт
                real_size =  (Size%sizeof(Pool)) ?  Size/sizeof(Pool) + 2 : Size/sizeof(Pool) + 1;

            if( segments > real_size  &&  Size != 0  ){

                segments -= real_size;

                // сделаем еще две ссылки и прогоним в поисках идущих подряд незанятых блоков
                Pool *ptr_first = PoolHead;
                Pool *ptr_second = PoolHead;
             //
                int counter = 0, gen_counter = 0;

                // количество скачков через уже выделенную память
                size_t seg_jump = 0;

                while(counter < real_size && ptr_first) //ищем верхнюю границу памяти
                {
                    if(ptr_first->next - ptr_first == 1)// проверка на наличие выделенных участков между блоками
                        ++counter;

                    else//найден скачек - обнуление счетчика
                        counter = 0;

                    ptr_first = ptr_first->next;
                    ++gen_counter; // далее, считаем общее количество пройденных блоков
                }

                // check fragmentation on the segment have been traversed
                if(counter != gen_counter)
                    seg_jump = 1;



                if(counter == real_size)
                {
    //рассмотрим 2 случая: память сегментирована и - нет.
    // используем 2 указателя на выделяемый отрезок памяти: начало и конец
    //если память не сегментирована, то второй указатель так и остается в начале списка
                    if(seg_jump){// если прыжок был, то нужно вырезать отрезок и объединить список
                            // общее количество пройденных блоков
                            // не равно количеству пройденных подряд не занятых блоков -> память сегментирована
                        ///Назначить PoolHead, проверить корректность.
                        for(int i = 0; i <= gen_counter-counter - 1; ++i)
                            ptr_second = ptr_second->next;//run until the block before the first block
                        queueHead[0] = (Type *)(ptr_second->next);//отдали первый блок
                        ptr_second->next = ptr_first;//copy the one's after last block address to the "next" field.

                    }else{

                        // память пока не сегментирована или
                        //выделяется один блок(может состоять из нескольких юнитов) в самом начале цепочки юнитов*/
                       // (*((double*)ptr_second) = size;
                        queueHead[0] = (Type *)ptr_second; // отдаем указатель на первый блок
                        PoolHead = ptr_first; //назначаем головй списка следующий юнит за выделенным блоком
                    }

                  *((double*)(ptr_first-1)) = size; // записываем в конец выделенного сегмента символ конца строки

                }




          }

          std::cout<< "Памяти осталось end"<<this->checkMemory()<<" байт. "<<std::endl;

             return  queueHead[0];

     }


template<class Type> bool  PoolAllocator<Type>::IncreasePointerVector(){

    SharedPtr<Type> tmp;
    --queueHead;

    memmove(queueHead, &tmp, sizeof(tmp));
    ++pointerCounter;

     Pool* end = ((Pool *) queueHead) - 1;
     end->next = NULL;

     segments -= sizeof(SharedPtr<Type>)/sizeof(Pool);


}


      template<class Type> size_t PoolAllocator<Type>::bytes(Type * Pointer){

            size_t i = 0;

            for( char *ptr = (char*)Pointer;
            ptr && *((double*)ptr) != size;
            ++ptr)
                ++i;

            return i;
        }

    template<class Type>
     SharedPtr<Type>&  PoolAllocator<Type>::ReAlloc(IPointers<Type>& pointer, size_t _size) {

          //  std::cout<<"size = "<<size<<std::endl;
            if(pointer == NULL) //<- будет срабатывать почти всегда, а как же данные?
                return Alloc(_size);
            else
                if(_size == 0 && pointer != NULL){
                    Free(pointer);
                    return *(SharedPtr<Type>*)(memory + size);
                }



                size_t oldSize = bytes(&pointer[0]);

                SharedPtr<Type>& ptr = Alloc(_size);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! проставить касты
                for(size_t i = 0; i < _size && i < oldSize; ++i)
                    ((char*)ptr)[i] = ((char*)pointer)[i];
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                Free(pointer);

                return ptr;

        }

    template<class Type> void PoolAllocator<Type>::Free(IPointers<Type>& Pointer) { //<-коснтантная ссылка?
            // оствобождает всю память от Pointer и до первого незахваченного куска
            std::cout<<"Free()"<<std::endl;
            if(Pointer != NULL){

                Pool* ptr = (Pool*)Pointer;// приводим указатель начало сегмента памяти к типу юнитов
                Pool* ptr_end = NULL;// указатель на предыдущий unit

                Pool* start = PoolHead;
                Pool PoolObject;

                size_t blockSize = bytes(&Pointer[0]) + sizeof(Pool);
                //short beg = 0;//для проверки выделен ли кусок в начале памяти или где-то посередине
            // Pointer = NULL;

                  for(unsigned i = 0 ; i < blockSize/sizeof(Pool); ++i)
                {
                    memmove(ptr, &PoolObject, sizeof(PoolObject));
                    ptr_end = ptr++;
                    ptr_end->next = ptr;
                    std::cout<<"i = "<<i<<std::endl;
                }

                if(PoolHead > (Pool*)Pointer)
                {
                    ptr_end->next = PoolHead;
                    PoolHead = (Pool*)Pointer;
                }else{
                    // если не с первого, то ищем подходящий стартовый блок
                    while(start && start->next < (Pool*)Pointer){///А ЕСЛИ ДО ЭТОГО БЛОКА ВЫДЕЛЕН ЕЩЕ ОДИН БЛОК?
                        start = start->next;// присваиваем start адрес блока идущего перед Pointer
                    }

                    ptr_end->next = start->next;
                    start->next = (Pool*)Pointer;
                }



            }

std::cout<<"-----end---Free()---"<<std::endl;
        }

template<class Type> bool PoolAllocator<Type>::Defragmentation()const {


}

#endif // ALLOCATOR_HPP_INCLUDED
