
#pragma once


#include <cassert>
#include <iterator>
#include <utility>
#include <iostream>
#include <functional>


#ifndef DEBUG_PRINT_ALGORITHMS
#define DEBUG_PRINT_ALGORITHMS false
#endif

namespace my {

    template< typename InputIterT, typename UnaryPredicateT >
    inline bool all_of(InputIterT first, InputIterT last, UnaryPredicateT unaryPredicate)
    {
        for (; first != last; ++first)
        {
            if (!unaryPredicate(*first))
                return false;
        }
        return true;
    }

    template< typename InputIterT, typename UnaryPredicateT >
    inline bool any_of(InputIterT first, InputIterT last, UnaryPredicateT unaryPredicate)
    {
        for (; first != last; ++first)
        {
            if (unaryPredicate(*first))
                return true;
        }
        return false;
    }

    template< typename InputIterT, typename UnaryPredicateT >
    inline bool none_of(InputIterT first, InputIterT last, UnaryPredicateT unaryPredicate)
    {
        for (; first != last; ++first)
        {
            if (unaryPredicate(*first))
                return false;
        }
        return true;
    }

    template< typename InputIterT, typename UnaryFunctionT >
    inline UnaryFunctionT for_each(InputIterT first, InputIterT last, UnaryFunctionT unaryFunction)
    {
        for (; first != last; ++first)
            unaryFunction(*first);
        return std::move(unaryFunction);
    }

    template< typename InputIterT, typename Size, typename UnaryFunctionT >
    inline InputIterT for_each_n(InputIterT first, Size count, UnaryFunctionT unaryFunction)
    {
        for (Size i = 0; i != count; ++i, ++first)
            unaryFunction(*first);
        return first;
    }

    template< typename InputIterT, typename ValueT >
    inline typename std::iterator_traits<InputIterT>::difference_type count(InputIterT first, InputIterT last, const ValueT& value)
    {
        typename std::iterator_traits<InputIterT>::difference_type c = 0;
        for (; first != last; ++first)
        {
            if (*first == value)
                ++c;
        }
        return c;
    }

    template< typename InputIterT, typename UnaryPredicateT >
    inline typename std::iterator_traits<InputIterT>::difference_type count_if(InputIterT first, InputIterT last, UnaryPredicateT unaryPredicate)
    {
        typename std::iterator_traits<InputIterT>::difference_type c = 0;
        for (; first != last; ++first)
        {
            if (unaryPredicate(*first))
                ++c;
        }
        return c;
    }

