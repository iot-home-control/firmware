#include "trampoline_magic.h"

ISR*const * tramps=get_trampolines(foo::make_index_sequence<NUM_ISR_TRAMPS>{});

std::function<void()> isr_trampolines[NUM_ISR_TRAMPS];

int allocate_trampoline(const std::function<void()> func)
{
    for(int index=0; index<NUM_ISR_TRAMPS; index++)
    {
        if(!isr_trampolines[index])
        {
            isr_trampolines[index]=func;
            return index;
        }
    }
    return -1;
}
