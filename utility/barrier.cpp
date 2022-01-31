

#include <condition_variable>
#include <mutex>

class MyBarrier{
     public:
        explicit MyBarrier(std::size_t counter) : 
        workers(counter), 
        actual_workers(counter), 
        curr_gen(0) {
        }

        void dec(){
            mutex.lock();

            workers--;
            if(--actual_workers<=0){
                curr_gen++;
                actual_workers = workers;
                mutex.unlock();
                cv.notify_all();
            }
            else{

                mutex.unlock();
            }
          
        }
        void setCounter(std::size_t counter){
             workers = counter;
             actual_workers = counter;
             curr_gen = 0;
        }
        void wait(int id) {
            std::unique_lock<std::mutex> lock(mutex);
            auto lGen = curr_gen;
            
            if (--actual_workers<=0) {
                
                curr_gen++;
                actual_workers = workers;
               
             
                cv.notify_all();
            } else {
                cv.wait(lock, [this, lGen] { return lGen != curr_gen; });
              
               
            }
        }

private:
    std::mutex mutex;
    std::condition_variable cv;
    std::size_t workers;
    std::size_t actual_workers;
    std::size_t curr_gen;
};