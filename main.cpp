#include <stdio.h>
#include <sys/time.h>
#include "UnitTest++.h"
//#include "test.hpp"
#include <vector>


#include "UnitTest++.h"
#include <ctime>
#include "smart_allocator.hpp"
#include <cstddef>


using namespace std;
SUITE(Smart_allocator_tests)
{

static const int SIZE = 256;

TEST(Alloc_2_mems_float){

static float A[SIZE];


SmartAllocator<float, float>A2(A, SIZE);


    IPointers<float>& A2_P1 = A2.alloc(10);


    for(size_t  i = 0; i < 10; i++)
          A2_P1[i] = (float) i+1;

    IPointers<float>& A2_P2 = A2.alloc(10);
    for(size_t  i = 0; i < 10; i++)
          A2_P2[i] = i*2+1;

    for(size_t  i = 0; i < 10; i++)
        CHECK_EQUAL(i+1, A2_P1[i]);

    for(size_t  i = 0; i < 10; i++)
        CHECK_EQUAL(i*2+1, A2_P2[i]);
    A2.free(A2_P1);
//    CHECK_EQUAL(SIZE*sizeof(float) - 10 * sizeof(float) - 8 - 2*sizeof(IPointers<float>), A2.checkMemory());
    for(size_t  i = 0; i < 10; i++)
        CHECK_EQUAL(i*2+1, A2_P2[i]);

    A2.free(A2_P2);

}



TEST(Alloc_2_mems_int)
    {

    int A[SIZE];
  SmartAllocator<int, int> A2(A, SIZE);
  IPointers<int>& A2_P1 = A2.alloc(10 );

    for(size_t  i = 0; i < 10; ++i)
          A2_P1[i] = i;

   IPointers<int>& A2_P2 = A2.alloc(10 );

    for(size_t  i = 0; i < 10; ++i)
          A2_P2[i] = i*2;

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i,   A2_P1[i]);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);

     A2.realloc(A2_P1, 20 * sizeof(int));

    for(size_t  i = 0; i < 20; ++i)
        A2_P1[i] = i;

        for(int i = 0; i < 20; ++i)
        CHECK_EQUAL(i,   A2_P1[i]);

    }

         TEST(Alloc_2_mems_char)
    {
    char A[SIZE];
    SmartAllocator<char, char> A2(A, SIZE);
    IPointers<char>& A2_P1 = A2.alloc(10 );
    for(size_t  i = 0; i < 10; i++)
          A2_P1[i] = i;

    IPointers<char>& A2_P2 = A2.alloc(10 );

    for(size_t  i = 0; i < 10; i++)
          A2_P2[i] = i*2;
    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i, A2_P1[i]);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);

 A2.free(A2_P1);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);

