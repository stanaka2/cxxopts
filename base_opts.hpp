#pragma once

#include "include/cxxopts.hpp"

/* helper functions */
// clang-format off
template <typename T, template <typename...> class Template>
struct is_specialization : std::false_type {};
template <template <typename...> class Template, typename... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};
// clang-format on

template <typename T>
std::enable_if_t<is_specialization<T, std::vector>::value, std::string> val_to_string(const T &vec)
{
  std::ostringstream oss;
  for(size_t i = 0; i < vec.size(); ++i) {
    if(i > 0) oss << ",";
    oss << vec[i];
  }
  return oss.str();
}

std::string val_to_string(const std::string &val) { return val; }
std::string val_to_string(const bool &val) { return val ? "true" : "false"; }

template <typename T>
std::enable_if_t<!is_specialization<T, std::vector>::value && !std::is_same<T, bool>::value &&
                      !std::is_same<T, std::string>::value,
                  std::string>
val_to_string(const T &val)
{
  return std::to_string(val);
}

class BaseOptions
{
public:
  std::string input_prefix = "None";
  std::string output_filename = "./output.dat";

  bool log_bin = false;
  int nk = 100;
  std::vector<float> krange = {1e-2, 50.0};
  int nr = 100;
  std::vector<float> rrange = {1.0, 150.0};

  int nmesh = 1024;
  int p_assign = 3;

  std::vector<float> mrange = {1.0, 20.0};

  bool verbose = false;

  // constant values
  const std::string h5_suffix = ".h5";
  // const std::string h5_suffix = ".hdf5";

  BaseOptions() = default;

  // Calling virtual functions in the constructor is unsafe
  // add_prog_options() won't be dispatched to the derived class.
  BaseOptions(int, char **) = delete;

  template <typename T>
  auto def_opt(const T &val)
  {
    return cxxopts::value<T>()->default_value(val_to_string(val));
  }

  void parse_arguments(int argc, char *argv[])
  {
    cxxopts::Options options(argv[0], "Base options");

    // clang-format off
    options.add_options("IO params")
    ("i,input_prefix", "Input prefix", def_opt(input_prefix))
    ("o,output_filename", "Output filename", def_opt(output_filename));

    options.add_options("bins params")
    ("nk", "number of k-bins", def_opt(nk))
    ("k,krange", "kmin and kmax", def_opt(krange))
    ("nr", "number of r-bins", def_opt(nr))
    ("r,rrange", "rmin and rmax", def_opt(rrange))
    ("log_bin", "use log_bin", def_opt(log_bin));

    options.add_options("mesh params")
    ("n,nmesh", "number of FFT mesh size", def_opt(nmesh))
    ("p,p_assign", "particle assign type", def_opt(p_assign));

    options.add_options("halo params")
    ("m,mrange", "minimum halo mass (log10)", def_opt(mrange));

    options.add_options("other params")
    ("v,verbose", "verbose output", def_opt(verbose))
    ("h,help", "Print help");
    // clang-format on

    add_prog_options(options);

    auto result = options.parse(argc, argv);

    if(result.count("help")) {
      std::cout << options.help() << std::endl;
      std::exit(EXIT_SUCCESS);
    }

    input_prefix = result["input_prefix"].as<decltype(input_prefix)>();
    output_filename = result["output_filename"].as<decltype(output_filename)>();

    nmesh = result["nmesh"].as<decltype(nmesh)>();
    p_assign = result["p_assign"].as<decltype(p_assign)>();

    nr = result["nr"].as<decltype(nr)>();
    nk = result["nk"].as<decltype(nk)>();

    krange = result["krange"].as<decltype(krange)>();
    rrange = result["rrange"].as<decltype(rrange)>();
    mrange = result["mrange"].as<decltype(mrange)>();

    verbose = result["verbose"].as<bool>();

    parse_prog_results(result);
  }

protected:
  virtual void add_prog_options(cxxopts::Options &) {  }
  virtual void parse_prog_results(const cxxopts::ParseResult &) { }
};
