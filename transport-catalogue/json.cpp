#include "json.h"
#include <istream>



namespace json {

namespace {

using Number = std::variant<int, double>;

Node LoadNode(std::istream& input);

Node LoadNull(std::istream& input) {
    const std::string nameNull = "null";
    for (size_t i = 0; i < nameNull.size(); i++) {
        if (nameNull.at(i) == input.get()) continue;
        else throw ParsingError("Parsing Nuul failed");
    }
    return Node{nullptr};
}

Node LoadBool(std::istream& input) {

    std::string nameFalse = "false";
    std::string nameTrue = "true";

    char c = input.get();

    bool flag = (c == 't');

    std::string const* name = flag ? &nameTrue : &nameFalse;
    for (size_t i = 1; i < name->size(); i++) {
        if (name->at(i) == input.get()) continue;
        else throw ParsingError("Parsing Bool failed");
    }
    return Node(flag);
}


Node LoadArray(std::istream& input) {
    Array result;

    if (input.peek() == -1) throw ParsingError("Parsing Array failed");

    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    return Node(std::move(result));
}



Node LoadNumber(std::istream& input) {

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream");
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected");
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении
                // код ниже попробует преобразовать строку в double
            }
        }
        //double test = std::stod(parsed_num);
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert " + parsed_num + " to number");
    }
}

std::string LoadString(std::istream& input) {
    using namespace std::literals;

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        }
        else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
            case 'n':
                s.push_back('\n');
                break;
            case 't':
                s.push_back('\t');
                break;
            case 'r':
                s.push_back('\r');
                break;
            case '"':
                s.push_back('"');
                break;
            case '\\':
                s.push_back('\\');
                break;
            default:
                // Встретили неизвестную escape-последовательность
                throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        }
        else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        }
        else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return s;
}

Node LoadDict(std::istream& input) {
    Dict result;
    if (input.peek() == -1) throw ParsingError("Array parsing error");

    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        std::string key = LoadString(input);
        input >> c;
        result.insert({ std::move(key), LoadNode(input) });
    }

    return Node(std::move(result));
}

Node LoadNode(std::istream& input) {
    char c;
    if (!(input >> c)) {
        throw ParsingError("end of file");
    }
    switch (c) {
    case '[':
        return LoadArray(input);
    case '{':
        return LoadDict(input);
    case '"':
        return LoadString(input);
    case 't':
        [[fallthrough]];
    case 'f':
        input.putback(c);
        return LoadBool(input);
    case 'n':
        input.putback(c);
        return LoadNull(input);
    default:
        input.putback(c);
        return LoadNumber(input);
    }
}



}  // namespace


bool Node::IsInt() const {
    return std::holds_alternative<int>(*this);
}

bool Node::IsDouble() const {
    return std::holds_alternative<double>(*this) || std::holds_alternative<int>(*this);
}

bool Node::IsPureDouble() const {
    return std::holds_alternative<double>(*this);
}

bool Node::IsBool() const {
    return std::holds_alternative<bool>(*this);
}

bool Node::IsString() const {
    return std::holds_alternative<std::string>(*this);
}

bool Node::IsNull() const {
    return std::holds_alternative<std::nullptr_t>(*this);
}

bool Node::IsArray() const {
    return std::holds_alternative<Array>(*this);
}

bool Node::IsMap() const {
    return std::holds_alternative<Dict>(*this);
}

int Node::AsInt() const {
    if (!IsInt())
    {
        throw std::logic_error("failed type");
    }

    return std::get<int>(*this);
}

bool Node::AsBool() const {
    if (!IsBool())
    {
        throw std::logic_error("failed type");
    }
    return std::get<bool>(*this);
}

double Node::AsDouble() const {
    if (!IsDouble())
    {
        throw std::logic_error("failed type");
    }

    if (IsInt())
    {
        return static_cast<double>(std::get<int>(*this));
    }

    return std::get<double>(*this);
}

const std::string& Node::AsString() const {
    if (!IsString()) {
        throw std::logic_error("failed type");
    }

    return std::get<std::string>(*this);
}

const Array& Node::AsArray() const {
    if (!IsArray())
    {
        throw std::logic_error("failed type");
    }

    return std::get<Array>(*this);
}

const Dict& Node::AsMap() const {
    if (!IsMap())
    {
        throw std::logic_error("failed type");
    }

    return std::get<Dict>(*this);
}

const Node::Value& Node::GetValue() const {
    return *this;
}

Node::Value &Node::GetValue(){
    return *this;
}

Document Load(std::istream& input) {
    return Document{ LoadNode(input) };
}



void PrintNode(const Node& value, const PrintContext& context);

template <typename Value>
void PrintValue(const Value& value, const PrintContext& context)
{
    context.out << value;
}

void PrintString(const std::string& value, std::ostream& out)
{
    out.put('"');
    for (const char c : value)
    {
        switch (c)
        {
        case '\r':
            out << "\\r"sv;
            break;
        case '\n':
            out << "\\n"sv;
            break;
        case '"':
            [[fallthrough]];
        case '\\':
            out.put('\\');
            [[fallthrough]];
        default:
            out.put(c);
            break;
        }
    }
    out.put('"');
}

template <>
void PrintValue<std::string>(const std::string& value, const PrintContext& context)
{
    PrintString(value, context.out);
}

template <>
void PrintValue<std::nullptr_t>(const std::nullptr_t&, const PrintContext& context)
{
    context.out << "null"sv;
}

template <>
void PrintValue<bool>(const bool& value, const PrintContext& context)
{
    context.out << (value ? "true"sv : "false"sv);
}

template <>
void PrintValue<Array>(const Array& nodes, const PrintContext& context)
{
    std::ostream& out = context.out;
    out << "[\n"sv;
    bool first = true;
    auto in_context = context.Indented();
    for (const Node& node : nodes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            out << ",\n"sv;
        }
        in_context.PrintIndent();
        PrintNode(node, in_context);
    }
    out.put('\n');
    context.PrintIndent();
    out.put(']');
}

template <>
void PrintValue<Dict>(const Dict& nodes, const PrintContext& context)
{
    std::ostream& out = context.out;
    out << "{\n"sv;
    bool first = true;
    auto in_context = context.Indented();
    for (const auto& [key, node] : nodes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            out << ",\n"sv;
        }
        in_context.PrintIndent();
        PrintString(key, context.out);
        out << ": "sv;
        PrintNode(node, in_context);
    }
    out.put('\n');
    context.PrintIndent();
    out.put('}');
}

void PrintNode(const Node& node, const PrintContext& context)
{
    std::visit([&context](const auto& value)
    {
        PrintValue(value, context);
    }, node.GetValue());
}

void Print(const Document& doc, std::ostream& output){
    PrintNode(doc.GetRoot(), PrintContext{ output });
}


}  // namespace json
