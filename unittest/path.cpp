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









