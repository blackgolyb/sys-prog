#include "buffer.h"
#include "utils.h"

struct ProducerArgs {
    const int id;
    const int number_of_messages;
    ThreadSafeBuffer<int>* buffer;

    ProducerArgs(int id, int number_of_messages, ThreadSafeBuffer<int>* buffer)
        : id(id), number_of_messages(number_of_messages), buffer(buffer) {}
};

struct ProducerResult {
  const int id;
  const int produced;

  ProducerResult(int id, int produced)
      : id(id), produced(produced){}
};

void *producer(void *row_arg);
