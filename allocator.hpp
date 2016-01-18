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

template <class Type>
class Visitor {
    public:

        virtual  Type* VisitSharedPointer(SharedPtr<Type>  * shared) = 0;
        virtual ~Visitor() { }
};


template <class Type>
 class Pointers{
 public:
    virtual void accept(Visitor<Type> * vis) = 0;

};



template <class Type> class SharedPtr: public Pointers<Type>
{

      Counter      *cr;
      Type         *ptr_;
      bool         isHeap;
protected:
    void swap(SharedPtr & shp){

        Counter * tmp = cr;
        cr = shp.cr;
        shp.cr = tmp;
        Type * tmp_ptr_ = ptr_;
        ptr_ = shp.ptr_;
        shp = tmp_ptr_;

     }

public:

     explicit SharedPtr(Type *ptr = NULL, bool isHeap = 0)
                                        :ptr_(ptr)
                                        ,isHeap(isHeap){

           cr = new Counter();
           cr->Add();

     }

    SharedPtr(const SharedPtr& ptr) {

        ptr_= ptr.get();

        ptr.cr->Add();
        cr = ptr.cr;
        isHeap = ptr.heap();

    }
   ~SharedPtr(){
        if(cr->Clean()==0){
            delete cr;

        if(isHeap)
            delete ptr_;
        }
     }

    SharedPtr& operator=(const SharedPtr& ptr){

        if(this != &ptr)
            SharedPtr(ptr).swap(*this);

        return *this;
    }


   bool operator!=(Type* pointer) const {
            return ptr_ != pointer;
    }

   bool operator==(Type* pointer) const {
            return ptr_ == pointer;
    }

   void operator=(Type* pointer) {
             ptr_ = pointer;
    }
  Type operator*()const{return *ptr_;}

   Type operator[](size_t i)const{return *(ptr_+ i);}

   Type& operator[](size_t i){
       return *(ptr_+ i);
       }

    template<typename T>
    operator T() {


        return (T)ptr_; }


     Type* get()const{
        return ptr_;
     }
     void reset(Type *ptr = NULL){
            if(!cr->Clean())
                delete ptr_;

            ptr_ = ptr;
            cr->Add();

     }
   //  Type& operator*() const{
    //    return *ptr_;
    // }
     Type* operator->(){
        return ptr_;
     }
     bool heap()const{
        return isHeap;
     }
     void accept(Visitor<Type> *vis){ vis->VisitSharedPointer(this);}
};


   template <class FirstType, class SecondType> bool operator==(FirstType val, SharedPtr<SecondType> ptr) {
            return val == *ptr;
    }

