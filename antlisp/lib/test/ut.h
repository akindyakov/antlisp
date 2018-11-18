#pragma once

#include <antlisp/lib/util/exception.h>

#include <fstream>

namespace AntLisp {

class UnitTestException
    : public Exception
{
public:
    explicit UnitTestException()
    {
        this->append("Unit test error. ");
    }

};

}  // namespace AntLisp


#define UT_ASSERT(code) \
    { \
        bool success = true; \
        const char* reason = nullptr; \
        try { \
            if (!(code)) { \
                success = false; \
                reason = "wrong value (" #code ")"; \
            } \
        } catch (...) { \
            reason = "an exception was throwned"; \
            success = false; \
        } \
        if (!success) { \
            throw AntLisp::UnitTestException() \
                <<  __FILE__ << ':' << __LINE__ \
                << " Assert failed: " << reason \
            ; \
        } \
    }


#define UT_ASSERT_EQUAL(left, right) \
    UT_ASSERT(left == right)

#define UT_ASSERT_NOT_EQUAL(left, right) \
    UT_ASSERT(left != right)

#define UT_ASSERT_EXCEPTION_TYPE(code, exception_type) \
    { \
        bool success = false; \
        const char* reason = nullptr; \
        try { \
            (volatile void)(code); \
        } catch (const exception_type&) { \
            success = true; \
        } catch (...) { \
            reason = ", wrong exception type, expected " #exception_type; \
        } \
        if (!success) { \
            if (reason == nullptr) { \
                reason = " Exception " #exception_type " was not throwned"; \
            } \
            throw AntLisp::UnitTestException() \
                << __FILE__ << ':' << __LINE__ \
                << reason \
            ; \
        } \
    }

#define UT_LIST(code) \
int main() { \
    std::fstream log_("test.results", std::ios::binary | std::ios::out); \
    try { \
        code \
    } catch (const AntLisp::UnitTestException& err) { \
        log_ << "FAIL\n"; \
        /* std::cerr << "failed: " << err.what() << '\n'; */ \
        log_ << "error: " << err.what() << '\n'; \
        return 1; \
    } \
    log_ << "PASS\n"; \
    return 0; \
}

#define RUN_TEST(test_function) {\
    auto success = true; \
    auto reason = std::string{}; \
    /* std::cerr << "  - [" #test_function "] "; */ \
    log_ << "=== RUN " <<  #test_function << '\n'; \
    try { \
        test_function(); \
    } catch (const std::exception& err) { \
        success = false; \
        reason = err.what(); \
    } \
    if (success) { \
        /* std::cerr << ": ok\n"; */ \
        log_ << "--- PASS: " << #test_function << " (0.00s)\n"; \
    } else { \
        /*std::cerr << ": failed " << reason << "\n"; */ \
        log_ << "--- FAIL: " << #test_function << " (0.00s)\n"; \
        throw AntLisp::UnitTestException(); \
    } \
}

