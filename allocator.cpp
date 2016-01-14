
/** \file
 *  \brief Definition of SmartAllocator methods
 */
#include "allocator.hpp"



    SmartAllocator::SmartAllocator(void *block, size_t bytes)
                            :memory((char *)block)
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

        SmartAllocator::~SmartAllocator(){}


        void *SmartAllocator::Alloc(size_t Size) {


            void *ptr = 0;

            if( segments*sizeof(Pool) >= Size  &&  Size != 0  ){


                size_t real_size = 0; // вычислим реальное число блоков с учетом размера каждой структуры в 8 байт
                real_size =  (Size%sizeof(Pool)) ?  Size/sizeof(Pool) + 2 : Size/sizeof(Pool) + 1;

                segments -= real_size;


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
                    if(seg_jump){// если прыжок был, то нужно вырезать отрезок и обединить список
                            // общее количество пройденных блоков
                            // не равно количеству пройденных подряд не занятых блоков -> память сегментирована

                        for(int i = 0; i <= gen_counter-counter - 1; ++i)
                            ptr_second = ptr_second->next;

                        ptr = (void *)(ptr_second->next);
                        ptr_second->next = ptr_first->next;

                    }else{

                        // память пока не сегментирована или
                        //выделяется один блок(может состоять из нескольких юнитов) в самом начале цепочки юнитов*/
                        ptr = ptr_second; // отдаем указатель на первый блок
                        PoolHead = ptr_first->next; //назначаем головй списка следующий юнит за выделенным блоком
                    }

                  *((double*)ptr_first) = size; // записываем в конец выделенного сегмента символ конца строки

                }

          }
        //  SharedPtr<void> pt(ptr);
            return ptr;
     }

        size_t SmartAllocator::bytes(void * Pointer){

            size_t i = 0;

            for( char *ptr = (char*)Pointer; *((double*)ptr) != size; ++ptr)
                ++i;

            return i;
        }

        void *SmartAllocator::ReAlloc(void *pointer, size_t size) {
            if(pointer == NULL)
                return Alloc(size);
            else
                if(size == 0 && pointer != 0)
                    Free(pointer);
            else{

                size_t oldSize = bytes(pointer);
                void *ptr = Alloc(size);

                for(size_t i = 0; i < size && i < oldSize; ++i)
                    ((char*)ptr)[i] = ((char*)pointer)[i];

                Free(pointer);
                return ptr;

            }
        }
        void SmartAllocator::Free(void *Pointer) {
            // оствобождает всю память от Pointer и до первого незахваченного куска
            Pool* ptr = (Pool*)Pointer;// приводим указатель начало сегмента памяти к типу юнитов
            Pool* start = PoolHead;
            short beg = 0;//для проверки выделен ли кусок в начале памяти или где-то посередине
            if(start < ptr){// если не с первого, то ищем подходящий блок

                while(start != 0 && start != ptr - 1){
                    start = start->next;// присваиваем start адрес блока идущего перед Pointer
                }
            // вычисляем размер выделенного блока
            size_t blockSize = start->next - ptr;

            }
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
