#include <iostream>
#include <cstddef>
#include <vector>

#include "Benchmark.h"
#include "Allocator.h"
#include "StackAllocator.h"
#include "CAllocator.h"
#include "LinearAllocator.h"
#include "PoolAllocator.h"
#include "FreeListAllocator.h"

int main() {
    const std::vector<std::size_t> ALLOCATION_SIZES {32, 64, 256, 512, 1024, 2048, 4096};
    const std::vector<std::size_t> ALIGNMENTS {8, 8, 8, 8, 8, 8, 8};

    Allocator * cAllocator = new CAllocator();
    Allocator * linearAllocator = new LinearAllocator(1e9);
    Allocator * stackAllocator = new StackAllocator(1e9);
    Allocator * poolAllocator = new PoolAllocator(16777216, 4096);
    Allocator * freeListAllocator = new FreeListAllocator(1e8, FreeListAllocator::PlacementPolicy::FIND_FIRST);

    Benchmark benchmark(1e1);

    std::cout << "C" << std::endl;
    benchmark.MultipleAllocation(cAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.MultipleFree(cAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomAllocation(cAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomFree(cAllocator, ALLOCATION_SIZES, ALIGNMENTS); 

    std::cout << "LINEAR" << std::endl;
    benchmark.MultipleAllocation(linearAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomAllocation(linearAllocator, ALLOCATION_SIZES, ALIGNMENTS);

    std::cout << "STACK" << std::endl;
    benchmark.MultipleAllocation(stackAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.MultipleFree(stackAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomAllocation(stackAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomFree(stackAllocator, ALLOCATION_SIZES, ALIGNMENTS);

    std::cout << "POOL" << std::endl;
    benchmark.SingleAllocation(poolAllocator, 4096, 8);
    benchmark.SingleFree(poolAllocator, 4096, 8);

    std::cout << "FREE LIST" << std::endl;
    benchmark.MultipleAllocation(freeListAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.MultipleFree(freeListAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomAllocation(freeListAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    benchmark.RandomFree(freeListAllocator, ALLOCATION_SIZES, ALIGNMENTS);
    
    delete cAllocator;
    delete linearAllocator;
    delete stackAllocator;
    delete poolAllocator;
    
    return 1;
}




