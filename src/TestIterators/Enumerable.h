
#pragma once


#include <memory>


namespace dot_net_style {

    template< typename T >
    class IEnumerator
    {
    public:

        virtual ~IEnumerator() = default;

        virtual const T& Current() const = 0;

        virtual bool MoveNext() = 0;

        virtual void Reset() = 0;

        virtual std::unique_ptr<IEnumerator<T>> Clone() const = 0;

        virtual bool Equal(const IEnumerator<T>& comparable) const = 0;
    };

    template< typename T >
    class IEnumerable
    {
    public:

        virtual ~IEnumerable() = default;

        virtual std::unique_ptr<IEnumerator<T>> GetEnumerator() const = 0;
    };

} // namespace dot_net_style
