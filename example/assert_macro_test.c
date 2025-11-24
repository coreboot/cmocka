/*
 * Copyright 2025 Andreas Schneider <asn@cryptomilk.org>
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

/**
 * @file assert_macro_test.c
 * @brief Example demonstrating cmocka assertion macros
 *
 * This example shows how to use cmocka's most common assertion macros:
 * - assert_uint_equal()   : Compare unsigned integer values
 * - assert_string_equal() : Compare string values
 * - assert_memory_equal() : Compare memory regions (structs, arrays, etc.)
 *
 * The example uses a simple game player statistics system to make the tests
 * more interesting and relatable.
 *
 * NOTE: This test is designed to FAIL to demonstrate what happens when
 * assertions don't match. The failures are intentional for educational
 * purposes.
 */

#include <cmocka.h>
#include <string.h>

#include "assert_macro.h"

/**
 * @brief Test player initialization
 *
 * Demonstrates:
 * - assert_uint_equal() for comparing the initial level
 * - assert_string_equal() for verifying the username was set correctly
 *
 * This test PASSES - all assertions match expected values.
 */
static void test_player_init(void **state)
{
    struct player_stats player;

    (void)state; /* unused */

    /* Initialize a new player */
    player_init(&player, "DragonSlayer42");

    /* Check that the player starts at level 1 */
    assert_uint_equal(player_get_level(&player), 1);

    /* Verify the username was set correctly */
    assert_string_equal(player_get_username(&player), "DragonSlayer42");
}

/**
 * @brief Test scoring and level advancement
 *
 * Demonstrates:
 * - assert_uint_equal() for checking level progression
 *
 * This test FAILS - the player should reach level 3 (requires 250 points),
 * but the test incorrectly expects level 4.
 */
static void test_player_scoring(void **state)
{
    struct player_stats player;

    (void)state; /* unused */

    player_init(&player, "LeetHacker");

    /* Award some points - should reach level 2 (100+ points) */
    player_award_points(&player, 150);
    assert_uint_equal(player_get_level(&player), 2);

    /* Award more points - should reach level 3 (250+ points total) */
    player_award_points(&player, 100);

    /* BUG: This assertion will FAIL!
     * Player has 250 points (level 3), but we're checking for level 4.
     * Level 4 requires 500+ points.
     */
    assert_uint_equal(player_get_level(&player), 4);
}

/**
 * @brief Test player statistics copying
 *
 * Demonstrates:
 * - assert_memory_equal() for comparing entire structures
 *
 * This test FAILS - the destination player's score is incorrectly modified
 * after copying, causing the memory comparison to fail.
 */
static void test_player_copy(void **state)
{
    struct player_stats original;
    struct player_stats copy;

    (void)state; /* unused */

    /* Create an original player with some stats */
    player_init(&original, "OriginalPlayer");
    player_award_points(&original, 300); /* Should be level 3 */

    /* Copy the stats to another player */
    player_copy_stats(&copy, &original);

    /* BUG: Accidentally modify the copy's score */
    copy.score = 999; /* Oops! This breaks the copy */

    /* BUG: This assertion will FAIL!
     * The structures are no longer identical because copy.score was changed.
     * assert_memory_equal() compares the entire memory region byte-by-byte.
     */
    assert_memory_equal(&copy, &original, sizeof(struct player_stats));
}

/**
 * @brief Test username validation
 *
 * Demonstrates:
 * - assert_string_equal() for string comparison
 *
 * This test FAILS - there's a typo in the expected username.
 */
static void test_player_username(void **state)
{
    struct player_stats player;

    (void)state; /* unused */

    player_init(&player, "NinjaWarrior");

    /* BUG: This assertion will FAIL!
     * Expected "NinjaWorrior" (typo) but actual is "NinjaWarrior"
     */
    assert_string_equal(player_get_username(&player), "NinjaWorrior");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_player_init),
        cmocka_unit_test(test_player_scoring),
        cmocka_unit_test(test_player_copy),
        cmocka_unit_test(test_player_username),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