A2.free(A2_P2);

    }

     TEST(nullptr_size)
    {

     int B[SIZE];
    SmartAllocator<int, int> A2(B, SIZE);
     IPointers<int>&  A2_P1 = A2.alloc(0);
     CHECK_EQUAL(NULL, reinterpret_cast<unsigned long>(&A2_P1[0]));
     A2.free(A2_P1);
    }


    TEST(Official)
    {

         int A[SIZE];
    SmartAllocator<int, int> A1(A, SIZE);
    IPointers<int>&  A1_P1 = A1.alloc(4);
    A1.realloc(A1_P1, 2 );
    A1.free(A1_P1);
    SmartAllocator<int, int> A2(A, SIZE);
    IPointers<int>&  A2_P1 = A2.alloc(10);

   for(int  i = 0; i < 10; i++)   A2_P1[i] = i;
     for(int  i = 0; i < 10; i++)
       if(  A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;

    IPointers<int> & A2_P2 = A2.alloc(10);

    for(int  i = 0; i < 10; i++)
        A2_P2[i] = -1;


    for(int  i = 0; i < 10; i++)
        if(  A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
    for(int  i = 0; i < 10; i++)
        if(  A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;

    /**-------1-------*/
    A2.realloc(A2_P1, 20);

    for(size_t  i = 10; i < 20; i++)
     A2_P1[i] = i;

    for(size_t  i = 0; i < 20; i++) CHECK_EQUAL(i, A2_P1[i]);
/**-------1-------*/

    for(size_t  i = 0; i < 10; i++)
        if(  A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;
/**-------2-------*/
    A2.realloc(A2_P1, 5 );

    for(size_t  i = 0; i < 5; i++)
        CHECK_EQUAL(i, A2_P1[i]);

/**-------2-------*/

    for(size_t  i = 0; i < 10; i++) if(  A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;
    A2.free(A2_P1);
    A2.free(A2_P2);
   }


TEST(Official_plus__alloc)
    {

   int *A = new int[SIZE];
    SmartAllocator<int, int> A1(A, SIZE, true);
    IPointers<int>& A1_P1 = A1.alloc(1);
     A1.realloc(A1_P1, 2 );
//    A1.checkMemory();
    IPointers<int>& A1_P2 = A1.alloc(1);
  //  A1.checkMemory();
    A1.free(A1_P1);
 //   A1.checkMemory();

    IPointers<int>& A1_P3 = A1.alloc(1);
    A1.free(A1_P3);
    A1.free(A1_P2);

    SmartAllocator<int, int> A2(A, SIZE);
    IPointers<int>&  A2_P1 = A2.alloc(10);

   for(int  i = 0; i < 10; i++)
          A2_P1[i] = i;

     for(int  i = 0; i < 10; i++)
       if(  A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;

    IPointers<int>& A2_P2 = A2.alloc(10);

    for(int  i = 0; i < 10; i++)
          A2_P2[i] = -1;

    for(int  i = 0; i < 10; i++)
        if(  A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
    for(int  i = 0; i < 10; i++)
        if(  A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;

    /**-------1-------*/
     A2.realloc(A2_P1, 20 );

    for(int  i = 10; i < 20; i++)
          A2_P1[i] = i;

    for(int  i = 0; i < 20; i++) CHECK_EQUAL(i, A2_P1[i]);
    /**-------1-------*/

    for(int  i = 0; i < 10; i++)
        if(  A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;


    /**-------2-------*/
     A2.realloc(A2_P1, 5);

    for(int  i = 0; i < 5; i++)
        CHECK_EQUAL(i, A2_P1[i]);

    /**-------2-------*/

    for(int  i = 0; i < 10; i++) if(  A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;

    A2.free(A2_P1);
    A2.free(A2_P2);
   // delete[] A;
    }



    TEST(Defragmentation){

           int NumberOfPointers = 14;

           int *A = new int[SIZE];
            SmartAllocator<int, int> A1(A, SIZE);
           IPointers<int>* Ps[NumberOfPointers];

           for (int i = 0; i < NumberOfPointers; ++i){
                Ps[i] = &A1.alloc(6);
              //  A1.checkMemory();
           }

           for(int j = 0; j < NumberOfPointers; ++j)
             if(j%2){
                A1.free(*Ps[j]);
              //  A1.checkMemory();
            }
        try{
            Ps[1] = &A1.alloc( 60);
        }catch(...)
        {
            std::cout<<"Bad alloc is here!"<<std::endl;
        }
        for(int j = 0; j < 5; ++j)
         (*Ps[1])[j] = j;

     for(int j = 0; j < 5; ++j)
        CHECK_EQUAL(j,(*Ps[1])[j]);
         delete [] A;
    }


TEST(execution_time)
{
std::cout<<"ПРИВЕТ!"<<std::endl;

    struct timeval t1, t2, t3;
    gettimeofday(&t1, nullptr);

    for(unsigned i = 0; i < 400; ++i)
    {
std::vector<int> v;
        v.push_back(0);
        v.erase(v.begin());

    }




    gettimeofday(&t2, nullptr);
    timersub(&t2, &t1, &t3);
    printf("=== %ld.%06ld\n",(long int)t3.tv_sec
           , (long int)t3.tv_usec);


struct timeval t1_1, t2_2, t3_3;

int A [4000];
SmartAllocator<int, int> AL(A, 4000);
gettimeofday(&t1_1, nullptr);


    for(unsigned i = 0; i < 400; ++i)
    {
        IPointers<int>& x_1 = AL.alloc(1);
            x_1[0] = 0;
            AL.realloc(x_1, 2);
            AL.free(x_1);

    }

    gettimeofday(&t2_2, nullptr);
    timersub(&t2_2, &t1_1, &t3_3);
    printf("=== %ld.%06ld\n",(long int)t3_3.tv_sec
           , (long int)t3_3.tv_usec);
}




}

SUITE(Allocator_tests)
{

static const int SIZE = 256;

TEST(Alloc_2_mems_float){

static float A[SIZE];

PoolAllocator<float, float>A2(A, SIZE);


    IPointers<float>& A2_P1 = A2.alloc(10 * sizeof(float));


    for(size_t  i = 0; i < 10; i++)
          A2_P1[i] = (float) i+1;

    IPointers<float>& A2_P2 = A2.alloc(10 * sizeof(float));
    for(size_t  i = 0; i < 10; i++)
          A2_P2[i] = i*2+1;

    for(size_t  i = 0; i < 10; i++)
        CHECK_EQUAL(i+1, A2_P1[i]);

    for(size_t  i = 0; i < 10; i++)
        CHECK_EQUAL(i*2+1, A2_P2[i]);
//CHECK_EQUAL(SIZE*sizeof(float) - 2*10 * sizeof(float) - 2*sizeof(ExtendedPointer<float>), A2.checkMemory());
    A2.free(A2_P1);
//    CHECK_EQUAL(SIZE*sizeof(float) - 10 * sizeof(float) - 2*sizeof(ExtendedPointer<float>), A2.checkMemory());
    for(size_t  i = 0; i < 10; i++)
        CHECK_EQUAL(i*2+1, A2_P2[i]);

    A2.free(A2_P2);

//    CHECK_EQUAL(SIZE*sizeof(float)- 2*sizeof(IPointers<float>), A2.checkMemory());
}



TEST(Alloc_2_mems_int)
    {

    int A[SIZE];
  PoolAllocator<int, int> A2(A, SIZE);
  IPointers<int>& A2_P1 = A2.alloc(10 * sizeof(int));

    for(size_t  i = 0; i < 10; ++i)
          A2_P1[i] = i;
//    CHECK_EQUAL(SIZE*sizeof(float)-10 * sizeof(int)- sizeof(ExtendedPointer<float>), A2.checkMemory());
   IPointers<int>& A2_P2 = A2.alloc(10 * sizeof(int));

    for(size_t  i = 0; i < 10; ++i)
          A2_P2[i] = i*2;

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i,   A2_P1[i]);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);

     A2.realloc(A2_P1, 20 * sizeof(int));

    for(size_t  i = 0; i < 20; ++i)
        A2_P1[i] = i;

        for(int i = 0; i < 20; ++i)
        CHECK_EQUAL(i,   A2_P1[i]);

    }

         TEST(Alloc_2_mems_char)
    {
    char A[SIZE];
    PoolAllocator<char, char> A2(A, SIZE);
    IPointers<char>& A2_P1 = A2.alloc(10 * sizeof(char));
    for(size_t  i = 0; i < 10; i++)
          A2_P1[i] = i;

    IPointers<char>& A2_P2 = A2.alloc(10 * sizeof(char));

    for(size_t  i = 0; i < 10; i++)
          A2_P2[i] = i*2;
    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i, A2_P1[i]);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);

 A2.free(A2_P1);

    for(int i = 0; i < 10; ++i)
        CHECK_EQUAL(i*2, A2_P2[i]);

A2.free(A2_P2);

    }

     TEST(nullptr_size)
    {

     int B[SIZE];
    PoolAllocator<int, int> A2(B, SIZE);
     IPointers<int>&  A2_P1 = A2.alloc(0);
     CHECK_EQUAL(NULL, reinterpret_cast<unsigned long>(&A2_P1[0]));
     A2.free(A2_P1);
    }


    TEST(Official)
    {

         int A[SIZE];
    PoolAllocator<int, int> A1(A, SIZE);
    IPointers<int>&  A1_P1 = A1.alloc(sizeof(int));
    A1.realloc(A1_P1, 2 * sizeof(int));
    A1.free(A1_P1);
    PoolAllocator<int, int> A2(A, SIZE);
    IPointers<int>&  A2_P1 = A2.alloc(10 * sizeof(int));

   for(int  i = 0; i < 10; i++)   A2_P1[i] = i;
     for(int  i = 0; i < 10; i++)
       if(  A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;

    IPointers<int> & A2_P2 = A2.alloc(10 * sizeof(int));

    for(int  i = 0; i < 10; i++)
        A2_P2[i] = -1;

    for(int  i = 0; i < 10; i++)
        if(  A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
    for(int  i = 0; i < 10; i++)
        if(  A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;

    /**-------1-------*/
    A2.realloc(A2_P1, 20 * sizeof(int));

    for(size_t  i = 10; i < 20; i++)
     A2_P1[i] = i;

    for(size_t  i = 0; i < 20; i++) CHECK_EQUAL(i, A2_P1[i]);
/**-------1-------*/

    for(size_t  i = 0; i < 10; i++)
        if(  A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;
/**-------2-------*/
    A2.realloc(A2_P1, 5 * sizeof(int));

    for(size_t  i = 0; i < 5; i++)
        CHECK_EQUAL(i, A2_P1[i]);

/**-------2-------*/

    for(size_t  i = 0; i < 10; i++) if(  A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;
    A2.free(A2_P1);
    A2.free(A2_P2);
    }


TEST(Official_plus__alloc)
    {

   int *A = new int[SIZE];
    PoolAllocator<int, int> A1(A, SIZE, true);
    IPointers<int>& A1_P1 = A1.alloc(sizeof(int));
     A1.realloc(A1_P1, 2 * sizeof(int));
//    A1.checkMemory();
    IPointers<int>& A1_P2 = A1.alloc(sizeof(int));
//    A1.checkMemory();
    A1.free(A1_P1);
//    A1.checkMemory();

    IPointers<int>& A1_P3 = A1.alloc(sizeof(int));
    A1.free(A1_P3);
    A1.free(A1_P2);

    PoolAllocator<int, int> A2(A, SIZE);
    IPointers<int>&  A2_P1 = A2.alloc(10 * sizeof(int));

   for(int  i = 0; i < 10; i++)
          A2_P1[i] = i;

     for(int  i = 0; i < 10; i++)
       if(  A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;

    IPointers<int>& A2_P2 = A2.alloc(10 * sizeof(int));

    for(int  i = 0; i < 10; i++)
          A2_P2[i] = -1;

    for(int  i = 0; i < 10; i++)
        if(  A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
    for(int  i = 0; i < 10; i++)
        if(  A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;

    /**-------1-------*/
     A2.realloc(A2_P1, 20 * sizeof(int));

    for(int  i = 10; i < 20; i++)
          A2_P1[i] = i;

    for(int  i = 0; i < 20; i++) CHECK_EQUAL(i, A2_P1[i]);
    /**-------1-------*/

    for(int  i = 0; i < 10; i++)
        if(  A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;


    /**-------2-------*/
     A2.realloc(A2_P1, 5 * sizeof(int));

    for(int  i = 0; i < 5; i++)
        CHECK_EQUAL(i, A2_P1[i]);

    /**-------2-------*/

    for(int  i = 0; i < 10; i++) if(  A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;

    A2.free(A2_P1);
    A2.free(A2_P2);
    //delete[] A;
    }



    TEST(Defragmentation){
std::cout<<"DEFRAGMENTATION TEST"<<std::endl;
           int NumberOfPointers = 17;

           int *A = new int[SIZE];
            PoolAllocator<int, int> A1(A, SIZE);
           IPointers<int>* Ps[NumberOfPointers];

           for (int i = 0; i < NumberOfPointers; ++i){
                Ps[i] = &A1.alloc(sizeof(int)*5);
//                A1.checkMemory();
           }

           for(int j = 0; j < NumberOfPointers; ++j)
             if(j%2){
                A1.free(*Ps[j]);
//                A1.checkMemory();
            }

        Ps[1] = &A1.alloc( sizeof(int)*45);
        for(int j = 0; j < 5; ++j)
         (*Ps[1])[j] = j;


     for(int j = 0; j < 5; ++j)
        CHECK_EQUAL(j,(*Ps[1])[j]);
     A1.free(*Ps[1]);
         delete [] A;
    }

TEST(execution_time_POOL)
{
std::cout<<"ПРИВЕТ!"<<std::endl;

    struct timeval t1, t2, t3;
    gettimeofday(&t1, nullptr);

    for(unsigned i = 0; i < 300; ++i)
    {
std::vector<int> v;
        v.push_back(0);
        v.erase(v.begin());

    }




    gettimeofday(&t2, nullptr);
    timersub(&t2, &t1, &t3);
    printf("=== %ld.%06ld\n",(long int)t3.tv_sec
           , (long int)t3.tv_usec);


struct timeval t1_1, t2_2, t3_3;

int A [4000];
PoolAllocator<int, int> AL(A, 4000);
gettimeofday(&t1_1, nullptr);


    for(unsigned i = 0; i < 300; ++i)
    {
        IPointers<int>& x_1 = AL.alloc(sizeof(int));
            x_1[0] = 0;
            AL.realloc(x_1, 2*sizeof(int));
            AL.free(x_1);

    }

    gettimeofday(&t2_2, nullptr);
    timersub(&t2_2, &t1_1, &t3_3);
    printf("=== %ld.%06ld\n",(long int)t3_3.tv_sec
           , (long int)t3_3.tv_usec);
}


}

template <typename T> void print(IPointers<T>& p){
    for(int i = 0; i < p.size(); ++i)
        std::cout<<p[i];
    std::cout<<std::endl;
}
int main()
{
    static int Arr[200];
    //It works even if initial memory type is different from desired.
    //So, it's not need to manually cast anything.
    SmartAllocator<char, int> At(Arr, 200);
    IPointers<char>& p = At.alloc(4);

    for(int i = 0 ; i < 4; ++i)
        p[i] = 'A' + i;

    print(p);//the first output

    At.realloc(p, 20);
    for(int i = 4 ; i < 20; ++i)
        p[i] = 'A' + i;

    print(p); //the second output

    At.free(p);
    //return 0;
   return UnitTest::RunAllTests();
}
