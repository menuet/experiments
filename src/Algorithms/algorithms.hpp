
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

} // namespace algo
