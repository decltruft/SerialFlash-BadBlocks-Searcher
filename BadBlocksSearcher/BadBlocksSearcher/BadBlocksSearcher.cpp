#include "BBlocksSearcher.h"


int main()
{
    uint32_t start_memory_address = 0x8000000;
    uint32_t end_memory_address = 0x8200000;

    SerialFlashHandler flash_handler(start_memory_address, end_memory_address);
    BBlocksSearcher bad_block_searcher(&flash_handler);
    bad_block_searcher.checking_flash_pull();

    return 0;
}

