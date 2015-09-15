
#pragma once


#include <algorithm>
#include <cassert>


namespace algo {

    namespace cppref {

        template <class ForwardIt>
        ForwardIt rotate(ForwardIt first, ForwardIt n_first, ForwardIt last)
        {
            assert(n_first != last);

            const auto begin = first;
            const auto end = last;
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

} // namespace algo
