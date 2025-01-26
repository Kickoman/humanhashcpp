#pragma once
#include <vector>
#include <string>


class HumanHasher {
public:
    struct Result {
        std::string humanReadableId;
        std::string uuidDigest;
    };

    HumanHasher();
    explicit HumanHasher(const std::vector<std::string>& wordlist);

    std::string humanize(const std::string& hexDigest, const size_t words = 4, const std::string& separator = "-") const;
    Result uuid(const size_t words = 4, const std::string& separator = "-") const;

private:
    const std::vector<std::string> wordlist;
};
