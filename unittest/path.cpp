#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include <ot/timer/timer.hpp>

// Function: random_path
std::vector<std::unique_ptr<ot::Path>> random_paths(size_t N) {
  std::default_random_engine gen(0);
  std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
  std::vector<std::unique_ptr<ot::Path>> paths;
  for(size_t i=0; i<N; ++i) {
    paths.emplace_back(std::make_unique<ot::Path>(dist(gen), nullptr));
  }
  return paths;
}

// ------------------------------------------------------------------------------------------------

// Testcase: PathHeap
TEST_CASE("PathHeap") {

  ot::PathHeap heap;
  
  REQUIRE(heap.empty());
  REQUIRE(heap.num_paths() == 0);
  REQUIRE(heap.extract().size() == 0);
  REQUIRE(heap.top() == nullptr);
  
  constexpr size_t N = 100;

  auto paths = random_paths(N); 

  std::vector<float> slacks;
  for(auto& path : paths) {
    slacks.push_back(path->slack);
    heap.push(std::move(path));
  }
  
  REQUIRE(heap.num_paths() == N);

  // Subcase: Pop
  SUBCASE("Pop") {
    std::sort(slacks.begin(), slacks.end(), std::greater<float>());
    for(size_t i=0; i<slacks.size(); ++i) {
      REQUIRE(heap.top()->slack == slacks[i]);
      heap.pop();
    }
  }

  // Subcase: Extract
  SUBCASE("Extract") {
    std::sort(slacks.begin(), slacks.end(), std::less<float>());
    auto P = heap.extract();
    REQUIRE(P.size() == N);
    REQUIRE(heap.num_paths() == 0);
    for(size_t i=0; i<P.size(); ++i) {
      REQUIRE(P[i].slack == slacks[i]);
    }
  }

  // Subcase:: Fit-Pop
  SUBCASE("Fit-Pop") {
    std::sort(slacks.begin(), slacks.end(), std::greater<float>());
    heap.fit(10);
    REQUIRE(heap.num_paths() == 10);
    for(size_t i=0; i<10; ++i) {
      REQUIRE(heap.top()->slack == slacks[N-(10-i)]);
      heap.pop();
    }
  }

  // Subcase: Fit-Extract
  SUBCASE("Fit-Extract") {
    std::sort(slacks.begin(), slacks.end(), std::less<float>());
    heap.fit(10);
    REQUIRE(heap.num_paths() == 10);
    auto P = heap.extract();
    REQUIRE(P.size() == 10);
    REQUIRE(heap.num_paths() == 0);
    for(size_t i=0; i<P.size(); ++i) {
      REQUIRE(P[i].slack == slacks[i]);
    }
  }

  // Subcase: Merge-and-Fit
  SUBCASE("Merge-and-Fit") {
    ot::PathHeap heap2;
    auto paths2 = random_paths(N);
    for(auto& path : paths2) {
      slacks.push_back(path->slack);
      heap2.push(std::move(path));
    }
    heap.merge_and_fit(std::move(heap2), 2*N);
    REQUIRE(heap.num_paths() == 2*N);
    REQUIRE(heap2.num_paths() == 0);

    std::sort(slacks.begin(), slacks.end(), std::less<float>());
    auto P = heap.extract();
    REQUIRE(P.size() == 2*N);
    
    for(size_t i=0; i<P.size(); ++i) {
      REQUIRE(P[i].slack == slacks[i]);
    }
  }
  
}

// Testcase: SetupClockEdges
TEST_CASE("SetupClockEdges") {

  std::string launch_cell = "DFFPOSX1";
  std::string capture_cell = "DFFNEGX1";
  float capture_time = 0.3f;

  SUBCASE("RisingToFalling") {
  }
  SUBCASE("RisingToRising") {
    capture_cell = "DFFPOSX1";
    capture_time = 0.6f;
  }
  SUBCASE("FallingToFalling") {
    launch_cell = "DFFNEGX1";
    capture_time = 0.9f;
  }
  SUBCASE("FallingToRising") {
    launch_cell = "DFFNEGX1";
    capture_cell = "DFFPOSX1";
    capture_time = 0.6f;
  }

  ot::Timer timer;
  timer.read_celllib(std::filesystem::path(OT_HOME) / "example/simple/osu018_stdcells.lib")
       .insert_primary_input("clk")
       .insert_gate("launch", launch_cell)
       .insert_gate("data_buf", "BUFX2")
       .insert_gate("capture", capture_cell)
       .connect_pin("launch:CLK", "clk")
       .connect_pin("capture:CLK", "clk")
       .insert_net("q")
       .connect_pin("launch:Q", "q")
       .connect_pin("data_buf:A", "q")
       .insert_net("n")
       .connect_pin("data_buf:Y", "n")
       .connect_pin("capture:D", "n")
       .create_clock("clk", "clk", 0.6f);

  for(auto [el, rf] : ot::SPLIT_TRAN) {
    timer.set_at("clk", el, rf, rf == ot::RISE ? 0.0f : 0.3f)
         .set_slew("clk", el, rf, 0.12f);
  }
  timer.update_timing();

  for(auto rf : ot::TRAN) {
    auto rat = timer.report_rat("capture:D", ot::MAX, rf);
    REQUIRE(rat);
    for(const auto& test : timer.tests()) {
      if(test.constrained_pin().name() == "capture:D") {
        auto constraint = test.constraint(ot::MAX, rf);
        REQUIRE(constraint);
        CHECK(*rat + *constraint == doctest::Approx(capture_time));
      }
    }
  }

  if(launch_cell == "DFFPOSX1" && capture_cell == "DFFNEGX1") {
    // Independent interpolation of the bundled Liberty tables, in ns.
    const float required[] = {0.107971644f, 0.108279201f};
    const float slack[] = {-0.076951335f, -0.148981526f};
    for(auto rf : ot::TRAN) {
      CHECK(*timer.report_rat("capture:D", ot::MAX, rf) == doctest::Approx(required[rf]));
      CHECK(*timer.report_slack("capture:D", ot::MAX, rf) == doctest::Approx(slack[rf]));
      CHECK(*timer.report_tns(ot::MAX, rf) == doctest::Approx(slack[rf]));
    }
    auto paths = timer.report_timing(1, ot::MAX);
    REQUIRE(paths.size() == 1);
    CHECK(paths.front().slack == doctest::Approx(slack[ot::FALL]));
  }
}









