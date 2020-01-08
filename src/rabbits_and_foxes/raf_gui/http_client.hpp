
#pragma once

#include <string>
#include <memory>
#include <platform/platform.h>
#if EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(push)
#pragma warning(disable : 5104) // disable warning C5104: found 'L#y' in macro
                                // replacement list, did you mean 'L""#y'?
#endif
#include <boost/thread/future.hpp>
#if EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(pop)
#endif

namespace raf { namespace visual {

    class HttpClient
    {
    public:

        HttpClient(std::string host, std::string port);

        HttpClient(HttpClient&&) noexcept;

        ~HttpClient() noexcept;

        HttpClient& operator=(HttpClient&&) noexcept;

        boost::future<std::string> async_get(std::string path);

    private:

        class Impl;

        std::unique_ptr<Impl> m_impl;
    };

}} // namespace raf::visual
