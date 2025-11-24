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

#ifndef ASSERT_MACRO_H
#define ASSERT_MACRO_H

#include <stdint.h>

/**
 * @brief Player statistics structure
 *
 * Represents a player's game statistics including score, level, and username.
 */
struct player_stats {
    uint32_t score;    /**< Player's current score */
    uint8_t level;     /**< Player's current level (1-100) */
    char username[32]; /**< Player's username */
};

/**
 * @brief Initialize a player with default values
 *
 * @param stats Pointer to player_stats structure to initialize
 * @param username The player's username (max 31 characters)
 *
 * Initializes a player with score 0, level 1, and the given username.
 */
void player_init(struct player_stats *stats, const char *username);

/**
 * @brief Award points to a player
 *
 * @param stats Pointer to player_stats structure
 * @param points Number of points to award
 *
 * Adds points to the player's score. If the player reaches certain
 * score thresholds, they automatically level up:
 * - Level 2: 100 points
 * - Level 3: 250 points
 * - Level 4: 500 points
 */
void player_award_points(struct player_stats *stats, uint32_t points);

/**
 * @brief Get the player's current level
 *
 * @param stats Pointer to player_stats structure
 * @return Current level of the player
 */
uint8_t player_get_level(const struct player_stats *stats);

/**
 * @brief Get the player's username
 *
 * @param stats Pointer to player_stats structure
 * @return Pointer to the username string
 */
const char *player_get_username(const struct player_stats *stats);

/**
 * @brief Copy player statistics
 *
 * @param dest Destination player_stats structure
 * @param src Source player_stats structure
 *
 * Copies all statistics from src to dest.
 */
void player_copy_stats(struct player_stats *dest,
                       const struct player_stats *src);

#endif /* ASSERT_MACRO_H */
