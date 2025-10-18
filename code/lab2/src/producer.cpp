#include "producer.h"


void *producer(void *row_arg) {
    auto arg = (ProducerArgs*)row_arg;
    int produced_count = 0;

    while (produced_count < arg->number_of_messages) {
        int item = randint(1000);

        arg->buffer->write(item);
        produced_count++;

        print_producer_tag(arg->id);
        printf("produced %d (buf_size=%zu, produced=%d/%d)\n",
               item, arg->buffer->size(), produced_count, arg->number_of_messages);

        msleep(randint(100, 300));
    }

    print_producer_tag(arg->id);
    printf("finished\n");
    return new ProducerResult(arg->id, produced_count);
}
