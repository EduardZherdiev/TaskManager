#pragma once
#include <vector>
#include <memory>
#include "task.h"
#include <utility>


class DBProcessing;

class TaskReader
{
public:
    TaskReader();
    std::pair<bool,
              std::vector<Task>> requestTaskBrowse();
    ~TaskReader();

private:
    std::unique_ptr<DBProcessing> m_dbProcessor;
};
