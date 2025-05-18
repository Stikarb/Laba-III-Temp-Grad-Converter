#include "mongoose.h"
#include "input.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Функция конвертации
float convert_temp(float value, const char *direction) {
    return (strcmp(direction, "CtoF") == 0) ? (value * 9/5 + 32) : ((value - 32) * 5/9);
}

// Обработчик запросов
static int process_request(struct mg_connection *c, struct mg_http_message *hm) {
    // ... (предыдущий код обработки авторизации)

    // Обработка /convert
    if (mg_strcmp(hm->uri, mg_str("/convert")) == 0) {
        if (mg_strcmp(hm->method, mg_str("POST")) == 0) {
            char temp_str[50], direction[10];
            mg_http_get_var(&hm->body, "temperature", temp_str, sizeof(temp_str));
            mg_http_get_var(&hm->body, "direction", direction, sizeof(direction));

            float temp = strtof(temp_str, NULL);
            if (temp == 0 && temp_str[0] != '0' && temp_str[0] != '\0') {
                response = read_file(PATH_ERROR_HTML);
            } else {
                float result = convert_temp(temp, direction);
                char dynamic_html[512];
                const char *input_scale = (strcmp(direction, "CtoF") == 0) ? "C" : "F";
                const char *output_scale = (strcmp(direction, "CtoF") == 0) ? "F" : "C";
                snprintf(
                    dynamic_html, sizeof(dynamic_html),
                    read_file(PATH_RESULT_HTML),
                    temp_str, input_scale, result, output_scale
                );
                response = strdup(dynamic_html);
            }
        } else {
            response = read_file(PATH_CONVERT_HTML);
        }
    }

    // ... (отправка ответа)
}
