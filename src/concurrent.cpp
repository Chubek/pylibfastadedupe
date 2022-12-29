#include "../include/fastadedupe.hpp"


void WorkerThread::run() {
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

                skip = candidate.compareHammingWith(lead);                
            }
        }
        
    }
}

void BranchThread::run() {
    SLEEP_THREAD;

    for (vector<Seq&> seqs: clusters) {
        queue.push(seqs);
    }
}