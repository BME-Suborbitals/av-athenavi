#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H

#include <vector>
#include "data_observer.h"
#include "soft_assert.h"

namespace tasks {
/**
 * @brief Generic data provider implementing the observer pattern
 * 
 * This template class manages a list of observers that receive data updates.
 * 
 * @tparam T The data type that will be distributed to observers
 */
template <typename T>
class DataProvider {
  private:
    std::vector<DataObserver<T>*> subscribers_{};

  public:
    /**
     * @brief Distributes data to all registered observers
     * @param data The data to be distributed to all observers
     */
    void NotifyListeners(const T& data);

    /**
     * @brief Registers a new observer to receive data updates
     * @param observer Pointer to an observer that will receive updates
     */
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