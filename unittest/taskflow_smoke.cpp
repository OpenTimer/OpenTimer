#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include <taskflow/taskflow.hpp>
#include <ot/timer/timer.hpp>

TEST_CASE("Taskflow and OpenTimer share the bundled executor") {
  tf::Executor executor(2);
  tf::Taskflow tasks;
  int value = 0;
  auto first = tasks.emplace([&] { value = 40; });
  auto second = tasks.emplace([&] { value += 2; });
  first.precede(second);

  ot::Timer timer;
  timer.set_num_threads(1);
  timer.insert_primary_input("in");
  timer.set_at("in", ot::MAX, ot::RISE, 3.0f);

  executor.run(tasks).get();
  timer.update_timing();
  CHECK(value == 42);
  REQUIRE(timer.report_at("in", ot::MAX, ot::RISE).has_value());
  CHECK(*timer.report_at("in", ot::MAX, ot::RISE) == 3.0f);

  timer.set_at("in", ot::MAX, ot::RISE, 5.0f).update_timing();
  executor.run(tasks).get();
  CHECK(value == 42);
  REQUIRE(timer.report_at("in", ot::MAX, ot::RISE).has_value());
  CHECK(*timer.report_at("in", ot::MAX, ot::RISE) == 5.0f);
}
