#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

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
        return std::to_string(_value);
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
        return _value ? "true" : "false";
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
        return format(R"("{}")", _value) ;
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
        return "null";
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
        return "\x1b[1;31merror: \x1b[0m" + _message;
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
        ret.append(") {");
        ret.append(_body->to_string());
        ret.append("}");

        return ret;
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
        return "builtin function";
    }

    std::shared_ptr<object::Object> run(const std::vector<std::shared_ptr<object::Object>>& args) const {
        return _fn(args);
    }
private:
    BuiltinFunction _fn;
};

} // namespace object
} // namespace autumn
