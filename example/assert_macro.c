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

#include <string.h>
#include "assert_macro.h"

void player_init(struct player_stats *stats, const char *username)
{
    if (stats == NULL || username == NULL) {
        return;
    }

    stats->score = 0;
    stats->level = 1;
    strncpy(stats->username, username, sizeof(stats->username) - 1);
    stats->username[sizeof(stats->username) - 1] = '\0';
}

void player_award_points(struct player_stats *stats, uint32_t points)
{
    if (stats == NULL) {
        return;
    }

    stats->score += points;

    /* Auto level-up based on score thresholds */
    if (stats->score >= 500) {
        stats->level = 4;
    } else if (stats->score >= 250) {
        stats->level = 3;
    } else if (stats->score >= 100) {
        stats->level = 2;
    }
}

uint8_t player_get_level(const struct player_stats *stats)
{
    if (stats == NULL) {
        return 0;
    }

    return stats->level;
}

const char *player_get_username(const struct player_stats *stats)
{
    if (stats == NULL) {
        return NULL;
    }

    return stats->username;
}

void player_copy_stats(struct player_stats *dest,
                       const struct player_stats *src)
{
    if (dest == NULL || src == NULL) {
        return;
    }

    memcpy(dest, src, sizeof(struct player_stats));
}
