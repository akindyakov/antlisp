#pragma once

#include <antlisp/lib/util/exception.h>

#include <antlisp/lib/function/stack_machine.h>

#include "code_stream.h"


namespace AntLisp {

class ParenthesesParser
{
public:
    ParenthesesParser() = delete;
    ParenthesesParser(const ParenthesesParser&) = delete;
    ParenthesesParser(ParenthesesParser&&) = default;

    ParenthesesParser& operator=(const ParenthesesParser&) = delete;
    ParenthesesParser& operator=(ParenthesesParser&&) = default;

    class Error
        : public Exception
    {
    };

    static ParenthesesParser openFromCodeStream(
        InCodeStream& codeStream
    );
    static ParenthesesParser fromCodeStream(
        InCodeStream& codeStream
    );

    bool isLocked() const;

    bool good() const;

    bool nextToken(
        std::string& token
    );

    std::string nextToken();

    bool check();

    ParenthesesParser nextParser();

    int getLevel() const noexcept {
        return level;
    }

private:
    explicit ParenthesesParser(
        InCodeStream& reader_
        , int level_
    );

private:
    InCodeStream& codeStream;
    int level = 0;
};

}  // namespace AntLisp
