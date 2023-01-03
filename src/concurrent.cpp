#include "../include/fastadedupe.hpp"

void assignClustersToThreads(HashMap &map, uint32_t num_threads_given, uint32_t num_workers_given, bool do_revcomp)
{
    std::cout << "Starting main operation...\n";

    int size = map.hashes_list.size();
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

    std::vector<MainQueue> queues_main(num_workers);
    std::vector<std::reference_wrapper<MainQueue>> queues;

    for (int i = 0; i < num_workers; i++) {
        std::reference_wrapper<MainQueue> q_ref = std::reference_wrapper<MainQueue>(queues_main[i]);
        queues.push_back(q_ref);
    }

    std::vector<std::thread> main_threads_objs;
    std::vector<std::thread> worker_threads_objs;

    int added = 0;
    for (int i = 0; i < size; i += num_threads)
    {
        int end = size - added < num_threads ? i + (size - added) : i + num_threads;

        main_threads_objs.push_back(std::thread(runBranchThread, std::ref(queues), std::cref(map), i, end));

        added++;
    }

    for (int i = 0; i < num_workers; i++)
    {
        worker_threads_objs.push_back(std::thread(runWorkerThread, queues[i], map.ref_seqs, do_revcomp));
    }
    
    SLEEP_THREAD;

    std::cout << "\nJoining worker threads...\n";
    std::for_each(worker_threads_objs.begin(), worker_threads_objs.end(), [](std::thread &wrk)
                  { wrk.join(); std::cout << "=";  });
    std::cout << "\nJoining main threads\n";
    std::for_each(main_threads_objs.begin(), main_threads_objs.end(), [](std::thread &brch)
                  { brch.join(); std::cout << "="; });
    std::cout << "\n";
}   

bool checkThreadNumExceeds(int num_workers, int num_threads)
{
    std::cout << "\nYour CPU supports " << std::thread::hardware_concurrency() << " concurrent hardware threads.\n";
    std::cout << "\nStarted with an overall " << (num_threads + num_workers) << " virtual threads.\n";

    return true;
}

void runWorkerThread(std::reference_wrapper<MainQueue> queue_ref, std::vector<std::reference_wrapper<Sequence>> ref_seqs, bool do_revcomp)
{
    SLEEP_THREAD;

    auto queue = queue_ref.get();

    while (!queue.isEmpty())
    {
        auto curr = queue.pop();
        
        for (int i = 0; i < curr.size(); i++)
        {
            auto ref_lead = ref_seqs[curr[i]];
            Sequence &lead = ref_lead.get().getSelf();
            if (lead.is_dup)
                continue;

        set_skip:
            bool skip = false;

            for (int j = i + 1; j <  curr.size(); j++)
            {
                if (skip)
                    goto set_skip;

                auto ref_candidate = ref_seqs[curr[j]];
                Sequence &candidate = ref_candidate.get().getSelf();
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

void runBranchThread(std::vector<std::reference_wrapper<MainQueue>> &queues, const HashMap &map, int start, int end)
{   
    int max_size = queues.size();
    int j = 0;


    for (int i = start; i < end; i++)
    {
        if (j >= max_size) j = 0;

        std::vector<seqsize_t> curr_seqs;
        auto curr_bucket = map.map.at(map.hashes_list[i]);

        queues[j++].get().push(curr_bucket);
    }
}

MainQueue::MainQueue()
{
    this_lock = new std::mutex();
}

std::vector<seqsize_t> MainQueue::pop()
{
    this_lock->lock();
    std::vector<seqsize_t> s = q.front();
    q.pop();
    this_lock->unlock();

    return s;
};

void MainQueue::push(std::vector<seqsize_t> s)
{
    this_lock->lock();
    q.push(s);
    this_lock->unlock();
}

bool MainQueue::isEmpty()
{
    return q.empty();
}
