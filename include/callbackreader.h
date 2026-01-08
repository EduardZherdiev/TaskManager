#pragma once
#include <vector>
#include <utility>
#include "callback.h"

class DBProcessing;

class CallbackReader {
public:
    CallbackReader();
    std::pair<bool, std::vector<Callback>> requestCallbackBrowse();
    ~CallbackReader();

private:
    std::unique_ptr<DBProcessing> m_dbProcessor;
};
