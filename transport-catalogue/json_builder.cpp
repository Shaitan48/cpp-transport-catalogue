#include "json_builder.h"

namespace json {

BaseContext::BaseContext(Builder& builder) : builder_(builder) {}

StartContainersContext::StartContainersContext(Builder& builder) : BaseContext(builder) {}

ArrayContext& StartContainersContext::StartArray() {
    return builder_.StartArray();
}

DictContext& StartContainersContext::StartDict() {
    return builder_.StartDict();
}

KeyContext::KeyContext(Builder& builder) : StartContainersContext(builder) {}

ValueContext KeyContext::Value(Node::Value value) {
    return builder_.Value(std::move(value));
}

ValueContext::ValueContext(Builder& builder) : BaseContext(builder) {}

KeyContext& ValueContext::Key(std::string key) {
    return builder_.Key(std::move(key));
}

Builder& ValueContext::EndDict() {
    return builder_.EndDict();
}

DictContext::DictContext(Builder& builder) : BaseContext(builder) {}

KeyContext& DictContext::Key(std::string key) {
    return builder_.Key(std::move(key));
}

Builder& DictContext::EndDict() {
    return builder_.EndDict();
}

ArrayContext::ArrayContext(Builder& builder) : StartContainersContext(builder) {}

ArrayContext& ArrayContext::Value(Node::Value value) {
    return builder_.Value(std::move(value));
}

Builder& ArrayContext::EndArray() {
    return builder_.EndArray();
}

Builder::Builder() : KeyContext(*this), ValueContext(*this), DictContext(*this), ArrayContext(*this) {}

KeyContext& Builder::Key(std::string key) {
    if (!root_.IsNull() || nodes_stack_.empty() || !nodes_stack_.back()->IsMap())
        throw std::logic_error("Error add Key" + key);

    nodes_stack_.emplace_back(std::make_unique<Node>(std::move(key)));
    return *this;
}

Builder& Builder::Value(Node::Value value) {
    if (!root_.IsNull() || !TryAddingNode())
        throw std::logic_error("Error add Value");

    std::visit([this](auto&& v) { nodes_stack_.emplace_back(std::make_unique<Node>(v)); }, value);
    AddNode(*nodes_stack_.back().release());
    return *this;
}

DictContext& Builder::StartDict() {
    if (!root_.IsNull() || !TryAddingNode())
        throw std::logic_error("Error StartDict");

    nodes_stack_.emplace_back(std::make_unique<Node>(Dict()));
    return *this;
}

Builder& Builder::EndDict() {
    if (!root_.IsNull() || nodes_stack_.empty() || !nodes_stack_.back()->IsMap())
        throw std::logic_error("Error EndDict");

    AddNode(*nodes_stack_.back().release());
    return *this;
}

ArrayContext& Builder::StartArray() {
    if (!root_.IsNull() || !TryAddingNode())
        throw std::logic_error("Error StartArray");

    nodes_stack_.emplace_back(std::make_unique<Node>(Array()));
    return *this;
}

Builder& Builder::EndArray() {
    if (!root_.IsNull() || nodes_stack_.empty() || !nodes_stack_.back()->IsArray())
        throw std::logic_error("Error EndArray");

    AddNode(*nodes_stack_.back().release());
    return *this;
}

Node& Builder::Build() {
    if (root_.IsNull() || !nodes_stack_.empty())
        throw std::logic_error("Error JSON");

    return root_;
}

bool Builder::TryAddingNode() const {
    return nodes_stack_.empty() || nodes_stack_.back()->IsArray() || nodes_stack_.back()->IsString();
}

void Builder::AddNode(Node top_node) {
    nodes_stack_.pop_back();

    if (nodes_stack_.empty()) {
        root_ = top_node;
    } else if (nodes_stack_.back()->IsArray()) {
        std::get<Array>(nodes_stack_.back()->GetValue()).emplace_back(std::move(top_node));
    } else if (nodes_stack_.back()->IsString()) {
        std::string key = std::get<std::string>(nodes_stack_.back()->GetValue());
        nodes_stack_.pop_back();
        std::get<Dict>(nodes_stack_.back()->GetValue()).emplace(std::move(key), std::move(top_node));
    } else {
                if (top_node.IsMap()) {
            nodes_stack_.back()->GetValue() = Dict();
        } else if (top_node.IsArray()) {
            nodes_stack_.back()->GetValue() = Array();
        }
    }
}

}  // namespace json
