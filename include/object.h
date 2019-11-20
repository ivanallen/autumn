#pragma once

#include <memory>
#include <string>

namespace autumn {
namespace object {

class Object {
public:
    enum Type {
        INTEGER,
        BOOLEAN,
        NULL_OBJECT,
        RETURN_OBJECT,
    };

    Object(Type type) : _type(type) {}

    Type type() const {
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
            Object(Object::Type::INTEGER),
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
            Object(Object::Type::BOOLEAN),
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

class Null : public Object {
public:
    Null() : Object(Object::Type::NULL_OBJECT) {
    }

    std::string inspect() const override {
        return "null";
    }
};

class ReturnValue: public Object {
public:
    ReturnValue(std::shared_ptr<Object>& value) :
        Object(Object::Type::RETURN_OBJECT),
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

} // namespace object
} // namespace autumn
