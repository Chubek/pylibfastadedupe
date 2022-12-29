#define SLEEP_THREAD    this_thread::sleep_for(chrono::milliseconds(100))

struct MainQueue {
    queue<vector<Seq&>> q;
    mutex this_lock;

    vector<Seq&> pop() {
        const std::lock_guard<mutex> lock(this_lock);
        vector<Seq&> s = q.front();
        q.pop();

        return s;
    };

    void push(vector<Seq&> s) {
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
        WorkerThread(MainQueue& queue): queue(queue) {}
        void run();
};


class BranchThread {
    public:
        MainQueue& queue;
        vector<vector<Seq&>> clusters;
        BranchThread(vector<vector<Seq&>> clusters, MainQueue& queue): clusters(clusters), queue(queue) {}
        void run();
};

void assignClustersToThreads(HashMap<Seq&, vector<Seq&>> hashmap, uint32_t num_threads, uint32_t num_workers);