template <class Type>
    class SmartAllocator: public Visitor<Type> {

private:

     struct Pool{
        Pool():next(0){}
        Pool *next;
    };

    Pool *PoolHead;
    char *memory;
    size_t size; // in bytes

    unsigned segments;

protected:
    size_t bytes(Type *pointer);

public:
  Type* VisitSharedPointer(SharedPtr<Type> * shared){
       return *shared;
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

        SmartAllocator(Type *block, size_t bytes);

    /*!
    Allocates the first available units in the pool.
    \param[in] size the size of the requested memory
    */

        SharedPtr<Type>  Alloc(size_t Size);

    /*!
    Reallocates the first available units in the pool with new size.
    if memory block gets bigger new memory segments won't be initialized.
    if pointer equals zero behaves like Alloc(size)
    if size == 0 && pointer != 0 behaves like Free(pointer)
    \param[in] pointer memory to be reallocated
    \param[in] size the size of the requested memory
    */

     SharedPtr<Type> ReAlloc(SharedPtr<Type>& pointer, size_t size) ;

    /*!
    Frees the units in the pool until it reaches
     another allocated block or unallocated memory.
    \param[in] pointer memory to be freed
    */
/*! @} */
        void Free(SharedPtr<Type>& Pointer);

        virtual ~SmartAllocator();



};

template<class Type>  SmartAllocator<Type> ::SmartAllocator(Type *block, size_t bytes)
                            :memory((char*)block)
                            ,size(bytes)
                            ,segments(size
                                  /sizeof(Pool))// количество кусков по 8 байт
                        {

            Pool p;// объявляем структуру и инициализируем конструктором по-умолчанию
            *((Pool*)(memory+segments*sizeof(Pool)-sizeof(Pool))) = p;// записываем первую структуру в память  со смещением
            PoolHead = (Pool*)(memory+segments*sizeof(Pool)-sizeof(Pool));// созраняем ссылку на голову списка. пока только 1 элемент

            for(size_t i = segments*sizeof(Pool)-sizeof(Pool)*2; i > 0; i -= sizeof(Pool))
            {
                *((Pool*)(memory+i)) = p;// записываем следующую структуру в память со смещением
                ((Pool*)(memory+i))->next = PoolHead;// передаем ссылку на предыдущий блок
                PoolHead = (Pool*)(memory+i);// сохраняем ссылку на голову списка
            }

        }

  template<class Type>  SmartAllocator<Type>::~SmartAllocator(){}



  template<class Type>  SharedPtr<Type>  SmartAllocator<Type>::Alloc(size_t Size) {


            SharedPtr<Type> ptr;

            if( segments*sizeof(Pool) >= Size  &&  Size != 0  ){


                size_t real_size = 0; // вычислим реальное число блоков с учетом размера каждой структуры в 8 байт
                real_size =  (Size%sizeof(Pool)) ?  Size/sizeof(Pool) + 2 : Size/sizeof(Pool) + 1;

                segments -= real_size;

                //std::cout<<"real_size = "<< real_size<<std::endl;
                // сделаем еще две ссылки и прогоним в поисках идущих подряд незанятых блоков
                Pool *ptr_first = PoolHead;
                Pool *ptr_second = PoolHead;
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

                        for(int i = 0; i <= gen_counter-counter - 1; ++i)
                            ptr_second = ptr_second->next;//run until the block before the first block
                        ptr = (Type *)(ptr_second->next->next);//отдали первый блок
                        ptr_second->next = ptr_first->next;//copy the one's after last block address to the "next" field.

                    }else{

                        // память пока не сегментирована или
                        //выделяется один блок(может состоять из нескольких юнитов) в самом начале цепочки юнитов*/
                       // (*((double*)ptr_second) = size;
                        ptr = (Type *)ptr_second; // отдаем указатель на первый блок
                        PoolHead = ptr_first->next; //назначаем головй списка следующий юнит за выделенным блоком
                    }

                  *((double*)ptr_first) = size; // записываем в конец выделенного сегмента символ конца строки

                }

          }

            return ptr;
     }

      template<class Type> size_t SmartAllocator<Type>::bytes(Type * Pointer){

            size_t i = 0;

            for( char *ptr = (char*)Pointer; *((double*)ptr) != size; ++ptr)
                ++i;

            return i;
        }

    template<class Type>
     SharedPtr<Type>  SmartAllocator<Type>::ReAlloc(SharedPtr<Type>& pointer, size_t size) {
            if(pointer == NULL)
                return Alloc(size);
            else
                if(size == 0 && pointer != NULL)
                    Free(pointer);
            else{

                size_t oldSize = bytes(pointer.get());
                SharedPtr<Type> ptr = Alloc(size);
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                for(size_t i = 0; i < size && i < oldSize; ++i)
                    ((char*)ptr)[i] = ((char*)pointer)[i];
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                Free(pointer);
                return ptr;

            }
        }

     template<class Type> void SmartAllocator<Type>::Free(SharedPtr<Type>& Pointer) {
            // оствобождает всю память от Pointer и до первого незахваченного куска
            if(Pointer!=NULL){
                Pool* ptr = (Pool*)Pointer;// приводим указатель начало сегмента памяти к типу юнитов
                Pool* start = PoolHead;
                //short beg = 0;//для проверки выделен ли кусок в начале памяти или где-то посередине
            // Pointer = NULL;
            /**НЕОБРАБОТАННЫЙ УЧАСТОК*/
                if(start < ptr){// если не с первого, то ищем подходящий блок

                    while(start != 0 && start != ptr - 1){
                        start = start->next;// присваиваем start адрес блока идущего перед Pointer
                    }
            // вычисляем размер выделенного блока
                    size_t blockSize = start->next - ptr;
                    std::cout<<"blockSize = "<< blockSize;
                }

/**НЕОБРАБОТАННЫЙ УЧАСТОК*/
                else{
                // память пока не фрагментирована и все выделенные блоки
                // идут подряд  перед невыделенными юнитами памяти
                    Pool* ptr_end = 0;// указатель на предыдущий unit
                    Pool p;

                   while(*((double*)ptr) != size){

                        *ptr = p;
                        ptr_end = ptr;
                        ++ptr;
                        ptr_end->next = ptr;



                    }

                }
            }

        }



#endif // ALLOCATOR_HPP_INCLUDED
