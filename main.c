#include "mongoose/mongoose.h"
#include "input/input.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

float convert_temp(float value, const char *direction);

static int process_request(struct mg_connection *c, struct mg_http_message *hm) {
    (void)c;
    char *response = NULL;
    int error_code = 0;

    // Обработка CSS
    if (mg_strcmp(hm->uri, mg_str("/css/styles.css")) == 0) {
        response = read_file(PATH_CSS_STYLES);
        if (response) {
            mg_http_reply(c, 200, CONTENT_TYPE_CSS, "%s", response);
        }
        goto cleanup;
    }

    // Обработка /convert
    if (mg_strcmp(hm->uri, mg_str("/convert")) == 0) {
        if (mg_strcmp(hm->method, mg_str("POST")) == 0) {
            char temp_str[50], direction[10];
            mg_http_get_var(&hm->body, "temperature", temp_str, sizeof(temp_str));
            mg_http_get_var(&hm->body, "direction", direction, sizeof(direction));

            float temp = strtof(temp_str, NULL);
            if (temp == 0 && temp_str[0] != '0' && temp_str[0] != '\0') {
                response = read_file(PATH_ERROR_HTML);
                error_code = 1;
            } else {
                float result = convert_temp(temp, direction);
                char *result_template = read_file(PATH_RESULT_HTML);
                char dynamic_html[512];
                const char *input_scale = (strcmp(direction, "CtoF") == 0) ? "C" : "F";
                const char *output_scale = (strcmp(direction, "CtoF") == 0) ? "F" : "C";
                snprintf(dynamic_html, sizeof(dynamic_html), result_template, temp_str, input_scale, result, output_scale);
                response = strdup(dynamic_html);
                free(result_template);
            }
        } else {
            response = read_file(PATH_CONVERT_HTML);
        }
    }

    if (response) {
        mg_http_reply(c, 200, CONTENT_TYPE_HTML, "%s", response);
    } else {
        mg_http_reply(c, 500, CONTENT_TYPE_HTML, "Internal Server Error");
        error_code = 1;
    }

cleanup:
    free(response);
    return error_code;
}

static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        process_request(c, hm);
    }
}

int main(void) {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, "http://0.0.0.0:8081", fn, NULL);
    printf("Сервер запущен на http://0.0.0.0:8081\n");
    for (;;) mg_mgr_poll(&mgr, 1000);
    mg_mgr_free(&mgr);
    return 0;
}

float convert_temp(float value, const char *direction) {
    return (strcmp(direction, "CtoF") == 0) ? (value * 9/5 + 32) : ((value - 32) * 5/9);
}