    template< typename InputIterT1, typename InputIterT2, typename BinaryPredicateT >
    std::pair<InputIterT1, InputIterT2> mismatch(InputIterT1 first1, InputIterT1 last1, InputIterT2 first2, InputIterT2 last2, BinaryPredicateT binaryPredicate)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!binaryPredicate(*first1, *first2))
                break;
        }
        return std::make_pair(first1, first2);
    }

    template< typename InputIterT1, typename InputIterT2 >
    std::pair<InputIterT1, InputIterT2> mismatch(InputIterT1 first1, InputIterT1 last1, InputIterT2 first2, InputIterT2 last2)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!(*first1 == *first2))
                break;
        }
        return std::make_pair(first1, first2);
    }

    template< typename InputIterT1, typename InputIterT2, typename BinaryPredicateT >
    bool equal(InputIterT1 first1, InputIterT1 last1, InputIterT2 first2, InputIterT2 last2, BinaryPredicateT binaryPredicate)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!binaryPredicate(*first1, *first2))
                return false;
        }
        return first1 == last1 && first2 == last2;
    }

    template< typename InputIterT1, typename InputIterT2 >
    bool equal(InputIterT1 first1, InputIterT1 last1, InputIterT2 first2, InputIterT2 last2)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!(*first1 == *first2))
                return false;
        }
        return first1 == last1 && first2 == last2;
    }

    template< typename InputIterT, typename ValueT >
    InputIterT find(InputIterT first, InputIterT last, const ValueT& value)
    {
        for (; first != last; ++first)
        {
            if (*first == value)
                break;
        }
        return first;
    }

    template< typename InputIterT, typename UnaryPredicateT >
    InputIterT find_if(InputIterT first, InputIterT last, const UnaryPredicateT& unaryPredicate)
    {
        for (; first != last; ++first)
        {
            if (unaryPredicate(*first))
                break;
        }
        return first;
    }

    template< typename InputIterT, typename UnaryPredicateT >
    InputIterT find_if_not(InputIterT first, InputIterT last, const UnaryPredicateT& unaryPredicate)
    {
        for (; first != last; ++first)
        {
            if (!unaryPredicate(*first))
                break;
        }
        return first;
    }

    namespace detail {
        template< typename ForwardIterT1, typename ForwardIterT2, typename BinaryPredicateT >
        inline bool beginsWith(ForwardIterT1 first, ForwardIterT1 last, ForwardIterT2 subFirst, ForwardIterT2 subLast, BinaryPredicateT binaryPredicate)
        {
            for (; first != last && subFirst != subLast; ++first, ++subFirst)
            {
                if (!binaryPredicate(*first, *subFirst))
                    return false;
            }
            return subFirst == subLast;
        }
        struct EqualityComparer
        {
            template< typename T1, typename T2 >
            bool operator()(const T1& t1, const T2& t2) const
            {
                return t1 == t2;
            }
        };
    }

    template< typename ForwardIterT1, typename ForwardIterT2, typename BinaryPredicateT >
    inline ForwardIterT1 search(ForwardIterT1 first, ForwardIterT1 last, ForwardIterT2 subFirst, ForwardIterT2 subLast, BinaryPredicateT binaryPredicate)
    {
        auto subNext = subFirst;
        if (subNext == subLast)
            return first;
        ++subNext;
        auto firstCharComparer = std::bind(binaryPredicate, *subFirst, std::placeholders::_1);
        auto firstCharMatch = my::find_if(first, last, firstCharComparer);
        while (firstCharMatch != last)
        {
            auto secondChar = firstCharMatch; ++secondChar;
            if (detail::beginsWith(secondChar, last, subNext, subLast, binaryPredicate))
                return firstCharMatch;
            ++first;
            firstCharMatch = my::find_if(first, last, firstCharComparer);
        }
        return last;
    }

    template< typename ForwardIterT1, typename ForwardIterT2 >
    inline ForwardIterT1 search(ForwardIterT1 first, ForwardIterT1 last, ForwardIterT2 subFirst, ForwardIterT2 subLast)
    {
        return my::search(first, last, subFirst, subLast, detail::EqualityComparer());
    }

    template< typename ForwardIterT1, typename ForwardIterT2, typename BinaryPredicateT >
    inline ForwardIterT1 find_end(ForwardIterT1 first, ForwardIterT1 last, ForwardIterT2 subFirst, ForwardIterT2 subLast, BinaryPredicateT binaryPredicate)
    {
        if (subFirst == subLast)
            return last;
        auto currentMatch = first;
        auto lastMatch = last;
        do
        {
            currentMatch = my::search(currentMatch, last, subFirst, subLast, binaryPredicate);
            if (currentMatch == last)
                break;
            lastMatch = currentMatch;
            ++currentMatch;
        } while (currentMatch != last);
        return lastMatch;
    }

    template< typename ForwardIterT1, typename ForwardIterT2 >
    inline ForwardIterT1 find_end(ForwardIterT1 first, ForwardIterT1 last, ForwardIterT2 subFirst, ForwardIterT2 subLast)
    {
        return my::find_end(first, last, subFirst, subLast, detail::EqualityComparer());
    }

    template< typename InputIterT, typename ForwardIterT, typename BinaryPredicateT >
    inline InputIterT find_first_of(InputIterT first, InputIterT last, ForwardIterT subFirst, ForwardIterT subLast, BinaryPredicateT binaryPredicate)
    {
        for (; first != last; ++first)
        {
            auto comparer = std::bind(binaryPredicate, std::placeholders::_1, *first);
            if (my::any_of(subFirst, subLast, comparer))
                break;
        }
        return first;
    }

    template< typename InputIterT, typename ForwardIterT >
    inline InputIterT find_first_of(InputIterT first, InputIterT last, ForwardIterT subFirst, ForwardIterT subLast)
    {
        return my::find_first_of(first, last, subFirst, subLast, detail::EqualityComparer());
    }

    template< typename ForwardIterT, typename BinaryPredicateT >
    ForwardIterT adjacent_find(ForwardIterT first, ForwardIterT last, BinaryPredicateT binaryPredicate)
    {
        if (first == last)
            return last;
        for (auto prev = first++; first != last; ++prev, ++first)
        {
            if (binaryPredicate(*prev, *first))
                return prev;
        }
        return last;
    }

    template< typename ForwardIterT >
    ForwardIterT adjacent_find(ForwardIterT first, ForwardIterT last)
    {
        return my::adjacent_find(first, last, detail::EqualityComparer());
    }

    template< typename ForwardIterT, typename SizeT, typename ValueT, typename BinaryPredicateT >
    ForwardIterT search_n(ForwardIterT first, ForwardIterT last, SizeT count, const ValueT& value, BinaryPredicateT binaryPredicate)
    {
        if (count <= 0)
            return first;
        auto foundStart = last;
        SizeT foundCount = 0;
        for (; first != last; ++first)
        {
            if (binaryPredicate(*first, value))
            {
                if (foundCount == 0)
                    foundStart = first;
                ++foundCount;
                if (foundCount == count)
                    return foundStart;
            }
            else
                foundCount = 0;
        }
        return last;
    }

    template< typename ForwardIterT, typename SizeT, typename ValueT >
    ForwardIterT search_n(ForwardIterT first, ForwardIterT last, SizeT count, const ValueT& value)
    {
        return my::search_n(first, last, count, value, detail::EqualityComparer());
    }

    template< typename InputIterT, typename OutputIterT >
    OutputIterT copy(InputIterT inFirst, InputIterT inLast, OutputIterT outFirst)
    {
        for (; inFirst != inLast; )
            *outFirst++ = *inFirst++;
        return outFirst;
    }

    template< typename InputIterT, typename OutputIterT, typename UnaryPredicateT >
    OutputIterT copy_if(InputIterT inFirst, InputIterT inLast, OutputIterT outFirst, UnaryPredicateT unaryPredicate)
    {
        for (; inFirst != inLast; )
        {
            auto value = *inFirst++;
            if (unaryPredicate(value))
                *outFirst++ = value;
        }
        return outFirst;
    }

    template< typename InputIterT, typename SizeT, typename OutputIterT >
    OutputIterT copy_n(InputIterT inFirst, SizeT count, OutputIterT outFirst)
    {
        if (count <= 0)
            return outFirst;
        *outFirst++ = *inFirst++;
        for (SizeT i = 1; i != count; ++i)
            *outFirst++ = *inFirst++;
        return outFirst;
    }

    template< typename BidirIterT1, typename BidirIterT2 >
    BidirIterT2 copy_backward(BidirIterT1 inFirst, BidirIterT1 inLast, BidirIterT2 outLast)
    {
        for (; inLast != inFirst;)
            *--outLast = *--inLast;
        return outLast;
    }

    template< typename InputIterT, typename OutputIterT >
    OutputIterT move(InputIterT inFirst, InputIterT inLast, OutputIterT outFirst)
    {
        for (; inFirst != inLast; )
            *outFirst++ = std::move(*inFirst++);
        return outFirst;
    }

    template< typename BidirIterT1, typename BidirIterT2 >
    BidirIterT2 move_backward(BidirIterT1 inFirst, BidirIterT1 inLast, BidirIterT2 outLast)
    {
        for (; inLast != inFirst;)
            *--outLast = std::move(*--inLast);
        return outLast;
    }

    template< typename ForwardIterT, typename ValueT >
    void fill(ForwardIterT first, ForwardIterT last, const ValueT& value)
    {
        for (; first != last; ++first)
            *first = value;
    }

    template< typename OutputIterT, typename SizeT, typename ValueT >
    OutputIterT fill_n(OutputIterT first, SizeT count, const ValueT& value)
    {
        if (count <= 0)
            return first;
        *first++ = value;
        for (SizeT i = 1; i != count; ++i)
            *first++ = value;
        return first;
    }

    template< typename ForwardIterT, typename GeneratorT >
    void generate(ForwardIterT first, ForwardIterT last, GeneratorT generator)
    {
        for (; first != last; ++first)
            *first = generator();
    }

    template< typename OutputIterT, typename SizeT, typename GeneratorT >
    OutputIterT generate_n(OutputIterT first, SizeT count, GeneratorT generator)
    {
        if (count <= 0)
            return first;
        *first++ = generator();
        for (SizeT i = 1; i != count; ++i)
            *first++ = generator();
        return first;
    }

    template< typename InputIterT, typename OutputIterT, typename UnaryOperationT >
    OutputIterT transform(InputIterT inFirst, InputIterT inLast, OutputIterT outFirst, UnaryOperationT unaryOperation)
    {
        for (; inFirst != inLast; ++inFirst)
            *outFirst++ = unaryOperation(*inFirst);
        return outFirst;
    }

    template< typename InputIterT1, typename InputIterT2, typename OutputIterT, typename BinaryOperationT >
    OutputIterT transform(InputIterT1 inFirst1, InputIterT1 inLast1, InputIterT2 inFirst2, OutputIterT outFirst, BinaryOperationT binaryOperation)
    {
        for (; inFirst1 != inLast1; ++inFirst1, ++inFirst2)
            *outFirst++ = binaryOperation(*inFirst1, *inFirst2);
        return outFirst;
    }

    template< typename ForwardIterT, typename ValueT >
    ForwardIterT remove(ForwardIterT first, ForwardIterT last, const ValueT& value)
    {
        auto newLast = first;
        for (; first != last; ++first)
        {
            if (!(*first == value))
            {
                if (newLast != first)
                    *newLast = *first;
                ++newLast;
            }
        }
        return newLast;
    }

    template< typename ForwardIterT, typename UnaryPredicateT >
    ForwardIterT remove_if(ForwardIterT first, ForwardIterT last, UnaryPredicateT unaryPredicate)
    {
        auto newLast = first;
        for (; first != last; ++first)
        {
            if (!unaryPredicate(*first))
            {
                if (newLast != first)
                    *newLast = *first;
                ++newLast;
            }
        }
        return newLast;
    }

    template< typename InputIterT, typename OutputIterT, typename ValueT >
    OutputIterT remove_copy(InputIterT inFirst, InputIterT inLast, OutputIterT outFirst, const ValueT& value)
    {
        for (; inFirst != inLast; ++inFirst)
        {
            if (!(*inFirst == value))
                *outFirst++ = *inFirst;
        }
        return outFirst;
    }

    template< typename InputIterT, typename OutputIterT, typename UnaryPredicateT >
    OutputIterT remove_copy_if(InputIterT inFirst, InputIterT inLast, OutputIterT outFirst, UnaryPredicateT unaryPredicate)
    {
        for (; inFirst != inLast; ++inFirst)
        {
            if (!unaryPredicate(*inFirst))
                *outFirst++ = *inFirst;
        }
        return outFirst;
    }

    template< typename ForwardIterT, typename ValueT >
    void replace(ForwardIterT first, ForwardIterT last, const ValueT& oldValue, const ValueT& newValue)
    {
        for (; first != last; ++first)
        {
            if (*first == oldValue)
                *first = newValue;
        }
    }

    template< typename ForwardIterT, typename UnaryPredicateT, typename ValueT >
    void replace_if(ForwardIterT first, ForwardIterT last, UnaryPredicateT unaryPredicate, const ValueT& newValue)
    {
        for (; first != last; ++first)
        {
            if (unaryPredicate(*first))
                *first = newValue;
        }
    }

    template< typename InputIterT, typename OutputIterT, typename ValueT >
    OutputIterT replace_copy(InputIterT inFirst, InputIterT inLast, OutputIterT outFirst, const ValueT& oldValue, const ValueT& newValue)
    {
        for (; inFirst != inLast; ++inFirst)
            *outFirst++ = (*inFirst == oldValue) ? newValue : *inFirst;
        return outFirst;
    }

    template< typename InputIterT, typename OutputIterT, typename UnaryPredicateT, typename ValueT >
    OutputIterT replace_copy_if(InputIterT inFirst, InputIterT inLast, OutputIterT outFirst, UnaryPredicateT unaryPredicate, const ValueT& newValue)
    {
        for (; inFirst != inLast; ++inFirst)
            *outFirst++ = unaryPredicate(*inFirst) ? newValue : *inFirst;
        return outFirst;
    }

    template< typename T >
    void swap(T& a, T& b)
    {
        auto temp = std::move(a);
        a = std::move(b);
        b = std::move(temp);
    }

    template< typename T2, size_t N >
    void swap(T2(&a)[N], T2(&b)[N])
    {
        using my::swap;
        for (size_t i = 0; i != N; ++i)
            swap(a[i], b[i]);
    }

    template< typename ForwardIterT1, typename ForwardIterT2 >
    void iter_swap(ForwardIterT1 iter1, ForwardIterT2 iter2)
    {
        using my::swap;
        swap(*iter1, *iter2);
    }

    template< typename ForwardIterT1, typename ForwardIterT2 >
    ForwardIterT2 swap_ranges(ForwardIterT1 first1, ForwardIterT1 last1, ForwardIterT2 first2)
    {
        for (; first1 != last1; ++first1, ++first2)
            my::iter_swap(first1, first2);
        return first2;
    }

    template< bool Enabled >
    struct Printer
    {
        template< typename InputIterT >
        void operator()(InputIterT first, InputIterT last)
        {
        }
    };

    template<>
    struct Printer<true>
    {
        template< typename InputIterT >
        void operator()(InputIterT first, InputIterT last)
        {
            for (; first != last; ++first)
            {
                std::cout << *first << ", ";
            }
            std::cout << "\n";
        }
    };

    template< typename BidirIterT, typename BinaryPredicateT >
    inline void inplace_merge(BidirIterT first, BidirIterT middle, BidirIterT last, BinaryPredicateT binaryPredicate)
    {
        Printer<DEBUG_PRINT_ALGORITHMS> print;
        print(first, last);
        for (auto current1 = first, current2 = middle; current1 != current2 && current2 != last;)
        {
            if (binaryPredicate(*current2, *current1))
            {
                for (auto shifted1 = current2; shifted1 != current1; --shifted1)
                {
                    using std::swap;
                    swap(*shifted1, *(shifted1 - 1));
                }
                ++current1;
                ++current2;
            }
            else
                ++current1;
            print(first, last);
        }
        print(first, last);
    }

    template< typename BidirIterT >
    inline void inplace_merge(BidirIterT first, BidirIterT middle, BidirIterT last)
    {
        my::inplace_merge(first, middle, last, std::less<typename std::iterator_traits<BidirIterT>::value_type>());
    }

    template< typename RandomIterT, typename BinaryPredicateT >
    inline void sort(RandomIterT first, RandomIterT last, BinaryPredicateT binaryPredicate)
    {
        const auto size = std::distance(first, last);
        if (size < 2)
            return;
        const auto middle = first + size / 2;
        my::sort(first, middle, binaryPredicate);
        my::sort(middle, last, binaryPredicate);
        my::inplace_merge(first, middle, last, binaryPredicate);
    }

    template< typename RandomIterT >
    inline void sort(RandomIterT first, RandomIterT last)
    {
        my::sort(first, last, std::less<typename std::iterator_traits<RandomIterT>::value_type>());
    }

    template< typename BidirIterT >
    inline void reverse(BidirIterT first, BidirIterT last)
    {
        for (; first != last; ++first)
        {
            if (first == --last)
                return;
            my::iter_swap(first, last);
        }
    }

    template< typename BidirIterT, typename OutputIterT >
    OutputIterT reverse_copy(BidirIterT inFirst, BidirIterT inLast, OutputIterT outFirst)
    {
        for (; inFirst != inLast; )
            *outFirst++ = *--inLast;
        return outFirst;
    }

    template< typename ForwardIterT >
    ForwardIterT rotate(ForwardIterT first, ForwardIterT mid, ForwardIterT last)
    {
        Printer<DEBUG_PRINT_ALGORITHMS> print;
        print(first, last);
        if (first == mid)
            return last;
        if (mid == last)
            return first;
        auto firstCurrent = first;
        auto midCurrent = mid;
        auto returned = last;
        for (;;)
        {
            do
            {
                using std::iter_swap;
                iter_swap(firstCurrent++, midCurrent++);
            } while (firstCurrent != mid && midCurrent != last);
            print(first, last);
            if (midCurrent == last)
            {
                if (returned == last)
                    returned = firstCurrent;
                if (firstCurrent == mid)
                    break;
                midCurrent = mid;
            }
            else if (firstCurrent == mid)
                mid = midCurrent;
        }
        return returned;
    }

    template< typename ForwardIterT, typename OutputIterT >
    OutputIterT rotate_copy(ForwardIterT inFirst, ForwardIterT inMid, ForwardIterT inLast, OutputIterT outFirst)
    {
        for (auto inCurrent = inMid; inCurrent != inLast; )
            *outFirst++ = *inCurrent++;
        for (auto inCurrent = inFirst; inCurrent != inMid; )
            *outFirst++ = *inCurrent++;
        return outFirst;
    }

    template< typename ForwardIterT, typename BinaryPredicateT >
    ForwardIterT unique(ForwardIterT first, ForwardIterT last, BinaryPredicateT binaryPredicate)
    {
        auto next = my::adjacent_find(first, last, binaryPredicate);
        if (next == last)
            return last;
        auto prev = next++;
        while (next != last)
        {
            if (binaryPredicate(*prev, *next))
                ++next;
            else
                *++prev = *next++;
        }
        return ++prev;
    }

    template< typename ForwardIterT >
    ForwardIterT unique(ForwardIterT first, ForwardIterT last)
    {
        return my::unique(first, last, detail::EqualityComparer());
    }

    template< typename InputIterT, typename OutputIterT, typename BinaryPredicateT >
    OutputIterT unique_copy(InputIterT inFirst, InputIterT inLast, OutputIterT outFirst, BinaryPredicateT binaryPredicate)
    {
        Printer<DEBUG_PRINT_ALGORITHMS> print;
        print(inFirst, inLast);
        if (inFirst == inLast)
            return outFirst;
        auto prev = inFirst;
        auto next = inFirst;
        *outFirst++ = *next++;
        while (next != inLast)
        {
            if (!binaryPredicate(*prev, *next))
                *outFirst++ = *next;
            ++prev;
            ++next;
        }
        return outFirst;
    }

    template< typename InputIterT, typename OutputIterT >
    OutputIterT unique_copy(InputIterT inFirst, InputIterT inLast, OutputIterT outFirst)
    {
        return my::unique_copy(inFirst, inLast, outFirst, detail::EqualityComparer());
    }

    template< typename InputIterT, typename UnaryPredicateT >
    bool is_partitioned(InputIterT first, InputIterT last, UnaryPredicateT unaryPredicate)
    {
        const auto no = my::find_if_not(first, last, unaryPredicate);
        if (no == last)
            return true;
        const auto yes = my::find_if(no, last, unaryPredicate);
        return yes == last;
    }

    template< typename ForwardIterT, typename UnaryPredicateT >
    ForwardIterT partition(ForwardIterT first, ForwardIterT last, UnaryPredicateT unaryPredicate)
    {
        auto no = my::find_if_not(first, last, unaryPredicate);
        if (no == last)
            return last;
        auto yes = my::find_if(no, last, unaryPredicate);
        while (yes != last)
        {
            my::iter_swap(no++, yes++);
            yes = my::find_if(yes, last, unaryPredicate);
        }
        return no;
    }

    template< typename InputIterT, typename OutputIterT1, typename OutputIterT2, typename UnaryPredicateT >
    std::pair<OutputIterT1, OutputIterT2> partition_copy(InputIterT inFirst, InputIterT inLast, OutputIterT1 outFirstTrue, OutputIterT2 outFirstFalse, UnaryPredicateT unaryPredicate)
    {
        for (; inFirst != inLast; ++inFirst)
        {
            if (unaryPredicate(*inFirst))
                *outFirstTrue++ = *inFirst;
            else
                *outFirstFalse++ = *inFirst;
        }
        return std::make_pair(outFirstTrue, outFirstFalse);
    }

    template< typename BidirIterT, typename UnaryPredicateT >
    BidirIterT stable_partition(BidirIterT first, BidirIterT last, UnaryPredicateT unaryPredicate)
    {
        auto no = my::find_if_not(first, last, unaryPredicate);
        if (no == last)
            return last;
        auto yes = my::find_if(no, last, unaryPredicate);
        while (yes != last)
        {
            for (auto prev = yes; prev != no; )
            {
                auto current = prev--;
                my::iter_swap(current, prev);
            }
            ++no;
            yes = my::find_if(yes, last, unaryPredicate);
        }
        return no;
    }

    template< typename ForwardIterT, typename UnaryPredicateT >
    ForwardIterT partition_point(ForwardIterT first, ForwardIterT last, UnaryPredicateT p)
    {
        const auto no = my::find_if_not(first, last, unaryPredicate);
        return no;
    }

} // namespace my
