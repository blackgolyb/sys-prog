#include "buffer.h"
#include "utils.h"

struct ConsumerArgs {
  const int id;
  const int ms_timeout_shutdown;
  const int work_emulation_time;
  ThreadSafeBuffer<int>* buffer;

  ConsumerArgs(int id, int ms_timeout_shutdown, int work_emulation_time,  ThreadSafeBuffer<int>* buffer)
      : id(id), ms_timeout_shutdown(ms_timeout_shutdown), work_emulation_time(work_emulation_time), buffer(buffer) {}
};


struct ConsumerResult {
  const int id;
  const int consumed;

  ConsumerResult(int id, int consumed)
      : id(id), consumed(consumed){}
};

void *consumer(void *row_arg);
