#ifndef HOTCHLIB_API_STORAGE_H
#define HOTCHLIB_API_STORAGE_H

#include <stdint.h>

#include "hlib/Bit.h"
#include "hlib/Array.h"

namespace hlib {
    /*
        An array with occupation awareness - unlike 
        CArray, this one can have holes
    */
    template <typename value_t, typename alloc_t = CDefaultAllocator> class CStorage {
        enum class ENodeState {
            Free = 1,               // free nodes are marked '1' to allow fast checks for free ones with the !=0 operator
            Busy = 0
        };

        constexpr static size_t s_iBlockSize = sizeof(uint64_t)*8*sizeof(value_t);
        constexpr static size_t s_iBlockCapacity = sizeof(uint64_t)*8;

        struct Block {
            char m_aData[ s_iBlockSize ];
            uint64_t m_iMask = UINT64_MAX;

            bool HasFreeSpace() { return m_iMask != 0; }

            int FindFreeIndex() {
                if( !this->HasFreeSpace() ) { return -1; }

                for(int i = 0; i < s_iBlockCapacity; i++) {
                    if( GetBit(m_iMask, i) == (bool)ENodeState::Free ) {
                        return i;
                    }
                }

                return -1;
            }
            
            void SetIndexState(int iIndex, ENodeState iState) {
                SetBit(m_iMask, iIndex, (bool)iState);
            }

            ENodeState GetIndexState(int iIndex) {
                return (ENodeState)(GetBit(m_iMask, iIndex));
            }

            value_t* operator[](int iIndex) {
                return (value_t*)(m_aData + iIndex*sizeof(value_t));
            }

            value_t* At(int iIndex) {
                return (value_t*)(m_aData + iIndex*sizeof(value_t));
            }

            void DestroyIndex(int iIndex) {
                if( this->GetIndexState(iIndex) == ENodeState::Free ) { return; }
                value_t* pValue = this->At(iIndex);

                pValue->~value_t();
                memset(pValue, 0, sizeof(value_t));
            }

            ~Block() {
                for(size_t i = 0; i < s_iBlockCapacity; i++) {
                    if( this->GetIndexState(i) == ENodeState::Busy) {
                        this->At(i)->~value_t();
                    }
                }
            }
        };

        CArray<Block, alloc_t> m_aBlocks;

        public:
            CStorage() = default;

            CStorage(size_t iInitialSize) {
                const size_t iBlocks = iInitialSize / s_iBlockCapacity + 1;

                m_aBlocks.Reserve(iBlocks);

                Block Buff;
                Buff.m_iMask = (uint64_t)(-1);
                memset(Buff.m_aData, 0, sizeof(Buff.m_aData));

                for(size_t i = 0; i < iBlocks; i++) {
                    m_aBlocks.PushBack(Buff);
                }
            }
            
            template <typename... args_t> value_t* Allocate(args_t&&... Args) {
                for(size_t i = 0; i < m_aBlocks.Length(); i++) {
                    Block& Current = m_aBlocks[i];

                    if(Current.HasFreeSpace()) {
                        int iIndex = Current.FindFreeIndex();
                        Current.SetIndexState(iIndex, ENodeState::Busy);

                        new(Current[iIndex]) value_t( std::forward<args_t>(Args)... );

                        return Current[iIndex];

                    } else {
                        continue;
                    }
                }

                // no free space was found, push a new block

                m_aBlocks.PushEmpty();

                Block& Last = m_aBlocks.Last();
                Last.m_iMask = (uint64_t)-1;
                SetBit(Last.m_iMask, 0, (int)ENodeState::Busy);
                memset(Last.m_aData, 0, sizeof(Last.m_aData));

                return Last[0];
            }
            
            void Free(value_t* pData) {
                if( (size_t)pData < (size_t)m_aBlocks.Data() ) { return; }      // pointer lays before the blocks array, definitely wrong one

                size_t iOffset = (char*)(pData) - (char*)(m_aBlocks.Data());
                size_t iBlockIndex = iOffset / sizeof(Block);

                if( iBlockIndex >= m_aBlocks.Length() ) { return; } // outside of the scope

                size_t iBlockOffset = iOffset - (iOffset / sizeof(Block));
                if(iBlockOffset % sizeof(value_t) != 0) { return; }   // points inside a value

                size_t iIndex = iBlockOffset / sizeof(value_t);

                Block* pBlock = &m_aBlocks[iBlockIndex];
                if( pBlock->GetIndexState(iIndex) == ENodeState::Free ) { return; } // the node is free

                value_t* pValue = pBlock->At(iIndex);

                pValue->~value_t();
                pBlock->SetIndexState(iIndex, ENodeState::Free);
            }
            
            void PrintInfo() {
                printf("Storage with %d blocks:\n", m_aBlocks.Length());

                for(size_t i = 0; i < m_aBlocks.Length(); i++) {
                    Block& block = m_aBlocks[i];

                    printf("\tBLOCK %d: [", i);

                    for(size_t j = 0; j < s_iBlockCapacity; j++) {
                        putc((block.GetIndexState(j) == ENodeState::Busy) ? '#' : '-', stdout);
                    }

                    puts("] "); PrintBits( block.m_iMask ); putc('\n', stdout);
                }
            }
    };
}

#endif
