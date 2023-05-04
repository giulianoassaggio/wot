#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <commands/add_command.hpp>
#include <commands/compose_command.hpp>

#include <string>
#include <node.hpp>
#include <identity.hpp>
#include <disk_db.hpp>

namespace {
std::string dummy_content =
R"(# Web of trust configuration file

# Possible algoritms for public key and signature accepted
#"nostr" will be here
algos = [ "dummy" ]

signer = "electrum"

verifier = "electrum"
)";
} // namespace

using namespace wot;

BOOST_AUTO_TEST_SUITE(AddCommandTest)

BOOST_AUTO_TEST_CASE(check) {

  BOOST_TEST_MESSAGE("Writing a config test file");
  DiskDb::generic_write_file("/tmp/test_file_config.toml",dummy_content);
  BOOST_CHECK(Config::get().load("/tmp/test_file_config.toml"));

  Identity i("aa");
  BOOST_CHECK( i.is_well_formed() == true );

  namespace po = boost::program_options;

  vm_t vm;

  vm.emplace("serial",po::variable_value((int)7, true));

  vm.emplace("circle",po::variable_value((std::string)"friends", true));

  vm.emplace("to",po::variable_value((std::string)"to", true));
  vm.emplace("unit",po::variable_value((std::string)"USD", true));
  vm.emplace("value",po::variable_value((int)50, true));
  vm.emplace("since",po::variable_value((int)8, true));

  vm.emplace("implementation",po::variable_value((std::string)"0.1", true));

  nlohmann::json j;

  ComposeCommand().fill_with(vm, j);
  ComposeCommand().complete(j);

  Node n(j);

  // Check what happens here - TODO
  //BOOST_CHECK( Db_nodes().add_node(n) );

  std::string signed_object = R"({"circle":"friends","implementation":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"d473f6f1e829932b68fb1d6889e6f12df4daafc4fba0a4be2510c3b6dc3b7c08","sig":"dummy-sig-for-d473f6f1e829932b68fb1d6889e6f12df4daafc4fba0a4be2510c3b6dc3b7c08"},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}]})";

  // Start from the artifact that arrives from sign test
  Node m(signed_object);
  // Verify the new node
  BOOST_CHECK( m.verify_node(/*force-accept-hash=*/false,/*force-accept-sig=*/false) );

  BOOST_CHECK( Db_nodes().add_node(m) );

  // We should check the output of the following, that should be == to signed_object
  DiskDb((std::string)"").print("d473f6f1e829932b68fb1d6889e6f12df4daafc4fba0a4be2510c3b6dc3b7c08");

  vm_t bitcoin_vm;
  bitcoin_vm.emplace("bitcoin-key-filter",po::variable_value((std::string)"1", true));
  BOOST_CHECK( ! Db_nodes().add_node(m, bitcoin_vm) );

  std::string not_signed_object = R"({"circle":"friends","implementation":"0.1","profile":{"about":"","aka":"","dob":"","facebook":"","key":"dummy-key","name":"","nostr":"","picture":"","telegram":"","twitter":""},"serial":7,"signature":{"hash":"1234","sig":"dummy-sig-for-1234"},"sources":[],"trust":[{"on":[],"since":8,"to":"to","unit":"USD","value":50}]})";
  Node m2(not_signed_object);
  BOOST_CHECK( m2.verify_node(/*force-accept-hash=*/true,/*force-accept-sig=*/true) );

  BOOST_CHECK( !Db_nodes().add_node(m2, bitcoin_vm) );

  vm_t empty_vm;
  Node m3(not_signed_object);
  BOOST_CHECK( m3.verify_node(/*force-accept-hash=*/true,/*force-accept-sig=*/true) );
  BOOST_CHECK( Db_nodes().add_node(m3, empty_vm) );

  BOOST_TEST_MESSAGE("Removing the config test file");
  DiskDb::generic_remove_file("/tmp/test_file_config.toml");
}

BOOST_AUTO_TEST_SUITE_END()
