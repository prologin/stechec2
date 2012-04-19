#ifndef LIB_UTILS_OPTIONS_HH_
# define LIB_UTILS_OPTIONS_HH_

# include <string>
# include <boost/program_options.hpp>

# define VERSION_MESSAGE(name, version) \
    "Stechec2 " name " v" version "\n" \
    "Copyright (c) 2012 Prologin\n\n" \
    "Written by Nicolas Hureau <nh@kalenz.fr>\n" \
    "       and Pierre Bourdon <delroth@gmail.com>"

namespace utils {

class Options // abstract
{
public:
    virtual void process(int argc, char** argv) = 0;

protected:
    void process_cli(boost::program_options::options_description opt,
                     boost::program_options::variables_map& vm,
                     int argc, char** argv);
    void process_config(boost::program_options::options_description opt,
                        boost::program_options::variables_map& vm,
                        const std::string& config_file);

protected:
    std::string usage_;
    std::string version_;
};

} // namespace utils

#endif // !LIB_UTILS_OPTIONS_HH_
