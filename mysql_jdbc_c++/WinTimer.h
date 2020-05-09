#ifndef WINTIMER_H_
#define WINTIMER_H_

#include <Windows.h>

class MyTimer {
public:

        MyTimer() : freq_{ 0 }, start_{ 0 }, stop_{ 0 } {
                QueryPerformanceFrequency(&freq_);
        }

        inline void start(){
                QueryPerformanceCounter(&start_);
        }

        inline void stop(){
                QueryPerformanceCounter(&stop_);
        }

        inline double elapse(){
                return (stop_.QuadPart - start_.QuadPart) / (double)freq_.QuadPart;
        }

        inline long long ticks(){
                return stop_.QuadPart - start_.QuadPart;
        }
private:
        LARGE_INTEGER freq_;
        LARGE_INTEGER start_;
        LARGE_INTEGER stop_;
};


#endif //WINTIMER_H_
