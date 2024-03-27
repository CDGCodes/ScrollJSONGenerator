#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct {
    unsigned char *buffer;
    size_t len;
    size_t buflen;
} get_request;

#define CHUNK_SIZE 4096

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t realsize = size * nmemb;
    get_request *req = (get_request *) userdata;

    while (req->buflen < req->len + realsize + 1)
    {
        req->buffer = realloc(req->buffer, req->buflen + CHUNK_SIZE);
        req->buflen += CHUNK_SIZE;
    }
    memcpy(&req->buffer[req->len], ptr, realsize);
    req->len += realsize;
    req->buffer[req->len] = 0;

    return realsize;
}

char *url = "https://db.ygoprodeck.com/api/v7/cardinfo.php";
char *path = "./json/";
char *fragmentedJSON[] = {"{\n"
	"\t\"credit\": \"Made with Blockbench\",\n"
	"\t\"parent\": \"scrollyugioh:custom/number_c103\",\n"
	"\t\"textures\": {\n"
		"\t\t\"1\": \"MyMod:Desktop/net6.0/skull/", "\"\n"
	"\t},\n"
	"\t\"elements\": [\n"
		"\t\t{\n"
			"\t\t\t\"from\": [3, 1, 8.5],\n"
			"\t\t\t\"to\": [13, 15, 9],\n"
			"\t\t\t\"faces\": {\n"
				"\t\t\t\t\"north\": {\"uv\": [0.82813, 0.28125, 6.04688, 7.92188], \"texture\": \"#1\"},\n"
				"\t\t\t\t\"east\": {\"uv\": [13.79688, 1.28125, 14.93751, 8.34375], \"texture\": \"#1\"},\n"
				"\t\t\t\t\"south\": {\"uv\": [7.04688, 7.23438, 12.25117, 14.8125], \"texture\": \"#1\"},\n"
				"\t\t\t\t\"west\": {\"uv\": [13.79688, 1.28125, 14.57813, 8.34375], \"texture\": \"#1\"},\n"
				"\t\t\t\t\"up\": {\"uv\": [14.01563, 6.90625, 14.53125, 8.34375], \"texture\": \"#1\"},\n"
				"\t\t\t\t\"down\": {\"uv\": [14.01563, 6.90625, 14.53125, 8.34375], \"texture\": \"#1\"}\n"
			"\t\t\t}\n"
		"\t\t}\n"
	"\t],\n"
	"\t\"display\": {\n"
		"\t\t\"thirdperson_righthand\": {\n"
			"\t\t\t\"translation\": [0, 1.75, 0.5],\n"
			"\t\t\t\"scale\": [0.27734, 0.26563, 0.31836]\n"
		"\t\t},\n"
		"\t\t\"thirdperson_lefthand\": {\n"
			"\t\t\t\"translation\": [0, 1.75, 0.5],\n"
			"\t\t\t\"scale\": [0.27734, 0.26563, 0.31836]\n"
		"\t\t},\n"
		"\t\t\"firstperson_righthand\": {\n"
			"\t\t\t\"translation\": [0, 1.75, 0.5],\n"
			"\t\t\t\"scale\": [0.27734, 0.26563, 0.31836]\n"
		"\t\t},\n"
		"\t\t\"firstperson_lefthand\": {\n"
			"\t\t\t\"translation\": [0, 1.75, 0.5],\n"
			"\t\t\t\"scale\": [0.27734, 0.26563, 0.31836]\n"
		"\t\t},\n"
		"\t\t\"ground\": {\n"
			"\t\t\t\"translation\": [0, -1.75, 0.5],\n"
			"\t\t\t\"scale\": [0.27734, 0.26563, 0.31836]\n"
		"\t\t},\n"
		"\t\t\"head\": {\n"
			"\t\t\t\"translation\": [0, 1.75, 0.5],\n"
			"\t\t\t\"scale\": [0.27734, 0.26563, 0.31836]\n"
		"\t\t},\n"
		"\t\t\"fixed\": {\n"
			"\t\t\t\"rotation\": [0, -172, 0],\n"
			"\t\t\t\"translation\": [-1, 0.75, 0],\n"
			"\t\t\t\"scale\": [0.69, 0.74, 1]\n"
		"\t\t}\n"
	"\t}\n"
"}"};

int main(int argc, char *argv[])
{
	struct stat st = {0};
	if (stat(path, &st) == -1) {
		mkdir(path, 0700);
	}
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    get_request req = {.buffer = NULL, .len = 0, .buflen=0};

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "length: 20");
        headers = curl_slist_append(headers, "numbers: true");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        req.buffer = malloc(CHUNK_SIZE);
        req.buflen = CHUNK_SIZE;

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&req);

        res = curl_easy_perform(curl);

		cJSON* root = cJSON_Parse(req.buffer);
		cJSON* data = root->child;
		if (data) {
			cJSON* card = data->child;
			printf("%lu\n", strlen(fragmentedJSON[0]+strlen(fragmentedJSON[1]+8)));
			while (card) {
				cJSON* id = cJSON_GetObjectItem(card, "id");
				char filepath[20];
				sprintf(filepath, "%s%d.json", path, id->valueint);
				char contents[800];
				sprintf(contents, "%s%d", fragmentedJSON[0], id->valueint);
				int fd = open(filepath, O_CREAT|O_RDWR|O_TRUNC, S_IRWXO|S_IRWXU|S_IRWXG);
				write(fd, contents, strlen(contents));
				write(fd, fragmentedJSON[1], strlen(fragmentedJSON[1]));
				close(fd);
				//printf("%d\n", id->valueint);
				card = card->next;
			}
		}
		cJSON_Delete(root);
        //printf("Data Received:\n%s\n", req.buffer);
        free(req.buffer);
    }
    //printf("%s%s", fragmentedJSON[0], fragmentedJSON[1]);
    curl_easy_cleanup(curl);
    return 0;
}
