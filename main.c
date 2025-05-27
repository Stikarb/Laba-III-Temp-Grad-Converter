#include "mongoose/mongoose.h"
#include "input/input.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

static int process_request(struct mg_connection *c, struct mg_http_message *hm) {
    char *response = NULL;

    // Отдача статических файлов
    if (mg_strcmp(hm->uri, mg_str("/css/style.css")) == 0) {
        response = read_file(PATH_CSS_STYLES);
        mg_http_reply(c, 200, CONTENT_TYPE_CSS, "%s", response ? response : "");
    } 
    else if (mg_strcmp(hm->uri, mg_str("/convert")) == 0) {
        response = read_file(PATH_CONVERT_HTML);
        mg_http_reply(c, 200, CONTENT_TYPE_HTML, "%s", response ? response : "");
    }
    else {
        // Для корневого пути перенаправляем на /convert
        mg_http_reply(c, 302, "Location: /convert\n", "");
    }

    free(response);
    return 0;
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
