#ifndef CBPP_BITWORK_H
#define CBPP_BITWORK_H

#include <stdint.h>
#include <stdio.h>

namespace cbpp {
    bool HasZeroBits(uint64_t iNum);

	template <typename int_t> bool TestBitmask(int_t word, int_t bmask){
		return (word & bmask) == bmask;
	}
	
	template <typename int_t> uint8_t GetBit(int_t word, uint16_t index){
		return (word & ((int_t)(1) << index)) >> index;
	}
	
	template <typename int_t> void SetBit(int_t& word, uint16_t index, uint8_t bit){
        word = bit ? (word | ((int_t)(1) << index)) : (word & ~((int_t)(1) << index));
	}

	template <typename int_t> void PrintBits(int_t word) {
		for(uint8_t i = 0; i < sizeof(word)*8; i++) {
			if(i % 8 == 0) {
				putc(' ', stdout);
			}
			printf("%u", GetBit<int_t>(word, 8-i-1) ? 1 : 0 );
		}
	}
}

#endif
