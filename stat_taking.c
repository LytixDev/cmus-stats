/*
 * Copyright 2022 Nicolai Brand
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "sqlite_handler.h"
#include "track_info.h"
#include "stat_taking.h"

#include <string.h>
#include <stdlib.h>

struct db_ctx *dbc;

void cmus_stats_init(void)
{
        dbc = malloc(sizeof(struct db_ctx));
        char *homedir = getenv("HOME");
        const char *postfix = "/.local/share/cmus-stats/cmus-stats.db";
        if (homedir == NULL) {
                dbc->db_connected = false;
                return;
        }

        strcpy(dbc->db_full_path, homedir);
        strcat(dbc->db_full_path, postfix);

        dbc->db = connect_to_db(dbc->db_full_path);
        if (dbc->db == NULL) {
                dbc->db_connected = false;
                return;
        }
        dbc->db_connected = true;

        create_table(dbc->db);
}

void cmus_stats_exit(void)
{
        sqlite3_close(dbc->db);
        free(dbc);
}

bool db_connected(void)
{
        return dbc->db_connected;
}

void save_track_info_to_db(struct track_info *ti)
{
        int MAX_LEN = 512;
        char title[MAX_LEN];
        char artist[MAX_LEN];
        char genre[MAX_LEN];
        const char *empty = "None";

        if (ti->title == NULL)
            if (strlen(ti->filename) < 512)
                strcpy(title, ti->filename);
            else
                strcpy(title, empty);
        
        else if (strlen(ti->title) > MAX_LEN)
            strcpy(title, empty);
        else
            strcpy(title, ti->title);

        if (ti->artist == NULL)
            strcpy(artist, empty);
        else if (strlen(ti->artist) > MAX_LEN)
            strcpy(artist, empty);
        else
            strcpy(artist, ti->artist);

        if (ti->genre == NULL)
            strcpy(genre, empty);
        else if (strlen(ti->genre) > MAX_LEN)
            strcpy(genre, empty);
        else
            strcpy(genre, ti->genre);

        insert_data(dbc->db, ti->uid, title, artist, genre, ti->duration);
}
