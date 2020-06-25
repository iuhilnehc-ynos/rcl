// Copyright 2020 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include "rcl/rcl.h"
#include "rcl/log_level.h"
#include "rcl/error_handling.h"
#include "rcutils/logging.h"

#include "./arg_macros.hpp"

int setup_and_parse_log_level_args(const char * log_level_string)
{
  rcl_arguments_t local_arguments = rcl_get_zero_initialized_arguments();
  const char * local_argv[] = {"process_name", "--ros-args", "--log-level", log_level_string};
  unsigned int local_argc = (sizeof(local_argv) / sizeof(const char *));
  return rcl_parse_arguments(
    local_argc, local_argv, rcl_get_default_allocator(), &local_arguments);
}

TEST(TestLogLevel, error_log_level) {
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("=debug"));
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("debug,"));
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl=debug,"));
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl=debug,,"));
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl="));
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl=,"));
}

#define GET_LOG_LEVEL_FROM_ARGUMENTS(log_level, ...) \
  { \
    rcl_ret_t ret; \
    rcl_arguments_t local_arguments = rcl_get_zero_initialized_arguments(); \
    const char * local_argv[] = {__VA_ARGS__}; \
    unsigned int local_argc = (sizeof(local_argv) / sizeof(const char *)); \
    ret = rcl_parse_arguments( \
      local_argc, local_argv, rcl_get_default_allocator(), &local_arguments); \
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
    ret = rcl_arguments_get_log_level(&local_arguments, &log_level); \
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
    EXPECT_TRUE(log_level != NULL); \
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT( \
    { \
      ASSERT_EQ(RCL_RET_OK, rcl_arguments_fini(&local_arguments)); \
    }); \
  }

TEST(TestLogLevel, no_log_level) {
  rcl_log_level_t * log_level = NULL;
  GET_LOG_LEVEL_FROM_ARGUMENTS(log_level, "process_name");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_log_level_fini(log_level);
  });
  EXPECT_EQ(-1, log_level->default_log_level);
  EXPECT_EQ(static_cast<size_t>(0), log_level->num_loggers);
}

TEST(TestLogLevel, default_log_level) {
  rcl_log_level_t * log_level = NULL;
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_level, "process_name", "--ros-args",
    "--log-level", "debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_log_level_fini(log_level);
  });
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_DEBUG), log_level->default_log_level);
  EXPECT_EQ(static_cast<size_t>(0), log_level->num_loggers);
}

TEST(TestLogLevel, logger_log_level_debug) {
  rcl_log_level_t * log_level = NULL;
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_level, "process_name", "--ros-args",
    "--log-level", "rcl=debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_log_level_fini(log_level);
  });
  EXPECT_EQ(-1, log_level->default_log_level);
  EXPECT_EQ(static_cast<size_t>(1), log_level->num_loggers);
  EXPECT_STREQ("rcl", log_level->logger_settings[0].name);
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_DEBUG), log_level->logger_settings[0].level);
}

TEST(TestLogLevel, logger_log_level_info) {
  rcl_log_level_t * log_level = NULL;
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_level, "process_name", "--ros-args",
    "--log-level", "rcl=info");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_log_level_fini(log_level);
  });
  EXPECT_EQ(-1, log_level->default_log_level);
  EXPECT_EQ(static_cast<size_t>(1), log_level->num_loggers);
  EXPECT_STREQ("rcl", log_level->logger_settings[0].name);
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_INFO), log_level->logger_settings[0].level);
}

TEST(TestLogLevel, default_log_level_with_logger) {
  rcl_log_level_t * log_level = NULL;
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_level, "process_name", "--ros-args",
    "--log-level", "debug,rcl=debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_log_level_fini(log_level);
  });
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_DEBUG), log_level->default_log_level);
  EXPECT_EQ(static_cast<size_t>(1), log_level->num_loggers);
  EXPECT_STREQ("rcl", log_level->logger_settings[0].name);
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_DEBUG), log_level->logger_settings[0].level);
}

TEST(TestLogLevel, logger_with_default_log_level) {
  rcl_log_level_t * log_level = NULL;
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_level, "process_name", "--ros-args",
    "--log-level", "rcl=debug,debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_log_level_fini(log_level);
  });
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_DEBUG), log_level->default_log_level);
  EXPECT_EQ(static_cast<size_t>(1), log_level->num_loggers);
  EXPECT_STREQ("rcl", log_level->logger_settings[0].name);
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_DEBUG), log_level->logger_settings[0].level);
}

TEST(TestLogLevel, multiple_log_level_with_default_at_front) {
  rcl_log_level_t * log_level = NULL;
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_level, "process_name", "--ros-args",
    "--log-level", "debug", "--log-level", "rcl=debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_log_level_fini(log_level);
  });
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_DEBUG), log_level->default_log_level);
  EXPECT_EQ(static_cast<size_t>(1), log_level->num_loggers);
  EXPECT_STREQ("rcl", log_level->logger_settings[0].name);
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_DEBUG), log_level->logger_settings[0].level);
}

TEST(TestLogLevel, multiple_log_level_with_default_at_back) {
  rcl_log_level_t * log_level = NULL;
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_level, "process_name", "--ros-args",
    "--log-level", "rcl=debug", "--log-level", "debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_log_level_fini(log_level);
  });
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_DEBUG), log_level->default_log_level);
  EXPECT_EQ(static_cast<size_t>(1), log_level->num_loggers);
  EXPECT_STREQ("rcl", log_level->logger_settings[0].name);
  EXPECT_EQ(static_cast<int>(RCUTILS_LOG_SEVERITY_DEBUG), log_level->logger_settings[0].level);
}

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  // We need to do rcutils_logging_shutdown() since the tests above may
  // have initialized logging.
  if (rcutils_logging_shutdown() != RCUTILS_RET_OK) {
    fprintf(stderr, "Failed shutting down rcutils logging\n");
  }
  return ret;
}
