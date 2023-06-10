#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

//using namespace std::string_view_literals;

namespace json {

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node : private std::variant<std::nullptr_t, std::string, int, double, bool, Array, Dict>
{
public:
   /* Реализуйте Node, используя std::variant */
    /*1 вариант*/
    using variant::variant;
    using Value = variant;

    /*2 вариант*/
 //   using Value = std::variant<std::nullptr_t, std::string, int, double, bool, Array, Dict>;

//    Node() = default;

    /*конструктор с шаблоном*/
    template <typename T>
    Node(T&& val)
        :Value(val)
    {
    }

    /*конструкторы типов*/
//    конструкторы без шаблона
//    Node(std::nullptr_t)    : Value(nullptr)           {}
//    Node(std::string&& value) : Value(std::move(value))  {}
//    Node(int&& value)         : Value(value)             {}
//    Node(double&& value)      : Value(value)             {}
//    Node(bool&& value)        : Value(value)             {}
//    Node(Array&& array)       : Value(std::move(array))  {}
//    Node(Dict&& map)          : Value(std::move(map))    {}


    const std::string&  AsString()  const;
    int                 AsInt()     const;
    double              AsDouble()  const;
    bool                AsBool()    const;
    const Array&        AsArray()   const;
    const Dict&         AsMap()     const;

    bool IsNull()       const;
    bool IsString()     const;
    bool IsInt()        const;
    bool IsDouble()     const;
    bool IsPureDouble() const;
    bool IsBool()       const;
    bool IsArray()      const;
    bool IsMap()        const;

    const Value& GetValue() const;
    Value& GetValue();


    bool operator==(const Node& rhs) const{
        return GetValue() == rhs.GetValue();
    };
//    bool operator!=(const Node& rhs) const{
//        return !(value_ == rhs.value_);
//    };

//private:
//    Value value_ = nullptr;
};

inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

class Document {
public:
    explicit Document(Node root)    : root_(std::move(root)) {}

    const Node& GetRoot() const
    {
        return root_;
    }


private:
    Node root_;
};

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}

inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

Document Load(std::istream& input);


struct PrintContext
        {
            std::ostream& out;
            int indent_step = 4;
            int indent = 0;

            void PrintIndent() const
            {
                for (int i = 0; i < indent; ++i)
                {
                    out.put(' ');
                }
            }

            PrintContext Indented() const
            {
                return { out, indent_step, indent_step + indent };
            }
        };



void Print(const Document& doc, std::ostream& output);

}  // namespace json
