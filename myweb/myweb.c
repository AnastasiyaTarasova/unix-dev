#include <stdio.h>
#include <string.h>

#define HTTP_HEADER_LEN 256
#define HTTP_REQUEST_LEN 256
#define HTTP_METHOD_LEN 6
#define HTTP_URI_LEN 8000
#define HTTP_URI_PATH_LEN 1000
#define HTTP_URI_PARAMS_LEN 7000
#define HTTP_SERVER_LEN 36

#define REQ_END 100
#define ERR_NO_URI -100
#define ERR_ENDLESS_URI -101

struct http_req {
	char request[HTTP_REQUEST_LEN];
	char method[HTTP_METHOD_LEN];
	char uri[HTTP_URI_LEN];
	char uri_path[HTTP_URI_PATH_LEN];
	char uri_params[HTTP_URI_PARAMS_LEN];
	// version
	// user_agent
	char server[HTTP_SERVER_LEN];
	// accept
};

int fill_req(char *buf, struct http_req *req) {
	if (strlen(buf) == 2) {
		// пустая строка (\r\n) означает конец запроса
		return REQ_END;
	}
	printf("<p>Input buf = </p>", buf, strlen(buf));
	
	char *p, *a, *b, *first_space, *sec_space, *third_space, *question;
	// Это строка GET-запроса
	p = strstr(buf, "GET");
	if (p == buf) {
		// Строка запроса должна быть вида
		// GET /dir/ HTTP/1.0
		// GET /dir HTTP/1.1
		// GET /test123?r=123 HTTP/1.1
		// и т.п.
		strncpy(req->request, buf, strlen(buf));
		strncpy(req->method, "GET", strlen("GET"));

		a = strchr(buf, '/');
		if ( a != NULL) { // есть запрашиваемый URI 
			b = strchr(a, ' '); // URI ot a do b
			if ( b != NULL ) { // конец URI
				strncpy(req->uri, a, b-a);
				sec_space = strchr(a, ' ');
				question = strchr(a, '?');
				if ( question == NULL && sec_space != NULL) { // path = c - a
					strncpy(req->uri_path, a, sec_space - a);
				}
				else if ( question != NULL ) {
					strncpy(req->uri_path, a, question - a);
					third_space = strchr(question, ' ');
					strncpy(req->uri_params, question + 1, third_space - question);
					return 0;
				}
				
			} else {
				return ERR_ENDLESS_URI;  
				// тогда это что-то не то
			}
		} else {
			return ERR_NO_URI; 
			// тогда это что-то не то
		}
	}

	p = strstr(buf, "HOST");
	if (p == buf)
	{
		strncpy(req->request, buf, strlen(buf));
                strncpy(req->method, "HOST", strlen("HOST"));
		first_space = strchr(p, ' ');
		sec_space = strchr(first_space, ' ');
		strncpy(req->server, first_space + 1, sec_space - first_space);
	}

	return 0;
}

int log_req(struct http_req *req) {
	// fprintf(stderr, "%s %s\n%s\n", req->request, req->method, req->uri);
	return 0;
}

int make_resp(struct http_req *req) {
	printf("HTTP/1.1 200 OK\r\n");
	printf("Content-Type: text/html\r\n");
	printf("\r\n");
	printf("<html><body><title>Page title</title><h1>Page Header</h1>");
	printf("<p>Request: %s", req->request);
	printf("<p>Method: %s</p>", req->method);
	printf("<p>URI Path %s</p>", req->uri_path);
	printf("<p>URI Params %s</p>", req->uri_params);
	printf("<p>Server %s", req->server);
	printf("</body></html>\r\n");
	return 0;
}

int main (void) {
	char buf[HTTP_HEADER_LEN];
	struct http_req req;
	while(fgets(buf, sizeof(buf),stdin)) {
		int ret = fill_req(buf, &req);
		if (ret == 0) 
			// строка запроса обработана, переходим к следующей
			continue;
		if (ret == REQ_END ) 
			// конец HTTP запроса, вываливаемся на обработку
			break;
		else
			// какая-то ошибка 
			printf("Error: %d\n", ret);
		
	}
	log_req(&req);
	make_resp(&req);
}
