// Acknowledgement: This API was adapted from LevelDB, and so we reproduce the
// LevelDB copyright statement below.
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#pragma once

#include <cstdlib>

namespace llsm {

// Database options
struct Options {
  // Create the database directory if it does not exist
  bool create_if_missing = false;

  // If set, prevent the database from being opened if it already exists
  bool error_if_exists = false;
};

struct ReadOptions {};

struct WriteOptions {};

}  // namespace llsm
