

namespace AntLisp {

template<>
class TypeInfo<Nil>
{
public:
    static constexpr const char* name() {
        return "nil";
    }
    static std::string toString(Nil = Nil{}) {
        return name();
    }
};

template<>
class TypeInfo<Integer>
{
public:
    static constexpr const char* name() {
        return "integer";
    }
    static std::string toString(Integer v) {
        return std::to_string(v);
    }
};

template<>
class TypeInfo<Float>
{
public:
    static constexpr const char* name() {
        return "float";
    }
    static std::string toString(Float v) {
        return std::to_string(v);
    }
};

template<>
class TypeInfo<Symbol>
{
public:
    static constexpr const char* name() {
        return "standard-char";
    }
    static std::string toString(Symbol ch) {
        return std::string("#\\") + ch;
    }
};

template<>
class TypeInfo<StringPtr>
{
public:
    static constexpr const char* name() {
        return "simple-base-string";
    }
    static std::string toString(const StringPtr& str) {
        return *str.get();
    }
};

template<>
class TypeInfo<FunctionDefinitionPtr>
{
public:
    static constexpr const char* name() {
        return "function";
    }
    static std::string toString(FunctionDefinitionPtr ptr) {
        auto out = std::ostringstream();
        out << name() << ": " << ptr;
        return out.str();
    }
};

template<>
class TypeInfo<ExtFunctionPtr>
{
public:
    static constexpr const char* name() {
        return "external-function";
    }
    static std::string toString(ExtFunctionPtr ptr) {
        auto out = std::ostringstream();
        out << name() << ": " << ptr;
        return out.str();
    }
};

template<>
class TypeInfo<ConsPtr>
{
public:
    static constexpr const char* name() {
        return "cons";
    }
    static std::string toString(const ConsPtr& cons) {
        auto out = std::ostringstream();
        out << '('
            << (cons ? cons->car.toString() : "?")
            << " . "
            << (cons ? cons->cdr.toString() : "?")
            << ')';
        return out.str();
    }
};

}  // namespace AntLisp
