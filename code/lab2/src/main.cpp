#include <alloca.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "producer.h"
#include "consumer.h"


constexpr size_t MAX_BUF = 5;
constexpr int PRODUCERS = 3;
constexpr int PRODUCER_N_MSGS_MIN = 8;
constexpr int PRODUCER_N_MSGS_MAX = 12;
constexpr int CONSUMERS = 3;
constexpr int CONSUMER_TIMEOUT_MS = 3000;
constexpr int CONSUMER_WORK_EMULATION_TIME = 600;

pthread_t * createConsumers(ThreadSafeBuffer<int>* buf) {
    auto consumerArgsFactory = [buf](int id) {
      return new ConsumerArgs(id, CONSUMER_TIMEOUT_MS, CONSUMER_WORK_EMULATION_TIME, buf);
    };
    pthread_t *consumers =
        create_n_threads(CONSUMERS, consumer, consumerArgsFactory);
    assert(consumers != NULL);
    return consumers;
}



pthread_t * createProducers(ThreadSafeBuffer<int>* buf) {
    auto producerArgsFactory = [buf](int id) {
      return new ProducerArgs(id, randint(PRODUCER_N_MSGS_MIN, PRODUCER_N_MSGS_MAX), buf);
    };
    pthread_t *producers =
        create_n_threads(PRODUCERS, producer, producerArgsFactory);
    assert(producers != NULL);
    return producers;
}


int finishProducers(pthread_t * producers) {
    int total_produced = 0;
    for (int i = 0; i < PRODUCERS; ++i){
        void* ret;
        pthread_join(producers[i], &ret);
        auto res = static_cast<ProducerResult*>(ret);
        total_produced += res->produced;
        delete res;
    }
    return total_produced;
}

int finishConsumers(pthread_t * consumers) {
    int total_consumed = 0;
    for (int i = 0; i < CONSUMERS; ++i){
        void* ret;
        pthread_join(consumers[i], &ret);
        auto res = static_cast<ConsumerResult*>(ret);
        total_consumed += res->consumed;
        delete res;
    }
    return total_consumed;
}

int main() {
  srand((unsigned)time(nullptr));

  ThreadSafeBuffer<int>* buf = new ThreadSafeBuffer<int>(MAX_BUF);

  pthread_t *producers = createProducers(buf);
  pthread_t *consumers = createConsumers(buf);

  int produced = finishProducers(producers);
  int consumed = finishConsumers(consumers);

  printf("All done. produced=%d consumed=%d\n", produced, consumed);
  return 0;
}
