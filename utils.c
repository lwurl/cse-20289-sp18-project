/* utils.c: spidey utilities */

#include "spidey.h"

#include <ctype.h>
#include <errno.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>

/**
 * Determine mime-type from file extension.
 *
 * @param   path        Path to file.
 * @return  An allocated string containing the mime-type of the specified file.
 *
 * This function first finds the file's extension and then scans the contents
 * of the MimeTypesPath file to determine which mimetype the file has.
 *
 * The MimeTypesPath file (typically /etc/mime.types) consists of rules in the
 * following format:
 *
 *  <MIMETYPE>      <EXT1> <EXT2> ...
 *
 * This function simply checks the file extension version each extension for
 * each mimetype and returns the mimetype on the first match.
 *
 * If no extension exists or no matching mimetype is found, then return
 * DefaultMimeType.
 *
 * This function returns an allocated string that must be free'd.
 **/
char * determine_mimetype(const char *path) {
    char *ext = NULL;
    char *mimetype;
    char *token = NULL;
    char buffer[BUFSIZ];
    FILE *fs = NULL;


    /* Find file extension */

    ext = strrchr(path, '.');
    ext++;

    /* Open MimeTypesPath file */
    if ( (fs = fopen(MimeTypesPath, "r")) == NULL){
        goto fail;
    }

    /* Scan file for matching file extensions */
    while (fgets(buffer, BUFSIZ, fs) && sizeof(buffer) > 2){
        chomp(buffer);

        mimetype = strtok(buffer, WHITESPACE);

        // If there was nothing or it is a comment
        if (mimetype == NULL || mimetype[0] == '#'){
            continue;
        }


        // Get past the <MIMETYPE> part
        token = skip_nonwhitespace(buffer);

        // Get to the <EXT1> part
        token = skip_whitespace(token);


        while (token != NULL){
            if (streq(token, ext)){
                goto complete;
            }
            token = strtok(NULL, WHITESPACE);
        }
    }

fail:
    mimetype = DefaultMimeType;

complete:
    if (fs){
        fclose(fs);
    }
    return strdup(mimetype);
}

/**
 * Determine actual filesystem path based on RootPath and URI.
 *
 * @param   uri         Resource path of URI.
 * @return  An allocated string containing the full path of the resource on the
 * local filesystem.
 *
 * This function uses realpath(3) to generate the realpath of the
 * file requested in the URI.
 *
 * As a security check, if the real path does not begin with the RootPath, then
 * return NULL.
 *
 * Otherwise, return a newly allocated string containing the real path.  This
 * string must later be free'd.
 **/
char * determine_request_path(const char *uri) {
    char real_path_str[BUFSIZ];
    char path[BUFSIZ];

    // Construct the path with root and the uri
    strcpy(path, RootPath);
    strcat(path, uri);

    int len_root = strlen(RootPath);
    realpath(path, real_path_str);
    if (real_path_str == NULL){
        return NULL;
    }
    if (strncmp(real_path_str, RootPath, len_root) != 0){
        return NULL;
    }

    return strdup(real_path_str);
}

/**
 * Return static string corresponding to HTTP Status code.
 *
 * @param   status      HTTP Status.
 * @return  Corresponding HTTP Status string (or NULL if not present).
 *
 * http://en.wikipedia.org/wiki/List_of_HTTP_status_codes
 **/
const char * http_status_string(HTTPStatus status) {
    static char *StatusStrings[] = {
        "200 OK",
        "400 Bad Request",
        "404 Not Found",
        "500 Internal Server Error",
        "418 I'm A Teapot",
    };
    const char *str;
    if (status == HTTP_STATUS_OK){
        str = StatusStrings[0];
    }
    else if (status == HTTP_STATUS_BAD_REQUEST){
        str = StatusStrings[1];
    }
    else if (status == HTTP_STATUS_NOT_FOUND){
        str = StatusStrings[2];
    }
    else if (status == HTTP_STATUS_INTERNAL_SERVER_ERROR){
        str = StatusStrings[3];
    }
    else if (status == HTTP_STATUS_I_AM_A_TEAPOT){
        str = StatusStrings[4];
    }

    return str;
}

/**
 * Advance string pointer pass all nonwhitespace characters
 *
 * @param   s           String.
 * @return  Point to first whitespace character in s.
 **/
char * skip_nonwhitespace(char *s) {
    while (isspace(*s)){
        s++;
    }
    return s;
}

/**
 * Advance string pointer pass all whitespace characters
 *
 * @param   s           String.
 * @return  Point to first non-whitespace character in s.
 **/
char * skip_whitespace(char *s) {
    while (!isspace(*s)){
        ++s;
    }
    return s;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
