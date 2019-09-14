
#pragma once


#include <algorithm>
#include <cassert>


namespace algo {

    // rotate
    namespace cppref {

        template <class ForwardIt>
        ForwardIt rotate(ForwardIt first, ForwardIt n_first, ForwardIt last)
        {
            assert(n_first != last);

            ForwardIt next = n_first;
            while (first != next) {
                std::iter_swap(first++, next++);
                if (next == last) {
                    next = n_first;
                }
                else if (first == n_first) {
                    n_first = next;
                }
            }
            return next;
        }

    } // namespace cppref

    namespace mine {

        template <class ForwardIt>
        ForwardIt rotate(ForwardIt first, ForwardIt n_first, ForwardIt last)
        {
            assert(n_first != last);

            if (first == n_first)
                return first;

            auto left = first;
            auto right = n_first;
            while (left != n_first && right != last)
                std::iter_swap(left++, right++);

            if (right == last)
                return mine::rotate(left, n_first, last);

            return mine::rotate(n_first, right, last);
        }

    } // namespace mine

    // partition
    namespace cppref {

        template<class BidirIt, class UnaryPredicate>
        BidirIt partition(BidirIt first, BidirIt last, UnaryPredicate p)
        {
            while (1) {
                while ((first != last) && p(*first)) {
                    ++first;
                }
                if (first == last--) break;
                while ((first != last) && !p(*last)) {
                    --last;
                }
                if (first == last) break;
                std::iter_swap(first++, last);
            }
            return first;
        }

    } // namespace cppref

    namespace mine {

        template<class FwdIt, class UnaryPredicate>
        FwdIt partition(FwdIt first, FwdIt last, UnaryPredicate p)
        {
            while ((first != last) && p(*first))
                ++first;
            if (first == last)
                return first;

            auto ppoint = first++;
            while (first != last)
            {
                if (p(*first))
                    std::iter_swap(ppoint++, first++);
                else
                    ++first;
            }

            return ppoint;
        }

    } // namespace mine

    // all_of, any_of, none_of
    namespace cppref {

        template< class InputIt, class UnaryPredicate >
        bool all_of(InputIt first, InputIt last, UnaryPredicate p)
        {
            return std::find_if_not(first, last, p) == last;
        }


        template< class InputIt, class UnaryPredicate >
        bool any_of(InputIt first, InputIt last, UnaryPredicate p)
        {
            return std::find_if(first, last, p) != last;
        }

        template< class InputIt, class UnaryPredicate >
        bool none_of(InputIt first, InputIt last, UnaryPredicate p)
        {
            return std::find_if(first, last, p) == last;
        }

    } // namespace cppref

    namespace mine {

        template< class InputItT, class UnaryPredicateT >
        bool all_of(InputItT first, InputItT last, UnaryPredicateT p)
        {
            for (; first != last; ++first)
            {
                if (!p(*first))
                    return false;
            }
            return true;
        }

        template< class InputItT, class UnaryPredicateT >
        bool any_of(InputItT first, InputItT last, UnaryPredicateT p)
        {
            for (; first != last; ++first)
            {
                if (p(*first))
                    return true;
            }
            return false;
        }

        template< class InputItT, class UnaryPredicateT >
        bool none_of(InputItT first, InputItT last, UnaryPredicateT p)
        {
            for (; first != last; ++first)
            {
                if (p(*first))
                    return false;
            }
            return true;
        }

    } // namespace mine

    // for_each
    namespace cppref {

        template<class InputIt, class UnaryFunction>
        UnaryFunction for_each(InputIt first, InputIt last, UnaryFunction f)
        {
            for (; first != last; ++first) {
                f(*first);
            }
            return f;
        }

    } // namespace cppref

    namespace mine {

        template< class InputItT, class UnaryFunctionT >
        UnaryFunctionT for_each(InputItT first, InputItT last, UnaryFunctionT f)
        {
            for (; first != last; ++first)
                f(*first);
            return f;
        }

    } // namespace mine
    // count, count_if
    namespace cppref {

        template<class InputIt, class T>
        typename std::iterator_traits<InputIt>::difference_type count(InputIt first, InputIt last, const T& value)
        {
            typename std::iterator_traits<InputIt>::difference_type ret = 0;
            for (; first != last; ++first) {
                if (*first == value) {
                    ret++;
                }
            }
            return ret;
        }

        template<class InputIt, class UnaryPredicate>
        typename std::iterator_traits<InputIt>::difference_type count_if(InputIt first, InputIt last, UnaryPredicate p)
        {
            typename std::iterator_traits<InputIt>::difference_type ret = 0;
            for (; first != last; ++first) {
                if (p(*first)) {
                    ret++;
                }
            }
            return ret;
        }

    } // namespace cppref

    namespace mine {

        template< class InputItT, class T >
        typename std::iterator_traits<InputItT>::difference_type count(InputItT first, InputItT last, const T& value)
        {
            typename std::iterator_traits<InputItT>::difference_type n = 0;
            for (; first != last; ++first)
            {
                if (*first == value)
                    ++n;
            }
            return n;
        }

        template< class InputItT, class UnaryPredicateT >
        typename std::iterator_traits<InputItT>::difference_type count_if(InputItT first, InputItT last, UnaryPredicateT p)
        {
            typename std::iterator_traits<InputItT>::difference_type n = 0;
            for (; first != last; ++first)
            {
                if (p(*first))
                    ++n;
            }
            return n;
        }

    } // namespace mine

} // namespace algo
