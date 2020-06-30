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

#ifndef RCL__LOG_LEVEL_H_
#define RCL__LOG_LEVEL_H_

#include "rcl/allocator.h"
#include "rcl/macros.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// A logger item to specify a name and a log level
typedef struct rcl_logger_setting_t
{
  /// name for the logger.
  char * name;
  /// level for the logger.
  rcl_log_severity_t level;
} rcl_logger_setting_t;

/// Hold default logger level and other logger setting.
typedef struct rcl_log_level_t
{
  /// Default logger level
  rcl_log_severity_t default_logger_level;
  /// Array of logger setting
  struct rcl_logger_setting_t * logger_settings;
  /// Number of logger settings
  size_t num_logger_settings;
  /// Allocator used to allocate objects in this struct
  rcl_allocator_t allocator;
} rcl_log_level_t;


/// \brief Initialize log level structure
/// \param[in] allocator memory allocator to be used
/// \return a pointer to log level structure on success or NULL on failure
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_log_level_t *
rcl_log_level_init(const rcl_allocator_t allocator);

/// \brief Copy log level structure, allocate memory by using allocator of rcl_log_level_t
/// \param[in] log_level points to the log level struct to be copied
/// \return a pointer to the copied log level structure on success or NULL on failure
RCL_PUBLIC
RCL_WARN_UNUSED
rcl_log_level_t * rcl_log_level_copy(const rcl_log_level_t * log_level);


/// \brief Free log level structure
/// \param[in] log_level points to the log level structure to be freed
RCL_PUBLIC
void
rcl_log_level_fini(rcl_log_level_t * log_level);

#ifdef __cplusplus
}
#endif

#endif  // RCL__LOG_LEVEL_H_
