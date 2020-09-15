#include "ac_search.hpp"
#include "debug.hpp"

#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
                bin.push_back(c);
            } else
                bin.push_back(*it);
        }
        patterns.push_back(std::move(bin));
    }
    return patterns;
}

bool search(ac::automaton& automaton, std::istream& is, std::string_view file)
{
    bool found = false;
    int pos = 0;
    ac::matcher matcher(automaton, 
                        [file, pos](size_t i) {
                            std::cout << file << ' ' << pos << ' ' << i <<
                                std::endl;
                        });
    for (char c; is.get(c); ++pos)
        if (matcher.step(c))
            found = true;
    return found;
}

void usage(char* argv0)
{
    std::cerr << "usage: " << argv0 << " pattern_file [file ...]" << std::endl;
}

int main(int argc, char* argv[])
{
    std::ios_base::sync_with_stdio(false);
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    int status = EXIT_FAILURE;
    try {
        auto patterns = read_patterns(argv[1]);
        ac::automaton automaton(patterns.begin(), patterns.end());
        if (argc == 2) {
            if (search(automaton, std::cin, "stdin"))
                status = EXIT_SUCCESS;
        } else {
            for (int a = 2; a < argc; ++a) {
                std::ifstream ifs(argv[a]);
                ifs.exceptions(ifs.failbit);
                ifs.exceptions(ifs.badbit);
                if (search(automaton, ifs, argv[a]))
                    status = EXIT_SUCCESS;
            }
        }
    } catch (std::exception& exc) {
        std::cerr << "Exception: " << exc.what() << std::endl;
        status = EXIT_FAILURE;
    }
    return status;
}
