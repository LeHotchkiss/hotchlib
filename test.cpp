#include <string.h>
#include <stdio.h>

//#define TEST_CARRAY
#define TEST_CBINTABLE

#ifdef TEST_CARRAY
    #include "hlib/Array.h"
#endif

#ifdef TEST_CBINTABLE
    #include "hlib/Table.h"
    #include "hlib/String.h"
#endif

class CDum {
    int m_i = 0;

    public:
        CDum() { puts("Dum is defaulted"); }
        CDum(int i) : m_i(i) { printf("Dum is born as %d\n", i); }
        CDum(const CDum& other) : m_i(other.m_i) { printf("Dum is born as %d from other dum\n", m_i); }
        CDum(CDum&& other) {
            printf("Dum is moved!\n");
            m_i = other.m_i;
            memset(&other, 0, sizeof(other));
        }

        CDum& operator=(const CDum& other) {
            m_i = other.m_i;
            printf("Dum is copied to %d\n\n", m_i);
            return *this;
        }

        CDum& operator=(CDum& other) {
            m_i = other.m_i;
            memset(&other, 0, sizeof(other));
            printf("Dum is moved! to %d\n\n", m_i);
            return *this;
        }

        int GetValue() const { return m_i; }

        ~CDum() {
            puts("Dum destruction!");
        }
};

int main(int argc, char** argv) {
    //  CArray

    #ifdef TEST_CARRAY
    {
        hlib::CArray<CDum> testArray;
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
        hlib::CBinTable<hlib::CConstString, CDum> testTable;

        const char* aKeys[] = {
            "hello",
            "world",
            "this is c++"
        };

        for(int i = 0; i < sizeof(aKeys) / sizeof(aKeys[0]); i++) {
            testTable.Insert(aKeys[i], i*10);
        }
    }
    #endif
}
