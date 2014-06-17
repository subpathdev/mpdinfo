#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "format.h"
#include "help.h"
#include "debug.h"


#define CAT_OUTPUT "[output]"
#define CAT_GENERAL "[general]"

#define CONFIG_HOST "host"
#define CONFIG_PORT "port"

#define CONFIG_PLAY "play"
#define CONFIG_PAUSE "pause"
#define CONFIG_STOP "stop"

typedef enum {

	C_GENERAL, C_OUTPUT

} Category;


typedef struct {

	char* key;
	char* value;

} ConfigLine;

struct {

	char* host;
	unsigned long int port;

} connectionInfo;


char* cropSpacesAndTabs(char* line) {

	while (line[0] != '\n') {

		if (line[0] != '\t' && line[0] != ' ') {
			return line;
		} else {
			line++;
		}
	}
	return line;
}

void free_connection_info() {

	free(connectionInfo.host);
}

Category parseCategory(char* cat) {

	if (!strncmp(cat, CAT_OUTPUT, strlen(CAT_OUTPUT))) {
		return C_OUTPUT;
	} else {
		return C_GENERAL;
	}

}

void setMPDHost(char* host) {
	printf("Host is set to: %s\n", host);
	connectionInfo.host = malloc(strlen(host) + 1);
	strcpy(connectionInfo.host, host);
}

char* getMPDHost() {
	return connectionInfo.host;
}

unsigned long int getMPDPort() {
	return connectionInfo.port;
}

void setMPDPort(char* port) {
	printf("Port is set to: %s\n", port);
	
	connectionInfo.port = strtoul(port, NULL, 10);

	if (connectionInfo.port == 0) {
		printf("port not correct, using default port 6600");
		connectionInfo.port = 6600;
	}
}

void parseConfigLineOutput(ConfigLine* cl) {
	debug("DEBUG", "in output category parsing");


	printf("current key: %s\n", cl->key);

	if (!strncmp(cl->key, CONFIG_PLAY, strlen(CONFIG_PLAY))) {
		printf("configurate play\n");
		formatPlay(cl->value);
		return;
	}
	if (!strncmp(cl->key, CONFIG_PAUSE, strlen(CONFIG_PAUSE))) {
		formatPause(cl->value);
		return;
	}
	if (!strncmp(cl->key, CONFIG_STOP, strlen(CONFIG_STOP))) {
		formatStop(cl->value);
		return;
	}
}

void parseConfigLineGeneral(ConfigLine* cl) {


	if (!strncmp(cl->key, CONFIG_HOST, strlen(CONFIG_HOST))) {
		setMPDHost(cl->value);
		return;
	}
	if (!strncmp(cl->key, CONFIG_PORT, strlen(CONFIG_PORT))) {
		setMPDPort(cl->value);
		return;
	}
}

char* cropSpacesAndTabsB(char* s) {

	int i = strlen(s) -1;
	while (i > 0 && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n')) {
		s[i] = '\0';
		i--;
	}

	return s;
}

char* strncpyN(char* dest, char* src, int size) {
	
	if (strlen(src) < size) {
		dest = malloc(strlen(src));
		strcpy(dest, src);
		return dest;
	}
	
	dest = malloc(size + 1);

	int i = 0;

	while (i < size) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return dest;

}

ConfigLine* gettingLineArgs(char* line) {
	line = cropSpacesAndTabs(line);
	int i = 0;

	while (line[i] != '=') {
		if (line[i] == '\n') {
			printf("Error parsing line: %s\n", line);
			return NULL;
		}
		i++;
	}

	ConfigLine* cl = malloc(sizeof(ConfigLine));

	
	cl->key = strncpyN(cl->key, line, i);

	cl->key = cropSpacesAndTabsB(cl->key);

	line += i + 1;

	line = cropSpacesAndTabs(line);
	cl->value = malloc(strlen(line) + 1);
	strcpy(cl->value, line);
	cl->value = cropSpacesAndTabsB(cl->value);

	return cl;
}



void parseConfigLine(Category cat, char* line) {


	ConfigLine* cl = gettingLineArgs(line);

	if (cl == NULL) {
		
		printf("lineargs not correct\n");
		return;
	}

	switch (cat) {
	
		case C_GENERAL:
			parseConfigLineGeneral(cl);
			break;
		case C_OUTPUT:
			parseConfigLineOutput(cl);
			break;
		default:
			parseConfigLineGeneral(cl);
			break;
	}

	free(cl->key);
	free(cl->value);
	free(cl);
}


void parseConfigFile(char* path) {

	debug("DEBUG", "starting config reader");

	FILE* file = fopen(path, "r");

	if (file == NULL) {

		perror("Cannot read config file.\n");
		return;
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	Category category = C_GENERAL;

	while ( (read = getline(&line, &len, file)) != -1) {
		debug("DEBUG", line);
		
		line = cropSpacesAndTabs(line);
		if (line[0] != '#' && line[0] != '\n') {
			
			if (line[0] == '[') {
				debug("DEBUG", line);
				category = parseCategory(line);
			} else {
				parseConfigLine(category, line);
			}
		}

	}
	free(line);
	fclose(file);

}

void parseArguments(int argc, char* argv[]) {

    //parse arguments
    int i;
    for (i = 1; i < argc; i++) {
        if (*argv[i] == '-') {
            if (i + 1 < argc && (!strcmp(argv[i], "--format=play") || !strcmp(argv[i], "-fpl"))) { 
		formatPlay(argv[i + 1]);
                i++;
            } else if (i + 1 < argc && (!strcmp(argv[i], "--format=pause") || !strcmp(argv[i], "-fpa"))) { 
		formatPause(argv[i + 1]);
                i++;
            } else if (i + 1 < argc && (!strcmp(argv[i], "--format=stop") || !strcmp(argv[i], "-fs"))) { 
		formatStop(argv[i + 1]);
                i++;
            }  else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
                printHelp();
            } else if (!strcmp(argv[i], "--debug") || !strcmp(argv[i], "-d")) {
                setDebug(1);
	    } else if (i + 1 < argc && (!strcmp(argv[i], "--config") || !strcmp(argv[i], "-c"))) {
	    	parseConfigFile(argv[i + 1]);
		i++;
	    } else {
                printf("Invalid arguments.\n");
                printHelp();
            }
        }
    
    }
    checkFormat();
}
