#include <iostream>
#include <chrono>


#define START(timename) auto timename = std::chrono::system_clock::now();
#define STOP(timename,elapsed)  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - timename).count();


void delay(std::chrono::microseconds m)
{
    auto active_wait = [](std::chrono::microseconds us)
    {
        long usecs = us.count();
        auto start = std::chrono::high_resolution_clock::now();
        auto end = false;
        while (!end)
        {
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            if (usec > usecs)
                end = true;
        }
        return;
    };
    active_wait(m);
    return;
}

class utimer {
  std::chrono::system_clock::time_point start;
  std::chrono::system_clock::time_point stop;
  std::string message; 
  using usecs = std::chrono::microseconds;
  using msecs = std::chrono::milliseconds;
  bool debug;

private:
  long * us_elapsed;
  
public:

  utimer(const std::string m, bool debug) : message(m),us_elapsed((long *)NULL),debug(debug) {
    start = std::chrono::system_clock::now();
  }
    
  utimer(const std::string m, long * us, bool debug) : message(m),us_elapsed(us),debug(debug) {
    start = std::chrono::system_clock::now();
  }

  ~utimer() {
    stop =
      std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed =
      stop - start;
    auto musec =
      std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    if(debug){
      //PROBLEM WITH PRINTS
      //std::cout << message << " computed in %ld usec " << std::endl;
      std::printf("%s computed in %ld usec ", message.c_str(), musec);
      std::cout << std::endl;
      if(us_elapsed != NULL)
      (*us_elapsed) = musec;
    }
      
  }
};
