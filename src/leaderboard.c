/**
 * @file leaderboard.c
 * @author Grupo 1
 * @brief Implementação do módulo de Leaderboard (placar) conectado ao Firebase Firestore.
 * @version 2.2
 * @copyright Copyright (c) 2025
 *
 * @note Mudanças da v2.2 (Correção de Conflito):
 * - Corrigido o caminho do #include para "raylib/curl/curl.h".
 * - Adicionado #define NOGDI e #define NOMINMAX para prevenir conflitos com windows.h.
 */

// <<< CORREÇÃO DE CONFLITO (Windows x Raylib) >>>
// Deve ser definido ANTES de qualquer include.
#if defined(_WIN32)
    #define NOGDI
    #define NOMINMAX
#endif

#include "raylib/leaderboard.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "raylib/curl/curl.h"
#include "raylib/cJSON.h"

//---------------------------------------------
// Constantes e Variáveis Estáticas
//---------------------------------------------
#define FIREBASE_PROJECT_ID "projeto-quiz-ods14" 

const char* FIRESTORE_BASE_URL = "https://firestore.googleapis.com/v1/projects/" FIREBASE_PROJECT_ID "/databases/(default)/documents";

static PlayerScore leaderboard[LEADERBOARD_SIZE];
static CURL *curl_handle = NULL;

struct MemoryStruct {
  char *memory;
  size_t size;
};

//---------------------------------------------
// Protótipos de Funções Privadas
//---------------------------------------------
static void FetchLeaderboardFromCloud(void);
static void SubmitScoreToCloud(const char* name, int score);
static int FetchPlayerRank(int score);
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

//---------------------------------------------
// Função Callback do cURL
//---------------------------------------------
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        fprintf(stderr, "Erro de alocação de memória no callback do cURL\n");
        return 0;
    }
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

//---------------------------------------------
// Implementação das Funções Públicas
//---------------------------------------------

void InitLeaderboard(void) {
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        strcpy(leaderboard[i].name, "---");
        leaderboard[i].score = 0;
    }
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    if(!curl_handle) {
        fprintf(stderr, "[Leaderboard] Erro fatal: Falha ao inicializar cURL handle.\n");
        return;
    }
    fprintf(stderr, "[Leaderboard] cURL inicializado com sucesso.\n");
    FetchLeaderboardFromCloud();
}

const PlayerScore* GetLeaderboard(void) {
    return leaderboard;
}

void UpdateLeaderboard(const char* newName, int newScore) {
    SubmitScoreToCloud(newName, newScore);
    FetchLeaderboardFromCloud();
}

int GetPlayerRank(int score) {
    return FetchPlayerRank(score);
}


//---------------------------------------------
// Funções de Comunicação com Firebase
//---------------------------------------------

static void SubmitScoreToCloud(const char* name, int score) {
    if (!curl_handle) {
        fprintf(stderr, "[SubmitScore] Erro: cURL handle não inicializado.\n");
        return;
    }

    CURLcode res;
    struct curl_slist *headers = NULL;
    char url[512];
    char json_payload[256];
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;
    
    snprintf(url, sizeof(url), "%s/scores", FIRESTORE_BASE_URL);
    snprintf(json_payload, sizeof(json_payload),
             "{\"fields\": {\"name\": {\"stringValue\": \"%s\"}, \"score\": {\"integerValue\": \"%d\"}}}",
             name, score);

    fprintf(stderr, "[SubmitScore] URL: %s\n", url);
    fprintf(stderr, "[SubmitScore] Payload: %s\n", json_payload);

    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);

    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
        fprintf(stderr, "[SubmitScore] curl_easy_perform() falhou: %s\n", curl_easy_strerror(res));
    } else {
        long response_code;
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
        fprintf(stderr, "[SubmitScore] HTTP Response Code: %ld\n", response_code);
        if (!(response_code >= 200 && response_code < 300)) {
           fprintf(stderr, "[SubmitScore] Erro no envio para Firestore. Resposta do servidor:\n%s\n", chunk.memory ? chunk.memory : "(sem corpo)");
        } else {
           fprintf(stderr, "[SubmitScore] Pontuação enviada com sucesso!\n");
        }
    }

    free(chunk.memory);
    curl_slist_free_all(headers);
    curl_easy_reset(curl_handle);
}

