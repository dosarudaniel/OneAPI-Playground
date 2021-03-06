cmake_minimum_required(VERSION 3.17 FATAL_ERROR)
project(porting-cuda-to-sycl LANGUAGES CXX)

set(CMAKE_C_COMPILER "${SYCL_ROOT}/bin/clang")
set(CMAKE_CXX_COMPILER "${SYCL_ROOT}/bin/clang++")
set(CMAKE_CXX_STANDARD 20)

macro(build_tests TESTS)
  foreach(TEST ${TESTS})
    get_filename_component(TARGET_NAME ${TEST} NAME_WE)
    add_executable(${TARGET_NAME} ${TEST})
    target_include_directories(${TARGET_NAME} PUBLIC
      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/base/inc>
      $<INSTALL_INTERFACE:base>
      )
    #target_compile_features(${TARGET_NAME}  PUBLIC cxx_std_17)
    target_compile_options(${TARGET_NAME}  PUBLIC ${SYCL_FLAGS} ${EXTRA_FLAGS})
    target_link_libraries(${TARGET_NAME}  PUBLIC ${SYCL_FLAGS} ${EXTRA_FLAGS})
    target_include_directories(${TARGET_NAME}  PUBLIC ${SYCL_INCLUDE_DIR})
  endforeach()
endmacro()

macro(add_to_test TESTS)
  foreach(TEST ${TESTS})
    get_filename_component(TARGET_NAME ${TEST} NAME_WE)
    add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
  endforeach()
endmacro()

set(ONEAPI_UNIT_TESTS_BASE
  test1.cpp                    # converted (unmodified) test_ranluxpp.cu, unit test for RANLUX++
  test2.cpp                    # converted (and modified) test_ranluxpp.cu, unit test for RANLUX++
  test3.cpp                    # converted (and modified) cufisher_price_v2.cu example
  test4.cpp                    # simplified cufisher_price_v2.cu example
  test5.cpp                    # empty program, testing clang++ and the CMakeLists.txt
  test6.cpp                    # converted (and modified) test_atomic.cu 
  test7.cpp                    # converted (and modified) test_atomic.cu 
  test8.cpp                    # converted (and modified) test_atomic.cu using oneAtomic.h
  test9.cpp                    # converted and modified  test_track_block.cu
  )

build_tests("${ONEAPI_UNIT_TESTS_BASE}")
add_to_test("${ONEAPI_UNIT_TESTS_BASE}")

