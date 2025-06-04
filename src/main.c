#include "mongoose/mongoose.h"
#include "input/input.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

// Функция для конвертации температуры (без изменений)
static void convert_temperature(double value, const char* direction, 
                                double* result, const char** from_unit, const char** to_unit) {
    if (strcmp(direction, "CtoF") == 0) {
        *result = value * 9.0 / 5.0 + 32.0;
        *from_unit = "C";
        *to_unit = "F";
    } else {
        *result = (value - 32.0) * 5.0 / 9.0;
        *from_unit = "F";
        *to_unit = "C";
    }
}

// Функция для генерации HTML с результатом (без изменений)
static char* generate_result_html(const char* result_text) {
    const char* template = "<!DOCTYPE html><html lang=\"ru\"><head>"
        "<meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
        "<title>Результат конвертации</title><link href=\"/css/style.css\" rel=\"stylesheet\"></head>"
        "<body><div class=\"glass-container\"><h1>🌡️ Результат конвертации</h1>"
        "<div class=\"result-message\"><span id=\"resultText\">%s</span>"
        "<a href=\"/convert\" class=\"back-btn\">← Новый расчёт</a></div></div></body></html>";
    
    char* html = malloc(strlen(template) + strlen(result_text) + 100);
    if (html) {
        sprintf(html, template, result_text);
    }
    return html;
}

static int process_request(struct mg_connection *c, struct mg_http_message *hm) {
    char *response = NULL;

    // Отдача статических файлов
    if (mg_strcmp(hm->uri, mg_str(URL_CSS_STYLES)) == 0) {
        response = read_file(PATH_CSS_STYLES);
        mg_http_reply(c, 200, CONTENT_TYPE_CSS, "%s", response ? response : "");
    } 
    // Обработка POST-запроса для конвертации
    else if (mg_strcmp(hm->uri, mg_str(URL_CONVERT)) == 0 && mg_strcmp(hm->method, mg_str("POST")) == 0) {
        char temp_str[20], direction[10];
        double temp_value, result;
        const char *from_unit, *to_unit;
        
        // Извлечение параметров из тела запроса
        mg_http_get_var(&hm->body, "temperature", temp_str, sizeof(temp_str));
        mg_http_get_var(&hm->body, "direction", direction, sizeof(direction));
        
        // Проверка и конвертация
        if (sscanf(temp_str, "%lf", &temp_value) == 1) {
            convert_temperature(temp_value, direction, &result, &from_unit, &to_unit);
            
            // Форматирование результата
            char result_text[100];
            snprintf(result_text, sizeof(result_text), "%.2f°%s = %.2f°%s", 
                    temp_value, from_unit, result, to_unit);
            
            // Генерация HTML с результатом
            response = generate_result_html(result_text);
            mg_http_reply(c, 200, CONTENT_TYPE_HTML, "%s", response ? response : "");
        } else {
            // Ошибка ввода
            response = read_file(PATH_ERROR_HTML);
            mg_http_reply(c, 400, CONTENT_TYPE_HTML, "%s", response ? response : "");
        }
    }
    // Отдача формы конвертации
    else if (mg_strcmp(hm->uri, mg_str(URL_CONVERT)) == 0) {
        response = read_file(PATH_CONVERT_HTML);
        mg_http_reply(c, 200, CONTENT_TYPE_HTML, "%s", response ? response : "");
    }
    else {
        // Для корневого пути перенаправляем на /convert
        mg_http_reply(c, 302, "Location: " URL_CONVERT "\r\n", "");
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
