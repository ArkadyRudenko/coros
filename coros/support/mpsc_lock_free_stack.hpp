#pragma once

#include <atomic>
#include <iostream>
#include <optional>

#include <coros/support/forward_list.hpp>

namespace coros::support {

template <typename T>
class MPSCStack {
 public:
  using Node = IntrusiveForwardListNode<T>;

  bool PushIfNotEmpty(Node* node) {
    node->next_ = top_.load();
    while (true) {
      if (node->next_ == nullptr) {
        return false;
      }
      if (top_.compare_exchange_strong(node->next_, node)) {
        return true;
      }
    }
  }

  T* TryPop() {
    Node* curr_top = top_.load();
    while (true) {
      if (curr_top == nullptr || curr_top == &nothing_) {
        return nullptr;
      }
      if (top_.compare_exchange_strong(curr_top, curr_top->next_)) {
        return curr_top->AsItem();
      }
    }
  }

  bool TryPushNothing() {
    Node* null = nullptr;
    return top_.compare_exchange_strong(null, &nothing_);
  }

  bool TryPopNothing() {
    Node* nothing = &nothing_;
    return top_.compare_exchange_strong(nothing, nullptr);
  }

  Node* TakeAll() {
    auto top = top_.load();
    while (true) {
      if (top_.compare_exchange_strong(top, &nothing_)) {
        return top;
      }
    }
  }

  Node* GetPtrNothing() { return &nothing_; }

  bool PushIsFirst(Node* node) {
    // true if stack is full empty
    node->next_ = top_.load();
    Node* ptr = node->next_;
    while (!top_.compare_exchange_strong(node->next_, node)) {
      ptr = node->next_;
    }
    return ptr == nullptr;
  }

  [[nodiscard]] bool IsEmpty() const {
    return top_.load() == nullptr; // TODO
  }

 private:
  std::atomic<Node*> top_{nullptr};
  Node nothing_;
};

}  // namespace coros::support