#pragma once

#include <memory>
#include <optional>
#include <queue>

#include "json.h"

namespace json {

class Builder

{
public:

    class BaseContext;
    class DictContext;
    class ArrayContext;
    class ValueContext;
    class KeyContext;
    class StartContainersContext;


    Builder();

    KeyContext Key(std::string key);
    Builder& Value(Node::Value value);

    DictContext StartDict();
    Builder& EndDict();

    ArrayContext StartArray();
    Builder& EndArray();

    Node& Build();

private:
    [[nodiscard]] bool CheckAvaibalityAddNode() const;

    void AddNode(Node top_node);
    Node root_{nullptr};
    std::vector<std::unique_ptr<Node>> nodes_stack_;
};

class Builder::BaseContext {
public:
    explicit BaseContext(Builder& builder);

protected:
    Builder& builder_;
};

class Builder::StartContainersContext : public Builder::BaseContext {
public:
    explicit StartContainersContext(Builder& builder);
    Builder::ArrayContext StartArray();
    Builder::DictContext StartDict();
};


class Builder::KeyContext : public StartContainersContext {

public:
    explicit KeyContext(Builder& builder);
    ValueContext Value(Node::Value value);

};

class Builder::ValueContext : public BaseContext {

public:
    explicit ValueContext(Builder& builder);
    KeyContext Key(std::string key);
    Builder& EndDict();

};

class Builder::DictContext : public BaseContext {

public:
    explicit DictContext(Builder& builder);
    KeyContext Key(std::string key);
    Builder& EndDict();

};

class Builder::ArrayContext : public StartContainersContext {

public:
    explicit ArrayContext(Builder& builder);
    ArrayContext Value(Node::Value value);
    Builder& EndArray();

};


}  // namespace json
