#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "dsp.h"

struct db {
    sqlite3 *conn;
};

static void db_error (int code)
{
    char *prefix = "SQLite error: ";
    strncpy(error_msg, prefix, ERR_BUF);
    strncpy(error_msg + strlen(prefix), sqlite3_errstr(code),
        ERR_BUF - strlen(prefix));
}

int db_open (struct db **db) {
    int err;
    *db = malloc(sizeof(struct db));
    if (err = sqlite3_open("dsp.db", &(*db)->conn)) {
        db_error(err);
        return DSP_ESQLITE;
    }
    return DSP_OK;
}

int db_close (struct db *db) {
    return DSP_OK;
}
