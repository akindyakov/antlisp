#pragma once

#include <antlisp/lib/util/exception.h>
#include <antlisp/lib/util/optional.h>

#include <antlisp/lib/parser/parentheses/code_stream/code_stream.h>


namespace AntLisp {

class ParenthesesParser
{
public:
    ParenthesesParser() = delete;
    ~ParenthesesParser();

    ParenthesesParser(ParenthesesParser&& other)
        : codeStream_(other.codeStream_)
        , level_(other.level_)
    {
        other.level_ = -1;
    }

    ParenthesesParser(const ParenthesesParser&) = delete;
    ParenthesesParser& operator=(const ParenthesesParser&) = delete;
    ParenthesesParser& operator=(ParenthesesParser&&) = delete;

    class Error
        : public Exception
    {
    };

    static ParenthesesParser fromCodeStream(
        InCodeStream& codeStream
    );

    bool isLocked() const;

    bool isEnd() const;

    bool good() const;

    bool nextToken(
        std::string& token
    );

    std::string nextToken();

    Optional<ParenthesesParser> nextParser();

    const CodeStat& getStat() const {
        return codeStream_.getStat();
    }

    void close();

private:
    explicit ParenthesesParser(
        InCodeStream& reader_
        , int level_
    );

private:
    InCodeStream& codeStream_;
    int level_ = 0;
};

}  // namespace AntLisp
