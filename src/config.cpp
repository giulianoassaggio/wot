#include <config.hpp>

#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#define LOG BOOST_LOG_TRIVIAL(info)

#include <electrum.hpp>
#include <disk_db.hpp>

namespace wot {

const std::shared_ptr<Signer> & Config::get_signer() {
  // If not loaded, load the default
  load();
  if(!signer) {
    LOG << "Error: no signer on file";
  }
  return signer;
}

const std::shared_ptr<Verifier> & Config::get_verifier() {
  // If not loaded, load the default
  load();
  if(!verifier) {
    LOG << "Error: no verifier on file";
  }
  return verifier;
}

bool Config::load(const std::string & file) {
  if (init_done) return true;

  std::filesystem::path abs_file;

  if(file == std::string()) {
    std::filesystem::path abs_dir = DiskDb().home_dir() / (std::filesystem::path)DEFAULT_DIR;
    abs_file = DiskDb().home_dir() / DEFAULT_CONFIG_FILE;

    DiskDb::generic_check_or_write_file_with_interaction(abs_dir, abs_file, default_content);
  } else {
    abs_file = file;
    if(!DiskDb::generic_file_exists(file)) {
      std::cerr << "Requested config file " << file << " does not exist" << std::endl;
      return false;
    }
  }

  try {
    config = toml::parse_file((std::string)abs_file);
    LOG << "Loaded config file.";
    if(config["signer"].value<string>().value_or("electrum") == "electrum"){
      signer = std::make_shared<ElectrumSigner>();
    }
    if(config["verifier"].value<string>().value_or("electrum") == "electrum") {
      verifier = std::make_shared<ElectrumVerifier>();
    }
    init_done = true;
    return true;
  }
  catch (const toml::parse_error& err) {
    std::cerr << "Parsing failed: " << err << std::endl;
    return false;
  }
}

bool Config::get_input(std::string & s) {
  try {
    s = std::string(std::istreambuf_iterator<char>(std::cin), {});
    LOG << "Loaded input.";
    return true;
  } catch(...) {
    return false;
  }
}

} // namespace wot
