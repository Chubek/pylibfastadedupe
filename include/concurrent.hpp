#ifndef CONCURRENT_H
#define CONCURRENT_H

#define SLEEP_THREAD    this_thread::sleep_for(chrono::milliseconds(100))

struct MainQueue {
    queue<vector<Seq&>&> q;
    mutex this_lock;

    vector<Seq&>& pop() {
        const std::lock_guard<mutex> lock(this_lock);
        vector<Seq&> s = q.front();
        q.pop();

        return s;
    };

    void push(vector<Seq&>& s) {
        const std::lock_guard<mutex> lock(this_lock);
        q.push(s); 
    }

    bool isEmpty() {
        const std::lock_guard<mutex> lock(this_lock);
        return q.empty();
    }

};

class WorkerThread {
    public:
        MainQueue& queue;        
        bool do_revcomp;
        WorkerThread(MainQueue& queue, bool do_revcomp): queue(queue), do_revcomp(do_revcomp) {}
        
        void run() {
            SLEEP_THREAD;

            while(!queue.isEmpty()) {
                vector<Seq&> curr_seqs = queue.pop();

                for (int i = 0; i < curr_seqs.size(); i++) {
                    Seq& lead = curr_seqs[i];            
                    if (lead.is_dup) continue;

                    set_skip:
                    bool skip = false;

                    for (int j = i + 1; j < curr_seqs.size(); j++) {
                        if (skip) goto set_skip;

                        Seq& candidate = curr_seqs[j];
                        if (candidate.is_dup) continue;

                        skip = candidate.compareHammingWith(lead);                
                        if (do_revcomp) {
                            skip = candidate.revCompcompareHammingWith(lead);
                        }
                    }
                }                
            }
        }

        thread spwanThread() {
            return thread([this] { this->run(); });
        }
};


class BranchThread {
    public:
        MainQueue& queue;
        vector<vector<Seq&>&> clusters;
        BranchThread(vector<vector<Seq&>&> clusters, MainQueue& queue): clusters(clusters), queue(queue) {}
        
        void run() {
            SLEEP_THREAD;

            for (int i = 0; i < clusters.size(); i++) {
                vector<Seq&>& seqs = clusters[i];
                queue.push(seqs);
            }
        }

        thread spwanThread() {
            return thread([this] { this->run(); });
        }
};

bool checkThreadNumExceeds(int num_workers, int num_threads);
void assignClustersToThreads(HashMap<Seq&, vector<Seq&>, Seq&> hashmap, uint32_t num_threads, uint32_t num_workers, bool do_revcomp);

#endif