#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {

        friend class SingleLinkedList;
        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node) {
            node_ = node;
        }

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        BasicIterator& operator++() noexcept {
            if (node_) {
                node_ = node_->next_node;
            }
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto old_iter = *this;
            if (node_) {
                node_ = node_->next_node;
            }
            return old_iter;
        }

        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    //шаблонный метод копирования списка с итераторами begin & end
    template <typename Conteiner>
    SingleLinkedList<Type> SingleLinkedListCopy(Conteiner& container) {
        SingleLinkedList<Type> list;
        try {
            for (const auto& value : container) {
                list.PushBack(value);
            }
        }
        catch (...) {
            list.Clear();
            throw;
        }
        return list;
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        SingleLinkedList<Type> list = SingleLinkedListCopy(values);
        swap(list);
    }

    SingleLinkedList(const SingleLinkedList& other) {
        SingleLinkedList<Type> list = SingleLinkedListCopy(other);
        swap(list);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        SingleLinkedList<Type> list = SingleLinkedListCopy(rhs);
        swap(list);
        return *this;
    }

    SingleLinkedList()
        : size_(0)
    {}

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] Node* GetHead() const noexcept {
        return &head_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* new_node;
        try {
            new_node = new Node(value, pos.node_->next_node);
            pos.node_->next_node = new_node;
        }
        catch (...) {
            throw;
        }
        size_++;
        return Iterator(new_node);
    }

    void PopFront() noexcept {
        Node* old_iter = head_.next_node;
        head_.next_node = head_.next_node->next_node;
        --size_;
        delete old_iter;
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node* old_iter = pos.node_->next_node;
        pos.node_->next_node = pos.node_->next_node->next_node;
        delete old_iter;
        --size_;
        return Iterator(pos.node_->next_node);
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        size_++;
    }

    void PushBack(const Type& value) {
        Node* iter = &head_;
        while (iter->next_node != nullptr) {
            iter = iter->next_node;
        }
        iter->next_node = new Node(value, nullptr);
        size_++;
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* next_node_old = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete next_node_old;
        }
        size_ = 0;
    }

    void swap(SingleLinkedList& other) noexcept {
        Node* buf_node = head_.next_node;
        int buf_size = size_;

        head_.next_node = other.head_.next_node;
        size_ = other.GetSize();

        other.head_.next_node = buf_node;
        other.size_ = buf_size;
    }

    ~SingleLinkedList() {
        Clear();
    }

private:
    Node head_;
    size_t size_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
