
#pragma once


#include <cassert>


namespace std_ex {

    namespace detail {

        template< typename ResourceT >
        struct Proxy
        {
            ResourceT* m_pResource;
        };

        struct DefaultResourceDeleter
        {
            template< typename ResourceT >
            void operator()(ResourceT* a_pResource) const
            {
                delete a_pResource;
            }
        };

        template< typename DeleterT >
        class UniquePtr98Base
        {
        protected:

            template< typename ResourceT >
            void deleteResource(ResourceT* a_pResource) const
            {
                m_deleter(a_pResource);
            }

            void swap(UniquePtr98Base& a_unique) throw()
            {
                using std::swap;
                swap(m_deleter, a_unique.m_deleter);
            }

        private:

            DeleterT m_deleter;
        };

        template<>
        class UniquePtr98Base < DefaultResourceDeleter >
        {
        protected:

            template< typename ResourceT >
            void deleteResource(ResourceT* a_pResource) const
            {
                DefaultResourceDeleter()(a_pResource);
            }

            void swap(UniquePtr98Base& a_unique) throw()
            {
            }
        };

    } // detail

    template< typename ResourceT, typename DeleterT = detail::DefaultResourceDeleter >
    class unique_ptr98 : private detail::UniquePtr98Base<DeleterT>
    {
        typedef detail::UniquePtr98Base<DeleterT> Base;

        typedef void (unique_ptr98::* bool_type)() const;

        void this_type_does_not_support_comparisons() const {}

    public:

        explicit unique_ptr98(ResourceT* a_pResource = 0)
            : m_pResource(a_pResource)
        {
        }

        unique_ptr98(detail::Proxy<ResourceT> a_proxy) throw()
            : m_pResource(a_proxy.m_pResource)
        {
        }

        ~unique_ptr98()
        {
            reset();
        }

        unique_ptr98& operator=(detail::Proxy<ResourceT> a_proxy) throw()
        {
            unique_ptr98 l_temp(a_proxy);
            l_temp.swap(*this);
            return *this;
        }

        void swap(unique_ptr98& a_unique) throw()
        {
            using std::swap;
            Base::swap(a_unique);
            swap(this->m_pResource, a_unique.m_pResource);
        }

        operator detail::Proxy<ResourceT>() throw()
        {
            detail::Proxy<ResourceT> l_proxy;
            l_proxy.m_pResource = this->m_pResource;
            this->m_pResource = 0;
            return l_proxy;
        }

        void reset(ResourceT* a_pResource = 0)
        {
            if (m_pResource)
                Base::deleteResource(m_pResource);
            m_pResource = a_pResource;
        }

        ResourceT* get() const
        {
            return m_pResource;
        }

        ResourceT* operator->() const
        {
            assert(m_pResource);
            return m_pResource;
        }

        ResourceT& operator*() const
        {
            assert(m_pResource);
            return *m_pResource;
        }

        operator bool_type() const
        {
            return m_pResource ? &unique_ptr98::this_type_does_not_support_comparisons : 0;
        }

    private:

        unique_ptr98(unique_ptr98& a_unique) throw(); // = delete
        unique_ptr98& operator=(unique_ptr98& a_unique) throw(); // = delete

        ResourceT* m_pResource;
    };

    template< typename ResourceT, typename DeleterT >
    inline unique_ptr98<ResourceT, DeleterT> move98(unique_ptr98<ResourceT, DeleterT>& a_unique) throw()
    {
        return unique_ptr98<ResourceT, DeleterT>(detail::Proxy<ResourceT>(a_unique));
    }

    template< typename ResourceT>
    inline unique_ptr98<ResourceT> make_unique98()
    {
        unique_ptr98<ResourceT> l_uniquePtr(new ResourceT);
        return move98(l_uniquePtr);
    }

    template< typename ResourceT, typename ArgT>
    inline unique_ptr98<ResourceT> make_unique98(const ArgT& a_arg)
    {
        unique_ptr98<ResourceT> l_uniquePtr(new ResourceT(a_arg));
        return move98(l_uniquePtr);
    }

    template< typename ResourceT, typename ArgT>
    inline unique_ptr98<ResourceT> make_unique98(ArgT& a_arg)
    {
        unique_ptr98<ResourceT> l_uniquePtr(new ResourceT(a_arg));
        return move98(l_uniquePtr);
    }

} // namespace std_ex
