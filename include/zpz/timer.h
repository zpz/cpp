#ifndef _zpz_utilities_timer_h_
#define _zpz_utilities_timer_h_

#include <chrono>

namespace zpz
{

class Timer
{
    using clock = std::chrono::high_resolution_clock;

  public:
    void start()
    {
        _t_start = clock::now();
        _t_stop = _t_start;
        _running = true;
    }

    void stop()
    {
        if (_running) {
            _t_stop = clock::now();
            _running = false;
        }
    }

    long microseconds() const
    {
        if (_running) {
            auto t = clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(t - _t_start).count();
        }
        return std::chrono::duration_cast<std::chrono::microseconds>(_t_stop - _t_start).count();
    }

    double milliseconds() const
    {
        return this->microseconds() / 1000.;
    }

    double seconds() const
    {
        return this->microseconds() / 1000000.;
    }

  private:
    bool _running = false;
    std::chrono::high_resolution_clock::time_point _t_start, _t_stop;
};

} // namespace zpz
#endif // _zpz_utilities_timer_h_
