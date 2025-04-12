#include <iostream>
#include <vector>
#include <cmath>

#include "base_opts.hpp"

class ProgramOptions : public BaseOptions
{
public:
  int jk_level = 1;
  int jk_type = 0;

  ProgramOptions() = default;

  // Calling virtual functions in the constructor is unsafe
  // add_prog_options() won't be dispatched to the derived class.
  ProgramOptions(int, char **) = delete;

protected:
  void add_prog_options(cxxopts::Options &options) override
  {
    // clang-format off
    options.add_options("program params")
    ("jk_level", "jackknife block level. block=level^3", def_opt(jk_level))
    ("jk_type", "0: spaced resampling, 1: random resampling", def_opt(jk_type));
    // clang-format on
  }

  void parse_prog_results(const cxxopts::ParseResult &result) override
  {
    jk_level = result["jk_level"].as<decltype(jk_level)>();
    jk_type = result["jk_type"].as<decltype(jk_type)>();
  }
};

int main(int argc, char **argv)
{
  ProgramOptions opt;
  opt.parse_arguments(argc, argv);

  std::string base_file = opt.input_prefix + ".0" + opt.h5_suffix;

  float mvir_min, mvir_max;
  mvir_min = std::pow(10.0, opt.mrange[0]);
  mvir_max = std::pow(10.0, opt.mrange[1]);

  int nr = opt.nr;
  float rmin = opt.rrange[0];
  float rmax = opt.rrange[1];
  bool log_bin = opt.log_bin;

  int jk_level = opt.jk_level;
  if(jk_level < 1) jk_level = 1;
  const int jk_block = jk_level * jk_level * jk_level;

  auto nmesh = opt.nmesh;

  std::cout << "# input prefix " << opt.input_prefix << std::endl;
  std::cout << "# base file " << base_file << std::endl;
  std::cout << "# output filename " << opt.output_filename << std::endl;
  std::cout << "# Mmin, Mmax " << mvir_min << ", " << mvir_max << std::endl;
  std::cout << "# Rmin, Rmax, NR " << rmin << ", " << rmax << ", " << nr << std::endl;
  std::cout << "# log_bin " << std::boolalpha << log_bin << std::endl;
  std::cout << "# FFT mesh " << nmesh << "^3" << std::endl;
  std::cout << "# jackknife block " << jk_block << std::endl;

  return EXIT_SUCCESS;
}
