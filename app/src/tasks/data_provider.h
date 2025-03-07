#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H

#include <vector>
#include "data_observer.h"
#include "soft_assert.h"

namespace tasks {
template <typename T>
class DataProvider {
  private:
    std::vector<DataObserver<T>*> subscribers_{};

  public:
    void NotifyListeners(const T& data);
    void Subscribe(DataObserver<T>* observer);
};

template <typename T>
inline void DataProvider<T>::NotifyListeners(const T& data) {
    for (auto subscriber : subscribers_) {
        subscriber->OnDataReceived(data);
    }
}

template <typename T>
inline void DataProvider<T>::Subscribe(DataObserver<T>* observer) {
    if (!soft_assert(observer != nullptr)) {
        return;
    }
    subscribers_.push_back(observer);
}

}  // namespace tasks

#endif  //DATA_PROVIDER_H