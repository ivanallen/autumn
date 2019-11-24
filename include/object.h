#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "color.h"
#include "program.h"
#include "format.h"

namespace autumn {
namespace object {
class Type {
public:
    enum TypeValue {
        INTEGER_OBJECT,
        BOOLEAN_OBJECT,
        STRING_OBJECT,
        ERROR_OBJECT,
        NULL_OBJECT,
        RETURN_OBJECT,
        FUNCTION_OBJECT,
        BUILTIN_OBJECT,
        ARRAY_OBJECT,
    };

    Type(TypeValue type) : _type(type) {
    }

    TypeValue value() const {
        return _type;
    }

    bool operator==(TypeValue type) const {
        return _type == type;
    }

    friend std::ostream& operator<<(
            std::ostream& out,
            const Type& type);
private:

    static std::unordered_map<int, std::string> _type_to_name;

    TypeValue _type;
};

class Object {
public:

    Object(Type type) : _type(type) {}
    virtual ~Object() {}

    const Type& type() const {
        return _type;
    }

    virtual std::string inspect() const = 0;

    template <typename T>
    const T* cast() const {
        return dynamic_cast<const T*>(this);
    }

    template <typename T>
    T* cast() {
        return dynamic_cast<T*>(this);
    }

protected:
    Type _type;
};

class Integer : public Object {
public:
    Integer(int value) :
            Object(Type::INTEGER_OBJECT),
            _value(value) {
    }

    std::string inspect() const override {
        std::string ret = std::to_string(_value);
        return color::light::yellow + ret + color::off;
    }

    int value() const {
        return _value;
    }
private:
    int _value = 0;
};

class Boolean : public Object {
public:
    Boolean(bool value) :
            Object(Type::BOOLEAN_OBJECT),
            _value(value) {
    }

    std::string inspect() const override {
        std::string ret = _value ? "true" : "false";
        return color::light::yellow + ret + color::off;
    }

    bool value() const {
        return _value;
    }
private:
    bool _value = 0;
};

class String: public Object {
public:
    String(const std::string& value) :
            Object(Type::STRING_OBJECT),
            _value(value) {
    }

    std::string inspect() const override {
        return format(R"("{}{}{}")",
                color::green,
                _value,
                color::off) ;
    }

    const std::string& value() const {
        return _value;
    }
private:
    std::string _value;
};

class Null : public Object {
public:
    Null() : Object(Type::NULL_OBJECT) {
    }

    std::string inspect() const override {
        return color::light::light + "null" + color::off;
    }
};

class ReturnValue: public Object {
public:
    ReturnValue(std::shared_ptr<Object>& value) :
        Object(Type::RETURN_OBJECT),
        _value(value) {
    }

    std::string inspect() const override {
        return _value->inspect();
    }

    std::shared_ptr<Object>& value() {
        return _value;
    }
private:
    std::shared_ptr<Object> _value;
};

class Error : public Object {
public:
    Error(const std::string& message) :
        Object(Type::ERROR_OBJECT),
        _message(message) {
    }

    std::string inspect() const override {
        return format("{}error:{} {}",
                color::light::red,
                color::off,
                _message);
    }

    const std::string& message() const {
        return _message;
    }
private:
    std::string _message;
};

class Environment;
class Function : public Object {
public:
    Function(
            std::vector<std::shared_ptr<ast::Identifier>>& parameters,
            std::shared_ptr<ast::BlockStatment> body,
            std::shared_ptr<Environment>& env) :
                Object(Type::FUNCTION_OBJECT),
                _parameters(parameters),
                _body(body),
                _env(env) {
    }

    const std::vector<std::shared_ptr<ast::Identifier>>& parameters() const {
        return _parameters;
    }

    const ast::BlockStatment* body() const {
        return _body.get();
    }

    std::string inspect() const override {
        if (_body == nullptr) {
            return std::string();
        }

        std::string ret = "fn";

        ret.append(1, '(');
        for (size_t i = 0; i < _parameters.size(); ++i) {
            if (i != 0) {
                ret.append(", ");
            }
            ret.append(_parameters[i]->to_string());
        }
        ret.append(") { ");
        ret.append(_body->to_string());
        ret.append(" }");

        return color::cyan + ret + color::off;
    }

    std::shared_ptr<Environment>& env() const {
        return _env;
    }
private:
    std::vector<std::shared_ptr<ast::Identifier>> _parameters;
    std::shared_ptr<ast::BlockStatment> _body;
    mutable std::shared_ptr<Environment> _env;
};

using BuiltinFunction = std::function<std::shared_ptr<object::Object>(const std::vector<std::shared_ptr<object::Object>>&)>;

class Builtin : public Object {
public:
    Builtin(const BuiltinFunction& fn) :
        Object(Type::BUILTIN_OBJECT),
        _fn(fn) {
    }

    std::string inspect() const override {
        return color::cyan + "builtin function" + color::off;
    }

    std::shared_ptr<object::Object> run(const std::vector<std::shared_ptr<object::Object>>& args) const {
        return _fn(args);
    }
private:
    BuiltinFunction _fn;
};

class Array : public Object {
public:
    Array(const std::vector<std::shared_ptr<object::Object>>& elements) :
        Object(Type::ARRAY_OBJECT),
        _elements(elements) {
    }

    Array() :
        Object(Type::ARRAY_OBJECT) {
    }

    std::string inspect() const override {
        std::string ret;
        ret.append(1, '[');
        for (size_t i = 0; i < _elements.size(); ++i) {
            if (i != 0) {
                ret.append(", ");
            }
            ret.append(_elements[i]->inspect());
        }
        ret.append(1, ']');
        return ret;
    }

    const std::vector<std::shared_ptr<object::Object>>& elements() const {
        return _elements;
    }

    void append(const std::shared_ptr<object::Object>& obj) {
        _elements.push_back(obj);
    }
private:
    std::vector<std::shared_ptr<object::Object>> _elements;
};

namespace constants {

extern std::shared_ptr<object::Object> Null;
extern std::shared_ptr<object::Object> True;
extern std::shared_ptr<object::Object> False;

} // namespace constants

} // namespace object
} // namespace autumn
