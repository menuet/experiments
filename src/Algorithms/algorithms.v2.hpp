
#pragma once


#include <cassert>
#include <iterator>
#include <utility>
#include <iostream>


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
        for (Size i=0; i != count; ++i, ++first)
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

    template< typename InputIter1, typename InputIter2, typename BinaryPredicateT >
    std::pair<InputIter1, InputIter2> mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, BinaryPredicateT binaryPredicate)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!binaryPredicate(*first1, *first2))
                break;
        }
        return std::make_pair(first1, first2);
    }

    template< typename InputIter1, typename InputIter2 >
    std::pair<InputIter1, InputIter2> mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!(*first1 == *first2))
                break;
        }
        return std::make_pair(first1, first2);
    }

    template< typename InputIter1, typename InputIter2, typename BinaryPredicateT >
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, BinaryPredicateT binaryPredicate)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!binaryPredicate(*first1, *first2))
                return false;
        }
        return first1 == last1 && first2 == last2;
    }

    template< typename InputIter1, typename InputIter2 >
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2)
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

    template< typename InputIterT >
    inline void print(InputIterT first, InputIterT last)
    {
#ifdef DEBUG_PRINT_RANGE
        for (; first != last; ++first)
        {
            std::cout << *first << ", ";
        }
        std::cout << "\n";
#endif
    }

    template< typename BidirIterT, typename BinaryPredicateT >
    inline void inplace_merge(BidirIterT first, BidirIterT middle, BidirIterT last, BinaryPredicateT binaryPredicate)
    {
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

} // namespace my
