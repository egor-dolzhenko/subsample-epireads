/*    subsample_epireads: 
 *
 *    Copyright (C) 2013 University of Southern California and
 *                       Egor Dolzhenko
 *                       Andrew D. Smith
 *
 *    Authors: Andrew D. Smith & Egor Dolzhenko
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <utility>

#include "OptionParser.hpp"
#include "smithlab_utils.hpp"
#include "smithlab_os.hpp"
#include "RNG.hpp"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::istringstream;

// Count the number of line in a file. 
size_t
count_lines(const string& file) {
  std::ifstream in(file.c_str());
  if (!in)
    throw SMITHLABException("cannot open input file: " + file);
  
  string dummy_line; 
  size_t num_lines = 0; 

  while (getline(in, dummy_line))
    ++num_lines;
 
  return num_lines;
}


int 
main(int argc, const char **argv) {
  
  try {
    
    bool VERBOSE = false;
    string outfile;
    size_t sample_size = 0;
    
    /****************** COMMAND LINE OPTIONS ********************/
    OptionParser opt_parse(strip_path(argv[0]), "", " <epireads>");
    opt_parse.add_opt("sample", 'n', "sample size", true, sample_size);
    opt_parse.add_opt("output", 'o', "output file name (default: stdout)", 
		      false, outfile);
    opt_parse.add_opt("verbose", 'v', "print more run info", false, VERBOSE);
    vector<string> leftover_args;
    opt_parse.parse(argc, argv, leftover_args);
    if (argc == 1 || opt_parse.help_requested()) {
      cerr << opt_parse.help_message() << endl
	   << opt_parse.about_message() << endl;
      return EXIT_SUCCESS;
    }
    if (opt_parse.about_requested()) {
      cerr << opt_parse.about_message() << endl;
      return EXIT_SUCCESS;
    }
    if (opt_parse.option_missing()) {
      cerr << opt_parse.option_missing_message() << endl;
      return EXIT_SUCCESS;
    }
    if (leftover_args.size() != 1) {
      cerr << opt_parse.help_message() << endl;
      return EXIT_SUCCESS;
    }
    const string epi_file(leftover_args.back());
    /****************** END COMMAND LINE OPTIONS *****************/

    size_t total_epireads = count_lines(epi_file);

    if (VERBOSE) 
      cerr << "There are " << total_epireads << " epireads." << std::endl; 

    vector<size_t> idx(total_epireads);
    for (size_t i = 0; i < total_epireads; ++i)
      idx[i] = i;

    srand(time(0));
    random_shuffle(idx.begin(), idx.end());
    idx.erase(idx.begin() + sample_size, idx.end());
    sort(idx.begin(), idx.end());

    std::ofstream of;
    if (!outfile.empty()) of.open(outfile.c_str());
    std::ostream out(outfile.empty() ? cout.rdbuf() : of.rdbuf());

    std::ifstream in(epi_file.c_str());
    if (!in)
      throw SMITHLABException("cannot open input file: " + epi_file);
  
    string line; 
    size_t index = 0;

    while (getline(in, line)) {
      if (index == idx.front()) {
        out << line << endl;
        idx.erase(idx.begin());
      }
      ++index;
    }
 
  }
  catch (const SMITHLABException &e) {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }
  catch (std::bad_alloc &ba) {
    cerr << "ERROR: could not allocate memory" << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
