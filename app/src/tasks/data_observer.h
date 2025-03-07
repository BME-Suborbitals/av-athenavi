#ifndef DATA_OBSERVER_H
#define DATA_OBSERVER_H

namespace tasks {
template <typename T>
class DataObserver {
  public:
    virtual void OnDataReceived(const T& data) = 0;
};
}  // namespace tasks

#endif  //DATA_OBSERVER_H