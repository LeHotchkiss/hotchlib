#include <string.h>
#include <stdio.h>

#define TEST_CARRAY
//#define TEST_CBINTABLE
//#define TEST_CSTACK
//#define TEST_STATIC_ARRAY
#define TEST_CSTRING

#ifdef TEST_CARRAY
    #include "hlib/Array.h"
#endif

#ifdef TEST_CBINTABLE
    #include "hlib/Table.h"
    #include "hlib/String.h"
#endif

#ifdef TEST_CSTACK
    #include "hlib/Stack.h"
#endif

#ifdef TEST_STATIC_ARRAY
    #include "hlib/StaticArray.h"
#endif

#ifdef TEST_CSTRING
    #include "hlib/String.h"
#endif

#include "hlib/New.h"

class CDum {
    int m_i = 0;

    public:
        CDum() {  }
        CDum(int i) : m_i(i) {  }
        CDum(const CDum& other) : m_i(other.m_i) {  }
        CDum(CDum&& other) {
            m_i = other.m_i;
            memset(&other, 0, sizeof(other));
        }

        CDum& operator=(const CDum& other) {
            m_i = other.m_i;
            return *this;
        }

        CDum& operator=(CDum& other) {
            m_i = other.m_i;
            memset(&other, 0, sizeof(other));
            return *this;
        }

        int GetValue() const { return m_i; }

        ~CDum() {
        }
};

int main(int argc, char** argv) {
    //  CArray

    #ifdef TEST_CARRAY
    {
        hlib::array_t<CDum> testArray;
        for(int i = 0; i < 10; i++) {
            testArray.PushBack(i);
        }
        testArray.Shrink();

        for(int i = 0; i < testArray.Length(); i++) {
            CDum* pTest = testArray.At(i);
            if(pTest == NULL) {
                printf("Dum №%d not found!\n", i);
                continue;
            }

            printf("Dum №%d = %d\n", i, pTest->GetValue());
        }
    }
    #endif

    // CBinTable

    #ifdef TEST_CBINTABLE
    {
        hlib::table_t<hlib::CConstString, CDum> testTable;

        const char* aKeys[] = {
            "hello",
            "world",
            "this is c++"
        };

        for(int i = 0; i < sizeof(aKeys) / sizeof(aKeys[0]); i++) {
            testTable.Insert(aKeys[i], i*10);
        }

        for(int i = 0; i < testTable.Length(); i++) {
            const hlib::CConstString key = testTable.IndexKey(i);
            printf("'%s' = %d\n", key.String(), testTable[key].GetValue());
        }
    }
    #endif

    #ifdef TEST_CSTACK
    {
        hlib::stack_t<int> stackTest;

        for(int i = 0; i < 20; i++) {
            printf("> %d\n", i);
            stackTest.Push(i);
        }

        for(int i = 0; i < 20; i++) {
            printf("< %d\n", stackTest.Head());
            stackTest.Pop();
        }
    }
    #endif

    #ifdef TEST_STATIC_ARRAY
    {
        hlib::static_array_t<int, 25> test(0);
    }
    #endif

    #ifdef TEST_CSTRING
    {
        hlib::string_t test1 = "hello world!";

        test1.PushFormatted(" %s:%d!!!", "addon", 67);

        hlib::string_t test2;
        test2 += "hello ";
        test2 += "world!";

        printf("Test 1:\n\t'%s'\n", test1.String());
        printf("Test 2:\n\t'%s'\n", test2.String());
    }
    #endif

    /*CDum* pTestDum = hlib::New<CDum>(52);
    hlib::Delete(pTestDum);*/
}
