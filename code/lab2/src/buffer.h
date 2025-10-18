#ifndef THREAD_SAFE_BUFFER_H
#define THREAD_SAFE_BUFFER_H

#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <stack>
#include <pthread.h>
#include <unistd.h>

class LockManager {
 public:
  LockManager() {
    pthread_mutex_init(&mutex_, nullptr);
    pthread_cond_init(&cond_not_full_, nullptr);
    pthread_cond_init(&cond_not_empty_, nullptr);
  }

  ~LockManager() {
    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&cond_not_full_);
    pthread_cond_destroy(&cond_not_empty_);
  }

  class Guard {
   public:
    explicit Guard(LockManager& parent) : parent_(parent) {
      pthread_mutex_lock(&parent_.mutex_);
      parent_.in_transaction_ = true;
    }
    ~Guard() {
      parent_.in_transaction_ = false;
      pthread_mutex_unlock(&parent_.mutex_);
    }

   private:
    LockManager& parent_;
  };

  Guard transaction() { return Guard(*this); }

  void lock() {
    if (!in_transaction_) pthread_mutex_lock(&mutex_);
  }

  void release() {
    if (!in_transaction_) pthread_mutex_unlock(&mutex_);
  }

  void signal_not_empty() { pthread_cond_signal(&cond_not_empty_); }
  void signal_not_full() { pthread_cond_signal(&cond_not_full_); }

  int wait_not_empty() { return pthread_cond_wait(&cond_not_empty_, &mutex_); }

  int wait_not_empty(const struct timespec& ts) {
    return pthread_cond_timedwait(&cond_not_empty_, &mutex_, &ts);
  }

  int wait_not_full() { return pthread_cond_wait(&cond_not_full_, &mutex_); }

 private:
  bool in_transaction_ = false;
  pthread_mutex_t mutex_;
  pthread_cond_t cond_not_full_;
  pthread_cond_t cond_not_empty_;
};

template<typename T>
class ThreadSafeBuffer {
 public:
  explicit ThreadSafeBuffer(size_t max_size) : max_size_(max_size) {}

  typename LockManager::Guard transaction() { return locker.transaction(); }

  void write(const T& item) {
    locker.lock();
    while (buf_.size() >= max_size_) {
      locker.wait_not_full();
    }
    buf_.push(item);
    locker.signal_not_empty();
    locker.release();
  }

  T read() {
    locker.lock();
    while (buf_.empty()) {
      locker.wait_not_empty();
    }
    T item = buf_.top();
    buf_.pop();
    locker.signal_not_full();
    locker.release();
    return item;
  }

  std::optional<T> read(long timeout_ms) {
    locker.lock();
    struct timespec ts = init_timeout_(timeout_ms);
    while (buf_.empty()) {
      int rc = locker.wait_not_empty(ts);
      if (rc == ETIMEDOUT) {
        locker.release();
        return std::nullopt;
      }
    }
    T item = buf_.top();
    buf_.pop();
    locker.signal_not_full();
    locker.release();
    return item;
  }

  size_t size() {
    locker.lock();
    size_t s = buf_.size();
    locker.release();
    return s;
  }

 private:
  struct timespec init_timeout_(long timeout_ms) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
      ts.tv_sec++;
      ts.tv_nsec -= 1000000000;
    }
    return ts;
  }

  std::stack<T> buf_;
  size_t max_size_;
  LockManager locker;
};

#endif // THREAD_SAFE_BUFFER_H
