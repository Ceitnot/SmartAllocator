#include <iostream>
#include "UnitTest++.h"
using namespace std;

#include "allocator.hpp"
SUITE(Allocator_tests)
{
    static size_t SIZE = 700;

TEST(Alloc_2_mems_float){
 float A[SIZE];
    IMemoryAllocator<float>& A2 = PoolAllocator<float>::CreateInstance(A, SIZE);
    SharedPtr<float> A2_P1 ( A2.Alloc(10 * sizeof(float)));


    for(size_t  i = 0; i < 10; i++)
        A2_P1[i] = i;


    SharedPtr<float> A2_P2 ( A2.Alloc(10 * sizeof(float)));

    for(size_t  i = 0; i < 10; i++)
        A2_P2[i] = i*2;

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i, A2_P1[i]);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);


 A2.Free(A2_P1);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);

A2.Free(A2_P2);

}



TEST(Alloc_2_mems_int)
    {
int A[SIZE];
  IMemoryAllocator<int>& A2 = PoolAllocator<int>::CreateInstance(A, SIZE);

  SharedPtr<int> A2_P1 = A2.Alloc(10 * sizeof(int));

    for(size_t  i = 0; i < 10; i++)
        A2_P1[i] = i;


   SharedPtr<int> A2_P2 (A2.Alloc(10 * sizeof(int)));

    for(size_t  i = 0; i < 10; i++)
        A2_P2[i] = i*2;



    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i, A2_P1[i]);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);


   A2.Free(A2_P2);

    A2.ReAlloc(A2_P1, 20 * sizeof(int));

    for(size_t  i = 0; i < 20; i++)
        A2_P1[i] = i;

        for(int i = 0; i < 20; ++i)
        CHECK_EQUAL(i, A2_P1[i]);
//std::cout<<"COMPLETED"<<"Alloc_2_mems_int"<<std::endl;
    }

         TEST(Alloc_2_mems_char)
    {
    char A[SIZE];
    IMemoryAllocator<char>& A2 = PoolAllocator<char>::CreateInstance(A, SIZE);
    SharedPtr<char> A2_P1 ( A2.Alloc(10 * sizeof(char)));


    for(size_t  i = 0; i < 10; i++)
        A2_P1[i] = i;


    SharedPtr<char> A2_P2 (A2.Alloc(10 * sizeof(char)));

    for(size_t  i = 0; i < 10; i++)
        A2_P2[i] = i*2;



    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i, A2_P1[i]);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);


 A2.Free(A2_P1);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);

