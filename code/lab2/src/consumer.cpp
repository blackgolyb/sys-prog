#include "consumer.h"

void *consumer(void *row_arg) {
  auto arg = (ConsumerArgs *)row_arg;
  auto buffer = arg->buffer;
  auto timeout = arg->ms_timeout_shutdown;
  int consumed_count = 0;

  while (true) {
    int item;
    size_t size;
    {
        auto tx = buffer->transaction();
        auto res = buffer->read(timeout);
        if (!res.has_value()) break;
        item = res.value();
        size = buffer->size();
    }

    consumed_count++;
    print_consumer_tag(arg->id);
    printf("consumed %d (buf_size=%zu, total_consumed=%d)\n", item, size, consumed_count);

    // work emulation
    if (arg->work_emulation_time){
        int max = (int)(arg->work_emulation_time * 1.3f);
        int min = (int)(arg->work_emulation_time * 0.7f);
        msleep(randint(min, max));
    }
  }

  print_consumer_tag(arg->id);
  printf("finished\n");
  return new ConsumerResult(arg->id, consumed_count);
}
