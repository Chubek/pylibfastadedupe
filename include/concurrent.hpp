#ifndef CONCURRENT_H
#define CONCURRENT_H

#define SLEEP_THREAD std::this_thread::sleep_for(std::chrono::milliseconds(100))

struct MainQueue
{
    std::queue<std::reference_wrapper<Node>> q;
    std::mutex *this_lock;

    MainQueue();

    std::reference_wrapper<Node> pop();
    void push(std::reference_wrapper<Node> s);
    bool isEmpty();
};


void runWorkerThread(MainQueue &queue, bool do_revcomp);
void runBranchThread(MainQueue &queue, const std::vector<std::reference_wrapper<Node>> &clusters, int start, int end);

bool checkThreadNumExceeds(int num_workers, int num_threads);
void assignClustersToThreads(std::vector<std::reference_wrapper<Node>> values, uint32_t num_threads_given, uint32_t num_workers_given, bool do_revcomp);

#endif