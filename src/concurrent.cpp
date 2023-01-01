#include "../include/fastadedupe.hpp"

void assignClustersToThreads(std::vector<std::reference_wrapper<Node>> values, uint32_t num_threads_given, uint32_t num_workers_given, bool do_revcomp)
{
    int size = values.size();
    int num_threads = num_threads_given;
    int num_workers = num_workers_given;

    if (size < num_threads_given)
    {
        num_threads = size;
    }

    if (size < num_workers_given)
    {
        num_workers = size;
    }

    auto _ = checkThreadNumExceeds(num_workers, num_threads);

    MainQueue queue;
    std::vector<std::thread> main_threads_objs;
    std::vector<std::thread> worker_threads_objs;

    int added = 0;
    for (int i = 0; i < size; i += num_threads)
    {
        int end = size - added < num_threads ? i + (size - added) : i + num_threads;

        main_threads_objs.push_back(std::thread(runBranchThread, std::ref(queue), std::cref(values), i, end));

        added++;
    }

    for (int i = 0; i < num_workers; i++)
    {
        worker_threads_objs.push_back(std::thread(runWorkerThread, std::ref(queue), do_revcomp));
    }

    std::for_each(worker_threads_objs.begin(), worker_threads_objs.end(), [](std::thread &wrk)
                  { wrk.join(); });
    std::for_each(main_threads_objs.begin(), main_threads_objs.end(), [](std::thread &brch)
                  { brch.join(); });
}

bool checkThreadNumExceeds(int num_workers, int num_threads)
{
    std::cout << "Your CPU supports " << std::thread::hardware_concurrency() << " concurrent hardware threads.";
    std::cout << "Started with an overall " << (num_threads + num_workers) << " virtual threads.";

    return true;
}

void runWorkerThread(MainQueue &queue, bool do_revcomp)
{
    SLEEP_THREAD;

    while (!queue.isEmpty())
    {
        auto curr = queue.pop();
        auto curr_seqs = curr.get();

        for (int i = 0; i < curr_seqs.seq_vec.size(); i++)
        {
            Sequence &lead = curr_seqs.seq_vec[i].getSelf();
            if (lead.is_dup)
                continue;

        set_skip:
            bool skip = false;

            for (int j = i + 1; j < curr_seqs.seq_vec.size(); j++)
            {
                if (skip)
                    goto set_skip;

                Sequence &candidate = curr_seqs.seq_vec[j].getSelf();
                if (candidate.is_dup)
                    continue;

                skip = candidate.compareHammingWith(lead);
                if (do_revcomp)
                {
                    skip = candidate.revCompcompareHammingWith(lead);
                }
            }
        }
    }
}

void runBranchThread(MainQueue &queue, const std::vector<std::reference_wrapper<Node>> &clusters, int start, int end)
{
    SLEEP_THREAD;

    for (int i = start; i < end; i++)
    {
        auto seqs = clusters.at(i);
        queue.push(seqs);
    }
}

MainQueue::MainQueue()
{
    this_lock = new std::mutex();
}

std::reference_wrapper<Node> MainQueue::pop()
{
    this_lock->lock();
    std::reference_wrapper<Node> s = q.front();
    q.pop();
    this_lock->unlock();

    return s;
};

void MainQueue::push(std::reference_wrapper<Node> s)
{
    this_lock->lock();
    q.push(s);
    this_lock->unlock();
}

bool MainQueue::isEmpty()
{
    return q.empty();
}
