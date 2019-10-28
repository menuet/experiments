
#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <set>
#include <string>

namespace cci { namespace c1 {

    inline bool all_unique_characters(const std::string& s)
    {
        std::set<char> chars;
        for (const auto& c : s)
        {
            if (!chars.insert(c).second)
                return false;
        }
        return true;
    }

    inline void reverse(char* const s)
    {
        assert(s);
        const auto length = std::strlen(s);
        if (length <= 1)
            return;
        for (auto left = s, right = s + length - 1; left < right;
             ++left, --right)
            std::swap(*left, *right);
    }

    inline std::size_t remove_adjacent_duplicates(std::string& s)
    {
        const auto original_length = s.length();
        if (original_length < 2)
            return 0;
        auto prev = s.begin();
        auto next = std::next(s.begin());
        for (; next != s.end(); ++next)
        {
            if (*next != *prev)
            {
                ++prev;
                *prev = *next;
            }
        }
        s.erase(std::next(prev), s.end());
        return original_length - s.length();
    }

    inline std::size_t remove_duplicates(std::string& s)
    {
        const auto original_length = s.length();
        if (original_length < 2)
            return 0;
        const auto non_dup_begin = s.begin();
        auto non_dup_end = std::next(non_dup_begin);
        for (auto tested = non_dup_end; tested != s.end(); ++tested)
        {
            if (std::find(non_dup_begin, non_dup_end, *tested) == non_dup_end)
                *non_dup_end++ = *tested;
        }
        s.erase(non_dup_end, s.end());
        return original_length - s.length();
    }

    inline bool are_anagrams(std::string s1, std::string s2)
    {
        std::sort(s1.begin(), s1.end());
        std::sort(s2.begin(), s2.end());
        return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end());
    }

    inline std::string replace_spaces(std::string s,
                                      const std::string& searched = " ",
                                      const std::string& replacement = "%20")
    {
        auto b = begin(s);
        const auto e = end(s);
        std::string new_s;
        for (;;)
        {
            const auto space_iter =
                std::search(b, e, searched.begin(), searched.end());
            new_s.append(b, space_iter);
            if (space_iter == e)
                break;
            new_s.append(replacement);
            b = std::next(space_iter, searched.length());
        }
        return new_s;
    }

    inline std::size_t count(const std::string& s, const std::string& searched)
    {
        auto b = begin(s);
        const auto e = end(s);
        std::size_t occurrences = 0;
        for (;;)
        {
            const auto iter =
                std::search(b, e, searched.begin(), searched.end());
            if (iter == e)
                return occurrences;
            ++occurrences;
            b = std::next(iter, searched.length());
        }
    }

    inline void replace_spaces_inplace(std::string& s,
                                       const std::string& searched = " ",
                                       const std::string& replacement = "%20")
    {
        const auto spaces_count = count(s, searched);
        if (spaces_count == 0)
            return;
        if (replacement.length() < searched.length())
            return; // case not handled
        const auto additional_length =
            (replacement.length() - searched.length()) * spaces_count;
        const auto previous_length = s.length();
        const auto new_length = previous_length + additional_length;
        s.resize(new_length);

        auto r_new_b = rbegin(s);
        auto r_old_b = rbegin(s) + additional_length;
        const auto r_e = rend(s);
        for (;;)
        {
            const auto r_space =
                std::search(r_old_b, r_e, searched.rbegin(), searched.rend());

            // Copy old thing up to the space but not included
            for (; r_old_b != r_space && r_new_b != r_e && r_old_b != r_e;
                 ++r_new_b, ++r_old_b)
                *r_new_b = *r_old_b;

            if (r_space == r_e)
                return;

            // Replace space with %20
            assert(std::distance(r_new_b, r_e) >=
                   static_cast<std::ptrdiff_t>(replacement.length()));
            std::copy(replacement.rbegin(), replacement.rend(), r_new_b);
            std::advance(r_new_b, replacement.length());

            assert(std::distance(r_old_b, r_e) >=
                   static_cast<std::ptrdiff_t>(searched.length()));
            std::advance(r_old_b, searched.length());
        }
    }

    template <typename DataT, std::size_t M, std::size_t N>
    struct Matrix
    {
        std::array<DataT, M * N> arr;
    };

    template <typename DataT, std::size_t N>
    using SquareMatrix = Matrix<DataT, N, N>;

    template <typename ColorT, std::size_t M, std::size_t N>
    inline ColorT& at(Matrix<ColorT, M, N>& img, std::size_t row,
                      std::size_t col)
    {
        return img.arr[row * N + col];
    }

    template <typename ColorT, std::size_t M, std::size_t N>
    inline const ColorT& at(const Matrix<ColorT, M, N>& img, std::size_t row,
                            std::size_t col)
    {
        return img.arr[row * N + col];
    }

    template <typename ColorT, std::size_t N>
    inline void vert_sym(SquareMatrix<ColorT, N>& img)
    {
        using std::swap;
        for (auto row = 0; row < N; ++row)
            for (auto col = 0; col < N / 2; ++col)
                swap(at(img, row, col), at(img, row, N - 1 - col));
    }

    template <typename ColorT, std::size_t N>
    inline void hori_sym(SquareMatrix<ColorT, N>& img)
    {
        using std::swap;
        for (auto row = 0; row < N / 2; ++row)
            for (auto col = 0; col < N; ++col)
                swap(at(img, row, col), at(img, N - 1 - row, col));
    }

    template <typename ColorT, std::size_t N>
    inline void diag_sym(SquareMatrix<ColorT, N>& img)
    {
        using std::swap;
        for (auto row = 0; row < N; ++row)
            for (auto col = row + 1; col < N; ++col)
                swap(at(img, row, col), at(img, col, row));
    }

    template <typename ColorT, std::size_t N>
    inline void rotate_90(SquareMatrix<ColorT, N>& img)
    {
        diag_sym(img);
        vert_sym(img);
    }

    template <typename DataT, std::size_t M, std::size_t N>
    inline void fill_row_col(Matrix<DataT, M, N>& matrix, std::size_t fill_row,
                             std::size_t fill_col, const DataT& value)
    {
        for (auto row = 0; row < N; ++row)
            at(matrix, row, fill_col) = value;
        for (auto col = 0; col < N; ++col)
            at(matrix, fill_row, col) = value;
    }

    template <typename DataT, std::size_t M, std::size_t N>
    [[nodiscard]] inline Matrix<DataT, M, N>
    zero_in_rows_and_columns(const Matrix<DataT, M, N>& src, const DataT& zero)
    {
        Matrix<DataT, M, N> dst = src;
        for (auto row = 0; row < N; ++row)
            for (auto col = 0; col < N; ++col)
            {
                if (at(src, row, col) == zero)
                    fill_row_col(dst, row, col, zero);
            }
        return dst;
    }

    inline bool is_rotation(std::string s1, const std::string& s2)
    {
        if (s1.length() != s2.length())
            return false;
        s1 += s1;
        return s1.find(s2) != std::string::npos;
    }

}} // namespace cci::c1
