/*
 * Copyright 2008 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ASSERT_MODULE_H
#define ASSERT_MODULE_H

/**
 * @file assert_module.h
 * @brief Example module for demonstrating assert() mocking with cmocka
 *
 * This module demonstrates how to test code that uses assert() statements
 * by using cmocka's mock_assert() functionality and expect_assert_failure().
 */

/**
 * @brief Increment an integer value
 *
 * @param value Pointer to the integer value to increment
 *
 * This function asserts that the value pointer is not NULL before
 * incrementing it. The assert can be mocked during testing to verify
 * that the function properly validates its input.
 */
void increment_value(int *const value);

/**
 * @brief Decrement an integer value
 *
 * @param value Pointer to the integer value to decrement
 *
 * This function uses a runtime NULL check (if statement) rather than
 * an assert. It silently returns if the pointer is NULL.
 */
void decrement_value(int *const value);

#endif /* ASSERT_MODULE_H */
