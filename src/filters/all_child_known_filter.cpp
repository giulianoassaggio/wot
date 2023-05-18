#include <iostream>

#include <filter.hpp>

#include <graph.hpp>

FILTER_START(AllChildKnownFilter)
  FILTER_DESC("every linked identity is present with a node in internal db")
  FILTER_LONG_DESC("check if all the keys linked from this node are present in our database")
  FILTER_TOKENS(0)
  bool check(const NodeBase & n) const override {
    auto gv = GraphView();
    for(auto const & child : n.get_trust()) {
      if(!gv.contains(child.get_to())) {
        return false;
      }
    }
    return(true);
  }
FILTER_END()