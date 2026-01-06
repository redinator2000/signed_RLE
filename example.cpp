#include <cassert>
#include <cstdio>
#include "signed_RLE.hpp"

#ifdef __linux__
#define SIZE_PF "%ld"
#else //_WIN32
#define SIZE_PF "%lld"
#endif

bool test_compression(const std::vector<int8_t> & some_data)
{
    printf("testing data\n");
    printf("  original   size " SIZE_PF "\n", some_data.size());
    std::vector<int8_t> compressed = signed_RLE::compress(some_data);
    printf("  compressed size " SIZE_PF "\n", compressed.size());
    std::vector<int8_t> uncompressed = signed_RLE::uncompress(compressed);
    //printf("  uncompressed data %d\n", uncompressed.size());
    if(some_data.size() == uncompressed.size())
    {
        for(size_t i = 0; i < some_data.size(); i++)
            if(some_data[i] != uncompressed[i])
            {
                printf("test failed!\n");
                return false;
            }
    }
    else
    {
        printf("test failed!\n");
        return false;
    }
    return true;
}

void example()
{
    test_compression({1});
    test_compression({2, 2});
    test_compression({3, 3, 3});    //signed_RLE slightly increases size for data that doesn't have any 4 bytes repeating
    test_compression({4, 4, 4, 4});
    test_compression({1, 2, 3, 4}); //signed_RLE slightly increases size for data that doesn't have any 4 bytes repeating
    test_compression({1, 2, 2, 3, 3, 3, 4, 4, 4, 4});
    test_compression({1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5});

    std::vector<int8_t> hundred_hundreds(100, 100);
    test_compression(hundred_hundreds);
    hundred_hundreds[52] = 52;
    test_compression(hundred_hundreds);

    std::vector<int8_t> tenthousand_fives(10000, 5);
    test_compression(tenthousand_fives);
    tenthousand_fives[5002] = 6;
    test_compression(tenthousand_fives);

    std::vector<int8_t> counting = {};
    counting.reserve(10000);
    for(int i = 0; i < 10000; i++)
        counting.push_back(i);
    test_compression(counting); //signed_RLE slightly increases size for data that doesn't have any 4 bytes repeating

    // test long segment splitting
    // needs a vector with at least 2,147,483,647 elements (maybe too big for a vector)
    // should allocate at least 3 GB of memory
    // probably not a good idea to run this
    /*
    std::vector<int8_t> threebillion_fives(3000000000, 5);
    test_compression(threebillion_fives);
    threebillion_fives[1000000000] = 6;
    test_compression(threebillion_fives);
    */
}