A2.Free(A2_P2);
//std::cout<<"COMPLETED :"<<"Alloc_2_mems_char"<<std::endl;
    }





     TEST(null_size)
    {
    int A[SIZE];
    IMemoryAllocator<int>& A2 = PoolAllocator<int>::CreateInstance(A, SIZE);
    SharedPtr<int>  A2_P1 ( A2.Alloc(0));
  //  std::cout<<"POINTER = "<<&A2_P1[0]<<std::endl;
           CHECK_EQUAL(NULL, (unsigned long int)(&A2_P1[0]));



    A2.Free(A2_P1);

    // std::cout<<"COMPLETED"<<"null_size"<<std::endl;
    }


    TEST(Official)


    {
         int A[SIZE];
   IMemoryAllocator<int>& A1 = PoolAllocator<int>::CreateInstance(A, SIZE);

    SharedPtr<int>  A1_P1 ( A1.Alloc(sizeof(int)));
    A1_P1 =  A1.ReAlloc(A1_P1, 2 * sizeof(int));
    A1.Free(A1_P1);
    IMemoryAllocator<int>& A2 = PoolAllocator<int>::CreateInstance(A, SIZE);
    SharedPtr<int>  A2_P1 (A2.Alloc(10 * sizeof(int)));
   for(int  i = 0; i < 10; i++) A2_P1[i] = i;
     for(int  i = 0; i < 10; i++)
       if(A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;

SharedPtr<int>  A2_P2 ( A2.Alloc(10 * sizeof(int)));

for(int  i = 0; i < 10; i++)
    A2_P2[i] = -1;

for(int  i = 0; i < 10; i++)
    if(A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
for(int  i = 0; i < 10; i++)
    if(A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;

/**-------1-------*/
A2_P1 =  A2.ReAlloc(A2_P1, 20 * sizeof(int));

for(size_t  i = 10; i < 20; i++)
    A2_P1[i] = i;

for(size_t  i = 0; i < 20; i++) CHECK_EQUAL(i, A2_P1[i]);
/**-------1-------*/

for(size_t  i = 0; i < 10; i++)
    if(A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;


/**-------2-------*/
A2_P1 =  A2.ReAlloc(A2_P1, 5 * sizeof(int));

for(size_t  i = 0; i < 5; i++)
    CHECK_EQUAL(i, A2_P1[i]);

/**-------2-------*/

for(size_t  i = 0; i < 10; i++) if(A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;
A2.Free(A2_P1);
A2.Free(A2_P2);

 //std::cout<<"COMPLETED"<<"official"<<std::endl;
    }


TEST(Official_plus_heap_alloc)
    {
   int *A = new int[SIZE];
   IMemoryAllocator<int> & A1 = PoolAllocator<int>::CreateInstance(A, SIZE);

    SharedPtr<int>  A1_P1 ( A1.Alloc(sizeof(int)));
    A1_P1 =  A1.ReAlloc(A1_P1, 2 * sizeof(int));
    A1.Free(A1_P1);
    PoolAllocator<int> & A2 = PoolAllocator<int>::CreateInstance(A, SIZE);
    SharedPtr<int>  A2_P1 ( A2.Alloc(10 * sizeof(int)));

   for(int  i = 0; i < 10; i++)
        A2_P1[i] = i;

     for(int  i = 0; i < 10; i++)
       if(A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;

    SharedPtr<int>  A2_P2 ( A2.Alloc(10 * sizeof(int)));

    for(int  i = 0; i < 10; i++)
        A2_P2[i] = -1;

    for(int  i = 0; i < 10; i++)
        if(A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
    for(int  i = 0; i < 10; i++)
        if(A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;

    /**-------1-------*/
    A2_P1 =  A2.ReAlloc(A2_P1, 20 * sizeof(int));

    for(int  i = 10; i < 20; i++)
        A2_P1[i] = i;

    for(int  i = 0; i < 20; i++) CHECK_EQUAL(i, A2_P1[i]);
    /**-------1-------*/

    for(int  i = 0; i < 10; i++)
        if(A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;


    /**-------2-------*/
    A2_P1 =  A2.ReAlloc(A2_P1, 5 * sizeof(int));

    for(int  i = 0; i < 5; i++)
        CHECK_EQUAL(i, A2_P1[i]);

    /**-------2-------*/

    for(int  i = 0; i < 10; i++) if(A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;

    A2.Free(A2_P1);
    A2.Free(A2_P2);
    delete[] A;
 //std::cout<<"COMPLETED"<<"official"<<std::endl;
    }

    TEST(Defragmentation){
           int NumberOfPointers = 20;
           int *A = new int[SIZE];
           IMemoryAllocator<int> & A1 = PoolAllocator<int>::CreateInstance(A, SIZE);
           SharedPtr<int> Ps[NumberOfPointers];
           for (int i = 0; i < NumberOfPointers; ++i)
                Ps[i] = A1.Alloc(SIZE/NumberOfPointers);

           for(int j = 0; j < NumberOfPointers; ++j)
            if(j%2){
                A1.Free(Ps[j]);
                NumberOfPointers -=j;
            }
        A1.Alloc( (SIZE) );
        std::cout<<"Получилось"<<std::endl;
    }

}



int main()
{


   // int A[848576];
   // PoolAllocator<int> & A2 = PoolAllocator<int>::CreateInstance(A, 848576);
   // SharedPtr<int>  A2_P1 (A2.Alloc(0));
  //  std::cout<<"POINTER = "<<&A2_P1[0]<<std::endl;
        //   CHECK_EQUAL(NULL, (unsigned long int)(&A2_P1[0]));



  //  A2.Free(A2_P1);
    return //0;
    UnitTest::RunAllTests();
}
