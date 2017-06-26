#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "dsp.h"

char const * const schema =
    "CREATE TABLE bucket (number INTEGER PRIMARY KEY);"
    "CREATE TABLE node (fingerprint PRIMARY KEY, address TEXT NOT NULL, public_key);";

struct db {
    sqlite3 *conn;
};

/*
static void db_error (int code)
{
    char *prefix = "SQLite error: ";
    strncpy(error_msg, prefix, ERR_BUF);
    strncpy(error_msg + strlen(prefix), sqlite3_errstr(code),
        ERR_BUF - strlen(prefix));
}*/

static dsp_error create_schema (struct db *db)
{
    dsp_error err;
    char *err_msg;
    int ret = sqlite3_exec(db->conn, schema, NULL, NULL, &err_msg);
    if (ret) {
        err = error(DSP_E_DATABASE);
        set_error_message(err, err_msg);
        sqlite3_free(err);
        return err;
    }
    return NULL;
}

static int validate_schema_callback (void *i, int argc, char **argv, char **column)
{
    if (strcmp(*column, "schema_version")) return -1;
    *(int *) i = atoi(*argv);
    return 0;
}

static dsp_error validate_schema (struct db *db)
{
    dsp_error err;
    int i = 0;
    char *err_msg;
    int ret = sqlite3_exec(db->conn, "PRAGMA schema_version",
            validate_schema_callback, &i, &err_msg);
    if (ret) {
        err = error(DSP_E_DATABASE);
        set_error_message(err, err_msg);
        sqlite3_free(err);
        return err;
    }
    if (!i) {
        if (err = create_schema(db))
            return trace(err);
    }
    return NULL;
}

dsp_error db_open (struct db **db) {
    int ret;
    dsp_error err;
    if (!(*db = malloc(sizeof(struct db))))
        return error(DSP_E_SYSTEM);
    if (ret = sqlite3_open("db", &(*db)->conn))
        return error(DSP_E_DATABASE);
    if (err = validate_schema(*db))
        return trace(err);
    return NULL;
}

dsp_error db_close (struct db *db) {
    assert(db && db->conn);
    int err = sqlite3_close(db->conn);
    if (err) return error(DSP_E_DATABASE);
    free(db);
    return NULL;
}
