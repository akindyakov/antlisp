

namespace AntLisp {

template<>
class TypeInfo<Nil>
{
public:
    static constexpr const char* name() {
        return "nil";
    }
    static std::string toString(Nil) {
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
class TypeInfo<ExtTypePtr>
{
public:
    static constexpr const char* name() {
        return "ext-type";
    }
    static std::string toString(const ExtTypePtr& ext) {
        if (not ext) {
          return "(ext-type nullptr)";
        }
        return ext->toString();
    }
};

template<>
class TypeInfo<FunctionPtr>
{
public:
    static constexpr const char* name() {
        return "function";
    }
    static std::string toString(FunctionPtr ptr) {
        auto out = std::ostringstream();
        out << name() << ":" << ptr;
        return out.str();
    }
};

}  // namespace AntLisp
