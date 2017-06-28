#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "dsp.h"

#define DB_NAME "db"

enum {
    SELECT_NODE,
    INSERT_NODE,
    UPDATE_NODE_ADDRESS,
    NUM_OF_STMTS
};

struct db {
    sqlite3 *conn;
    sqlite3_stmt *statement[NUM_OF_STMTS];
};

char *sql[] = {
    "SELECT public_key, address FROM node WHERE fingerprint = ?",
    "INSERT INTO node VALUES (?, ?, ?)",
    "UPDATE node SET address = ?"
};

char const * const schema =
    "CREATE TABLE node ("
        "fingerprint PRIMARY KEY,"
        "public_key NOT NULL,"
        "address TEXT NOT NULL);";
static dsp_error create_schema (struct db *db)
{
    char *err_msg;
    int ret = sqlite3_exec(db->conn, schema, NULL, NULL, &err_msg);
    if (ret) {
        dsp_error err = db_error(ret, err_msg);
        sqlite3_free(err_msg);
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

//TODO: create or update every table
static dsp_error validate_schema (struct db *db)
{
    dsp_error err;
    int i = 0;
    char *err_msg;
    int ret = sqlite3_exec(db->conn, "PRAGMA schema_version",
            validate_schema_callback, &i, &err_msg);
    if (ret) {
        err = db_error(ret, err_msg);
        sqlite3_free(err_msg);
        return err;
    }
    if (!i) {
        if (err = create_schema(db))
            return trace(err);
    }
    return NULL;
}

static dsp_error prepare_statements (struct db *db)
{
    for (int i = 0; i < NUM_OF_STMTS; i++) {
        int ret = sqlite3_prepare_v2(db->conn, sql[i], -1, &db->statement[i],
                NULL);
        if (ret) return db_error(ret, "Failed to initialize SQL statement");
        // The prepared statement should not be NULL if no error is returned
        assert(db->statement[i]);
    }
    return NULL;
}

dsp_error db_open (struct db **db) {
    int ret;
    dsp_error err;
    if (!(*db = malloc(sizeof(struct db))))
        return sys_error(DSP_E_SYSTEM, errno, NULL);
    if (ret = sqlite3_open(DB_NAME, &(*db)->conn))
        return db_error(ret, "Failed to open database");
    if (err = validate_schema(*db))
        return trace(err);
    if (err = prepare_statements(*db))
        return trace(err);
    return NULL;
}

dsp_error db_close (struct db *db) {
    assert(db && db->conn);
    int ret;
    for (int i; i < NUM_OF_STMTS; i++) {
        if (ret = sqlite3_finalize(db->statement[i]))
            return db_error(ret, "Failed to finalize SQL statement");
    }
    if (ret = sqlite3_close(db->conn))
        return db_error(ret, "Failed to close database");
    free(db);
    return NULL;
}

static dsp_error reset_stmt (struct db *db, int i)
{
    sqlite3_clear_bindings(db->statement[i]);
    int ret = sqlite3_reset(db->statement[i]);
    if (ret) return db_error(ret, "Failed to reset SQL statment");
    return NULL;
}

dsp_error select_node (struct db *db, unsigned char *fingerprint, struct node **node)
{
    // Bind fingerprint to where clause
    int ret = sqlite3_bind_blob(db->statement[SELECT_NODE], 1, fingerprint, HASH_LENGTH, SQLITE_STATIC);
    if (ret) return db_error(ret, NULL);
    // Select row
    ret = sqlite3_step(db->statement[SELECT_NODE]);
    if (ret != SQLITE_ROW) {
        if (ret == SQLITE_DONE) {
            *node = NULL;
            return NULL;
        }
        return db_error(ret, NULL);
    }
    // Allocate node
    *node = calloc(1, sizeof(struct node));
    if (!*node) return sys_error(DSP_E_SYSTEM, errno, "Failed to allocate node");
    // Set fingerprint
    memcpy((*node)->fingerprint, fingerprint, HASH_LENGTH);
    // Retrieve public key blob
    void const *res = sqlite3_column_blob(db->statement[SELECT_NODE], 0);
    int n = sqlite3_column_bytes(db->statement[SELECT_NODE], 0);
    if (n != PUBLIC_KEY_LENGTH) return error(DSP_E_NODE_INVALID, "Invalid public key");
    memcpy((*node)->public_key, res, PUBLIC_KEY_LENGTH);
    // Retrieve address string 
    res = sqlite3_column_text(db->statement[SELECT_NODE], 1);
    n = sqlite3_column_bytes(db->statement[SELECT_NODE], 1);
    strncpy((*node)->address, res, n + 1);
    assert(strlen(res) == n);
    // Reset statement
    dsp_error err = reset_stmt(db, SELECT_NODE);
    if (err) return trace(err);
    return NULL;
}

dsp_error insert_node (struct db *db, struct node *node)
{
    // Bind fingerprint
    int ret = sqlite3_bind_blob(db->statement[INSERT_NODE], 1, node->fingerprint, HASH_LENGTH, SQLITE_STATIC);
    if (ret) return db_error(ret, NULL);
    // Bind public_key
    ret = sqlite3_bind_blob(db->statement[INSERT_NODE], 2, node->public_key, PUBLIC_KEY_LENGTH, SQLITE_STATIC);
    if (ret) return db_error(ret, NULL);
    // Bind address
    ret = sqlite3_bind_text(db->statement[INSERT_NODE], 3, node->address, -1, SQLITE_STATIC);
    if (ret) return db_error(ret, NULL);
    // Perform insert
    ret = sqlite3_step(db->statement[SELECT_NODE]);
    if (ret != SQLITE_DONE) return db_error(ret, NULL);
    // Reset statement
    dsp_error err = reset_stmt(db, INSERT_NODE);
    if (err) return trace(err);
    return NULL;
}

dsp_error update_node (struct db *db, struct node *node)
{
    return NULL;
}
