#pragma once

#include <iostream>

namespace coros::support {

template <typename T>
struct IntrusiveForwardListNode {

  using Node = IntrusiveForwardListNode;

  IntrusiveForwardListNode* next_{nullptr};

  Node* Next() { return next_; }

  void SetNext(Node* node) noexcept { next_ = node; }

  void ResetNext() noexcept { SetNext(nullptr); }

  void LinkBefore(Node* node) noexcept { SetNext(node); }

  T* AsItem() noexcept { return static_cast<T*>(this); }
};

template <typename T>
class IntrusiveForwardList {
  using Node = IntrusiveForwardListNode<T>;

 public:
  IntrusiveForwardList() = default;

  // Non-copyable
  IntrusiveForwardList(const IntrusiveForwardList&) = delete;
  IntrusiveForwardList& operator=(const IntrusiveForwardList&) = delete;

  // Move-constructable
  IntrusiveForwardList(IntrusiveForwardList&& that) {
    head_ = that.head_;
    tail_ = that.tail_;
    size_ = that.size_;
    that.Reset();
  }

  IntrusiveForwardList& operator=(IntrusiveForwardList&& that) = delete;

  void PushBack(Node* node) noexcept {
    ++size_;

    node->next_ = nullptr;

    if (IsEmpty()) {
      head_ = tail_ = node;
    } else {
      tail_->next_ = node;
      tail_ = node;
    }
  }

  T* PopFront() {
    if (IsEmpty()) {
      return nullptr;
    }

    --size_;
    Node* front = head_;

    if (head_ == tail_) {
      head_ = tail_ = nullptr;
    } else {
      head_ = head_->next_;
    }

    front->next_ = nullptr;

    return front->AsItem();
  }

  // O(1)
  void Append(IntrusiveForwardList& that) noexcept {
    if (that.IsEmpty()) {
      return;
    }

    if (IsEmpty()) {
      head_ = that.head_;
      tail_ = that.tail_;
      size_ = that.size_;
    } else {
      tail_->next_ = that.head_;
      tail_ = that.tail_;
      size_ += that.size_;
    }

    that.Reset();
  }

  [[nodiscard]] bool IsEmpty() const noexcept { return head_ == nullptr; }

  ~IntrusiveForwardList() {
    if (!IsEmpty()) {
      std::cerr << "List is not empty";
      std::abort();
    }
  }

 private:
  void Reset() noexcept {
    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
  }

 private:
  Node* head_{nullptr};
  Node* tail_{nullptr};
  size_t size_{0};
};

}  // namespace coros::support