static void FetchLeaderboardFromCloud(void) {
    if (!curl_handle) {
        fprintf(stderr, "[FetchLeaderboard] Erro: cURL handle não inicializado.\n");
        return;
    }

    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;
    char url[512];

    snprintf(url, sizeof(url), "%s/scores?orderBy=score%%20desc&pageSize=%d", FIRESTORE_BASE_URL, LEADERBOARD_SIZE);
    fprintf(stderr, "[FetchLeaderboard] URL: %s\n", url);

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);

    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
        fprintf(stderr, "[FetchLeaderboard] curl_easy_perform() falhou: %s\n", curl_easy_strerror(res));
    } else {
        long response_code;
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
        fprintf(stderr, "[FetchLeaderboard] HTTP Response Code: %ld\n", response_code);
        
        if (response_code == 200) {
            fprintf(stderr, "[FetchLeaderboard] Resposta recebida (tamanho: %zu bytes). Analisando JSON...\n", chunk.size);
            cJSON *json = cJSON_Parse(chunk.memory);
            if (json == NULL) {
                const char *error_ptr = cJSON_GetErrorPtr();
                if (error_ptr != NULL) {
                    fprintf(stderr, "[FetchLeaderboard] Erro ao parsear JSON: %s\n", error_ptr);
                } else {
                    fprintf(stderr, "[FetchLeaderboard] Erro ao parsear JSON, mas cJSON_GetErrorPtr() retornou NULL.\n");
                }
            } else {
                cJSON *documents = cJSON_GetObjectItemCaseSensitive(json, "documents");
                if (!cJSON_IsArray(documents)) {
                     fprintf(stderr, "[FetchLeaderboard] Resposta JSON não contém um array 'documents'.\n");
                } else {
                    cJSON *doc = NULL;
                    int count = 0;
                    cJSON_ArrayForEach(doc, documents) {
                        if (count >= LEADERBOARD_SIZE) break;
                        cJSON *fields = cJSON_GetObjectItemCaseSensitive(doc, "fields");
                        cJSON *nameObj = cJSON_GetObjectItemCaseSensitive(fields, "name");
                        cJSON *scoreObj = cJSON_GetObjectItemCaseSensitive(fields, "score");
                        if (cJSON_IsObject(nameObj) && cJSON_IsObject(scoreObj)) {
                            cJSON *nameVal = cJSON_GetObjectItemCaseSensitive(nameObj, "stringValue");
                            cJSON *scoreVal = cJSON_GetObjectItemCaseSensitive(scoreObj, "integerValue");
                            if (cJSON_IsString(nameVal) && (nameVal->valuestring != NULL) && cJSON_IsString(scoreVal)) {
                                strncpy(leaderboard[count].name, nameVal->valuestring, MAX_NAME_LENGTH);
                                leaderboard[count].name[MAX_NAME_LENGTH] = '\0';
                                leaderboard[count].score = atoi(scoreVal->valuestring);
                                fprintf(stderr, "[FetchLeaderboard] Lido: %s - %d\n", leaderboard[count].name, leaderboard[count].score);
                                count++;
                            }
                        }
                    }
                    cJSON_Delete(json);
                    fprintf(stderr, "[FetchLeaderboard] Leitura do JSON concluída. %d scores carregados.\n", count);
                    for (int i = count; i < LEADERBOARD_SIZE; i++) {
                         strcpy(leaderboard[i].name, "---");
                         leaderboard[i].score = 0;
                    }
                }
            }
        } else {
             fprintf(stderr, "[FetchLeaderboard] Erro ao buscar do Firestore. Resposta do servidor:\n%s\n", chunk.memory ? chunk.memory : "(sem corpo)");
        }
    }
    free(chunk.memory);
    curl_easy_reset(curl_handle);
}

static int FetchPlayerRank(int score) {
    if (!curl_handle) {
        fprintf(stderr, "[FetchPlayerRank] Erro: cURL handle não inicializado.\n");
        return -1;
    }

    CURLcode res;
    struct curl_slist *headers = NULL;
    char url[512];
    char json_payload[512];
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;
    int rank = -1;

    snprintf(url, sizeof(url), "%s:runAggregationQuery", FIRESTORE_BASE_URL);

    snprintf(json_payload, sizeof(json_payload),
             "{\"structuredAggregationQuery\": {\"structuredQuery\": {\"from\": [{\"collectionId\": \"scores\"}], \"where\": {\"fieldFilter\": {\"field\": {\"fieldPath\": \"score\"}, \"op\": \"GREATER_THAN\", \"value\": {\"integerValue\": \"%d\"}}}}, \"aggregations\": [{\"count\": {}, \"alias\": \"total_count\"}]}}",
             score);

    fprintf(stderr, "[FetchPlayerRank] URL: %s\n", url);
    fprintf(stderr, "[FetchPlayerRank] Payload: %s\n", json_payload);

    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "[FetchPlayerRank] curl_easy_perform() falhou: %s\n", curl_easy_strerror(res));
    } else {
        long response_code;
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
        fprintf(stderr, "[FetchPlayerRank] HTTP Response Code: %ld\n", response_code);

        if (response_code == 200) {
            cJSON *json_array = cJSON_Parse(chunk.memory);
            cJSON *json = cJSON_GetArrayItem(json_array, 0); 

            if (json) {
                cJSON *result = cJSON_GetObjectItemCaseSensitive(json, "result");
                cJSON *aggregateFields = cJSON_GetObjectItemCaseSensitive(result, "aggregateFields");
                cJSON *totalCountObj = cJSON_GetObjectItemCaseSensitive(aggregateFields, "total_count");
                cJSON *countVal = cJSON_GetObjectItemCaseSensitive(totalCountObj, "integerValue");

                if (cJSON_IsString(countVal)) {
                    int count = atoi(countVal->valuestring);
                    rank = count + 1; 
                    fprintf(stderr, "[FetchPlayerRank] %d scores maiores. Rank do jogador: %d\n", count, rank);
                } else {
                    fprintf(stderr, "[FetchPlayerRank] Erro ao parsear 'integerValue' do count.\n");
                }
            } else {
                fprintf(stderr, "[FetchPlayerRank] Erro ao parsear JSON da resposta.\n");
            }
            cJSON_Delete(json_array);
        } else {
            fprintf(stderr, "[FetchPlayerRank] Erro na consulta. Resposta do servidor:\n%s\n", chunk.memory ? chunk.memory : "(sem corpo)");
        }
    }

    free(chunk.memory);
    curl_easy_reset(curl_handle);

    return rank;
}