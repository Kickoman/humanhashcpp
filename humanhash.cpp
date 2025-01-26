#include "humanhash.h"
#include <cstdint>
#include <ios>
#include <numeric>
#include <stdexcept>
#include <sstream>
#include <random>
#include <functional>

const std::vector<std::string> DEFAULT_WORDLIST = {
    "ack", "alabama", "alanine", "alaska", "alpha", "angel", "apart", "april",
    "arizona", "arkansas", "artist", "asparagus", "aspen", "august", "autumn",
    "avocado", "bacon", "bakerloo", "batman", "beer", "berlin", "beryllium",
    "black", "blossom", "blue", "bluebird", "bravo", "bulldog", "burger",
    "butter", "california", "carbon", "cardinal", "carolina", "carpet", "cat",
    "ceiling", "charlie", "chicken", "coffee", "cola", "cold", "colorado",
    "comet", "connecticut", "crazy", "cup", "dakota", "december", "delaware",
    "delta", "diet", "don", "double", "early", "earth", "east", "echo",
    "edward", "eight", "eighteen", "eleven", "emma", "enemy", "equal",
    "failed", "fanta", "fifteen", "fillet", "finch", "fish", "five", "fix",
    "floor", "florida", "football", "four", "fourteen", "foxtrot", "freddie",
    "friend", "fruit", "gee", "georgia", "glucose", "golf", "green", "grey",
    "hamper", "happy", "harry", "hawaii", "helium", "high", "hot", "hotel",
    "hydrogen", "idaho", "illinois", "india", "indigo", "ink", "iowa",
    "island", "item", "jersey", "jig", "johnny", "juliet", "july", "jupiter",
    "kansas", "kentucky", "kilo", "king", "kitten", "lactose", "lake", "lamp",
    "lemon", "leopard", "lima", "lion", "lithium", "london", "louisiana",
    "low", "magazine", "magnesium", "maine", "mango", "march", "mars",
    "maryland", "massachusetts", "may", "mexico", "michigan", "mike",
    "minnesota", "mirror", "mississippi", "missouri", "mobile", "mockingbird",
    "monkey", "montana", "moon", "mountain", "muppet", "music", "nebraska",
    "neptune", "network", "nevada", "nine", "nineteen", "nitrogen", "north",
    "november", "nuts", "october", "ohio", "oklahoma", "one", "orange",
    "oranges", "oregon", "oscar", "oven", "oxygen", "papa", "paris", "pasta",
    "pennsylvania", "pip", "pizza", "pluto", "potato", "princess", "purple",
    "quebec", "queen", "quiet", "red", "river", "robert", "robin", "romeo",
    "rugby", "sad", "salami", "saturn", "september", "seven", "seventeen",
    "shade", "sierra", "single", "sink", "six", "sixteen", "skylark", "snake",
    "social", "sodium", "solar", "south", "spaghetti", "speaker", "spring",
    "stairway", "steak", "stream", "summer", "sweet", "table", "tango", "ten",
    "tennessee", "tennis", "texas", "thirteen", "three", "timing", "triple",
    "twelve", "twenty", "two", "uncle", "undress", "uniform", "uranus", "utah",
    "vegan", "venus", "vermont", "victor", "video", "violet", "virginia",
    "washington", "west", "whiskey", "white", "william", "winner", "winter",
    "wisconsin", "wolfram", "wyoming", "xray", "yankee", "yellow", "zebra",
    "zulu"
};

std::vector<uint8_t> hexToBytes(const std::string& hex) {
    if (hex.size() % 2 != 0) {
        throw std::invalid_argument("Hexadecimal string should have an even length");
    }
    std::vector<uint8_t> bytes;
    bytes.reserve(hex.size() / 2);
    for (std::string::size_type i = 0; i < hex.size(); i += 2) {
        const uint8_t byte = std::stoi(hex.substr(i, 2), nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

std::vector<uint8_t> compress(const std::vector<uint8_t>& bytes, const size_t target) {
    if (target > bytes.size()) {
        throw std::invalid_argument("Fewer input bytes than requested output");
    }

    const size_t segmentSize = bytes.size() / target;
    std::vector<std::vector<uint8_t>> segments(target);
    for (std::size_t i = 0; i < target; ++i) {
        const auto begin = bytes.begin() + i * segmentSize;
        const auto end = (i == target - 1) ? bytes.end() : begin + segmentSize;
        segments[i] = std::vector<uint8_t>(begin, end);
    }
    std::vector<uint8_t> checksums;
    for (const auto& segment : segments) {
        uint8_t checksum = std::accumulate(segment.begin(), segment.end(), 0, std::bit_xor<uint8_t>());
        checksums.push_back(checksum);
    }
    return checksums;
}

std::string generateUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, 15);

    std::ostringstream oss;
    for (int i = 0; i < 32; ++i) {
        oss << std::hex << dist(gen);
    }
    return oss.str();
}

HumanHasher::HumanHasher()
    : wordlist(DEFAULT_WORDLIST)
{ }

HumanHasher::HumanHasher(const std::vector<std::string>& wordlist)
    : wordlist(wordlist)
{
    if (wordlist.size() != 256) {
        throw std::invalid_argument("Word list should have exactly 256 items");
    }
}

std::string HumanHasher::humanize(const std::string& hexDigest, const size_t words, const std::string& separator) const {
    const auto compressed = compress(
        hexToBytes(hexDigest), words
    );
    std::ostringstream result;
    for (size_t i = 0; i < compressed.size(); ++i) {
        if (i > 0) {
            result << separator;
        }
        result << wordlist[compressed[i]];
    }
    return result.str();
}

HumanHasher::Result HumanHasher::uuid(const size_t words, const std::string& separator) const {
    const auto uuidDigest = generateUUID();
    return {
        .humanReadableId = humanize(uuidDigest, words, separator),
        .uuidDigest = uuidDigest,
    };
}
