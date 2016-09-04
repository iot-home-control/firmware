#pragma once
#include <functional>
#include <Arduino.h>

// See http://stackoverflow.com/a/32223343
namespace foo
{
    template <size_t... Ints>
    struct index_sequence
    {
        using type = index_sequence;
        using value_type = size_t;
        static constexpr std::size_t size() noexcept { return sizeof...(Ints); }
    };

    // --------------------------------------------------------------

    template <class Sequence1, class Sequence2>
    struct _merge_and_renumber;

    template <size_t... I1, size_t... I2>
    struct _merge_and_renumber<index_sequence<I1...>, index_sequence<I2...>>
      : index_sequence<I1..., (sizeof...(I1)+I2)...>
    { };

    // --------------------------------------------------------------

    template <size_t N>
    struct make_index_sequence
      : _merge_and_renumber<typename make_index_sequence<N/2>::type,
                            typename make_index_sequence<N - N/2>::type>
    { };

    template<> struct make_index_sequence<0> : index_sequence<> { };
    template<> struct make_index_sequence<1> : index_sequence<0> { };
}

#define NUM_ISR_TRAMPS 20
#define ARRAY_COUNT(arr) (sizeof((arr))/sizeof((arr)[0]))
extern std::function<void()> isr_trampolines[NUM_ISR_TRAMPS];

template<int N>
void isr_tramp()
{
    static_assert(N<ARRAY_COUNT(isr_trampolines),"Too many trampolines");
    isr_trampolines[N]();
}

using ISR = void();

template<size_t... N>
ISR*const * get_trampolines(foo::index_sequence<N...>)
{
    constexpr static ISR* tramps[sizeof...(N)] = {(&isr_tramp<N>)...};
    return tramps;
}

extern ISR*const * tramps;

int allocate_trampoline(const std::function<void()> func);
