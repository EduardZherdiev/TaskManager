#pragma once
#include <vector>
#include <utility>
#include "Feedback.h"

class DBProcessing;

class FeedbackReader {
public:
    FeedbackReader();
    std::pair<bool, std::vector<Feedback>> requestFeedbackBrowse();
    ~FeedbackReader();

private:
};
