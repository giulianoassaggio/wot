#pragma once

#include <string>

#include <boost/program_options.hpp>

#include <command.hpp>
#include <db_nodes.hpp>

COMMAND_START(LsCommand)
  COMMAND_CLI("ls")
  COMMAND_SHORT_DESCRIPTION("List of the objects in the internal db filtered against the provided filters.")
  COMMAND_SYNOPSIS("wot [--jsonl] [--rule-filter RULE] [--to-filter TO] [--from-filter FROM] [...] ls")
  COMMAND_DESCRIPTION(R"(
  List of the objects of the internal db, filtered against the provided filters.

  If option "--jsonl" is present, the list will be printed to stdout in a jsonl
  way (one json object per line). This is suitable for exporting the whole db.

  Beware that the objects exported can have a hash or signature that is of the
  original form, so the db will not be verifiable.

  Link filters: the objects are first filtered, then are listed as full objects.
  This means that if a single link of an object pass the filter, then all the
  links are shown, because the whole object is shown.
)")

  bool act(const vm_t & vm) const override {
    if(vm.count("jsonl")) {
      Db_nodes().filtered_export(vm);
      return true;
    } else {
      Db_nodes().list_nodes(vm);
      return true;
    }
  }
COMMAND_END()
