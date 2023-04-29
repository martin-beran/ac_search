#include "ac_search.hpp"
#include "debug.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

namespace sc = std::chrono;

class stopwatch {
    using clock = sc::steady_clock;
    using float_sec = sc::duration<double>;
public:
    stopwatch(): t0(clock::now()) {}
    void stop() {
        d = sc::duration_cast<float_sec>(clock::now() - t0);
    }
    std::string speed(double val) {
        std::ostringstream s;
        s << std::fixed << std::setprecision(3) << (val / d.count());
        return s.str();
    }
private:
    clock::time_point t0;
    float_sec d{};
    friend std::ostream& operator<<(std::ostream& os, const stopwatch& sw) {
        std::ostringstream s;
        s << std::fixed << std::setprecision(3) << sw.d.count();
        os << s.str() << "s";
        return os;
    }
};

std::vector<std::string> read_patterns(std::string_view file)
{
    std::vector<std::string> patterns;
    std::ifstream ifs(file.data());
    ifs.exceptions(ifs.failbit | ifs.badbit);
    ifs.exceptions(ifs.exceptions() & ~ifs.failbit);
    int n_line = 0;
    for (std::string line; std::getline(ifs, line);) {
        ++n_line;
        int n_char = 0;
        std::string bin;
        for (auto it = line.begin(); it != line.end(); ++it) {
            ++n_char;
            auto invalid = [file, n_line, n_char]() {
                std::ostringstream os;
                os << file << ':' << n_line << ':' << n_char <<
                    ": Invalid pattern character";
                throw std::runtime_error(os.str());
            };
            if (*it == '\\') {
                int c = 0;
                for (int d = 0; d <= 1; ++d) {
                    if (++it == line.end())
                        invalid();
                    ++n_char;
                    if (*it == '\\') {
                        c = *it;
                        break;
                    }
                    c *= 16;
                    if (*it >= '0' && *it <='9')
                        c += *it - '0';
                    else if (*it >= 'A' && *it <= 'F')
                        c += *it - 'A' + 10;
                    else if (*it >= 'a' && *it <= 'f')
                        c += *it - 'a' + 10;
                    else
                        invalid();
                }
                bin.push_back(char(c));
            } else
                bin.push_back(*it);
        }
        patterns.push_back(std::move(bin));
    }
    return patterns;
}

void search(ac::automaton<>& automaton, std::istream& is)
{
    long n_match = 0;
    ac::matcher matcher(automaton,
        [&n_match](ac::automaton<>::index_type) { ++n_match; }
    );
    std::string data;
    stopwatch t_read_data;
    for (char c; is.get(c);)
        data.push_back(c);
    t_read_data.stop();
    std::cout << "data_size=" << data.size() << " read_data=" << t_read_data <<
        std::endl;
    stopwatch t_match;
    for (char c: data)
        matcher.step(c);
    t_match.stop();
    std::cout << "matches=" << n_match << " time=" << t_match <<
        " bytes/s=" << t_match.speed(double(data.size())) <<  std::endl;
}

void usage(char* argv0)
{
    std::cerr << "usage: " << argv0 << " pattern_file [file] [threshold]" <<
        std::endl;
}

} // namespace

int main(int argc, char* argv[])
{
    std::ios_base::sync_with_stdio(false);
    if (argc < 2 || argc > 4) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    int status = EXIT_FAILURE;
    try {
        ptrdiff_t threshold =
            argc > 3 ? std::stoll(argv[3]) : ac::automaton<>::threshold_default;
        stopwatch t_total;
        stopwatch t_read_patterns;
        auto patterns = read_patterns(argv[1]);
        t_total.stop();
        size_t pat_sz = 0;
        for (auto&& p: patterns)
            pat_sz += p.size();
        std::cout << "patterns=" << patterns.size() <<
            " patterns_size=" << pat_sz <<
            " read_patterns=" << t_total << std::endl;
        stopwatch t_build_automaton;
        ac::automaton automaton(patterns.begin(), patterns.end(), threshold);
        t_build_automaton.stop();
        std::cout << "automaton_size=" << automaton.size() <<
            " nodes=" << automaton.nodes() <<
            " full_size=" << automaton.full_size() <<
            " build_automaton=" << t_build_automaton << std::endl;
        if (argc == 2)
            search(automaton, std::cin);
        else {
            std::ifstream ifs(argv[2]);
            ifs.exceptions(ifs.failbit);
            ifs.exceptions(ifs.badbit);
            search(automaton, ifs);
        }
        t_total.stop();
        std::cout << "total_time=" << t_total << std::endl;
        status = EXIT_SUCCESS;
    } catch (std::exception& exc) {
        std::cerr << "Exception: " << exc.what() << std::endl;
        status = EXIT_FAILURE;
    }
    return status;
}
