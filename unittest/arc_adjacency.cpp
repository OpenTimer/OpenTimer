// This file is distributed under the MIT License. See LICENSE for details.
#include <gtest/gtest.h>
#include <ot/timer/arc.hpp>
#include <ot/timer/net.hpp>
#include <ot/timer/pin.hpp>

namespace {
std::vector<ot::Arc *> contents(const ot::detail::Arc_list &list) {
  return {list.begin(), list.end()};
}

TEST(ArcAdjacency, EraseAndReinsertPreserveOrderAndOtherIterators) {
  ot::Pin from("from"), to("to");
  ot::Net net("net");
  ot::Arc a(from, to, net), b(from, to, net), c(from, to, net);
  ot::detail::Arc_link la, lb, lc;
  ot::detail::Arc_list list;
  EXPECT_TRUE(list.empty());
  EXPECT_EQ(list.begin(), list.end());
  list.push_back(la, a);
  list.push_back(lb, b);
  list.push_back(lc, c);
  EXPECT_EQ(contents(list), (std::vector<ot::Arc *>{&a, &b, &c}));
  EXPECT_EQ(list.size(), 3u);
  auto kept = list.begin();
  ++kept;
  const auto &value = *kept;
  list.erase(
      la); // head removal must leave another iterator and its reference valid
  EXPECT_EQ(*kept, &b);
  EXPECT_EQ(value, &b);
  list.push_back(la, a);
  EXPECT_EQ(contents(list), (std::vector<ot::Arc *>{&b, &c, &a}));
  list.erase(lc); // interior removal
  EXPECT_EQ(contents(list), (std::vector<ot::Arc *>{&b, &a}));
  EXPECT_EQ(*kept++, &b);
  EXPECT_EQ(*kept, &a);
  list.erase(la); // tail removal
  EXPECT_EQ(contents(list), (std::vector<ot::Arc *>{&b}));
  list.erase(lb); // singleton removal
  EXPECT_TRUE(list.empty());
  EXPECT_EQ(list.begin(), list.end());
  EXPECT_EQ(list.size(), 0u);
  list.push_back(lc, c);
  EXPECT_EQ(contents(list), (std::vector<ot::Arc *>{&c}));
  list.erase(lc);
}

TEST(ArcAdjacency, EndpointLinksRemainIndependentIncludingSelfLoops) {
  ot::Pin pin("pin");
  ot::Net net("net");
  ot::Arc arc(pin, pin, net);
  ot::detail::Arc_link input, output;
  ot::detail::Arc_list fanin, fanout;
  fanin.push_back(input, arc);
  fanout.push_back(output, arc);
  auto retained = fanout.begin();
  fanin.erase(input);
  EXPECT_TRUE(fanin.empty());
  EXPECT_EQ(fanout.size(), 1u);
  EXPECT_EQ(*retained, &arc);
  fanin.push_back(input, arc);
  fanout.erase(output);
  EXPECT_EQ(contents(fanin), (std::vector<ot::Arc *>{&arc}));
  EXPECT_TRUE(fanout.empty());
  fanin.erase(input);
}
} // namespace
