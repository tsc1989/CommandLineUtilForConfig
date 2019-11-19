/*
* Usage: To Configure any standard configuration file
* using command line utility. 
* It works on plain text file as well as encrypted file
* considering chanfing key file to the AES256 key which shall
* be used to encrypt or decrypt configuration file
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>
#include<time.h>

typedef struct {
    int is_empty;
    char str[200];
    char config_param_name[450];
    char config_param_value[450];
    char pre_string[450];
    char post_string[450];
    char comment[450];
} MetaData_t;

#define RED   "\x1B[31m"
#define RESET "\x1B[0m"

#define TEMP_DECRYPTED_FILE "/tmp/test"
#define TEMP_DECRYPTED_MODIFIED_FILE "/tmp/test2"

int main (int argc, char *argv[]) {

    FILE *fp; 
    char str[1000];
    int lines = 0, i , j, empty = 1;
	char *mstr, *value, *tmp = NULL;
    MetaData_t *data = NULL;
	int total_len = 0;
	char cmd[200] = {0};
    char *input_filename = NULL;
    int is_encrypted = 0;
	char dec_filename[200];
	char backup_file_name[200];
    int  remaining_spaces_cnt= 0;
    int c;
	time_t rawtime;
	struct tm *tm;
	time(&rawtime);
	tm = gmtime(&rawtime);

	if (argc < 4) {
		printf("%s -e <encrypted file / Unencrzpted file> -f <file name> \n", argv[0]);
		exit(0);
	}
    while((c = getopt(argc, argv, "f:e:")) != -1) {
        switch (c) {
            case 'f':
            input_filename = optarg;
            break;
            case 'e':
            is_encrypted = atoi(optarg);
            break;
        }
    }
	sprintf(backup_file_name, "%s_%d.%d.%d-%d.%d.%d",input_filename, tm->tm_year+1900,
			tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

    sprintf(cmd,"cp %s /tmp/orig-conf", input_filename);
    system(cmd);

    if (is_encrypted == 1) {
        sprintf(cmd, "openssl enc -d -aes-256-cbc -in %s -out %s  -pass file:key", argv[1], TEMP_DECRYPTED_FILE);
        system(cmd);
        strcpy(dec_filename, TEMP_DECRYPTED_FILE);
	} else {
        strcpy(dec_filename, input_filename);
    }
	fp = fopen(dec_filename, "r");

    while(fgets(str,sizeof(str),fp) != NULL) {
        lines++;
    }
    printf("Total number of lines in config files is = %d\n", lines);
    data = (MetaData_t *)malloc(lines * sizeof(MetaData_t));
    memset(data, 0, sizeof(*data));

    i = 0;
	rewind(fp);
    while(fgets(str,sizeof(str),fp) != NULL) {
		int len;
		char *ministr;
		int k = 0;
		if (str[0] == '#') {
            strncpy(data[i].comment, str, strlen(str) -1);
            i++;
			continue;
		}
		while(isspace(str[k]) || isblank(str[k])) {
			k++;
		}
		if (k > 0) {
			if ((str[k] == '#') || (k == strlen(str))) {
                strncpy(data[i].comment, str, strlen(str) -1);
                i++;
				continue;
            }
		}
		total_len = strlen(str);
        if (0/*empty == 1*/) {
            data[i].is_empty = 1;
        } else {

			ministr = strchr(str, '=');
			len = ministr - str;
			int k1;
			for (k1 = 0;k1 < len; k1++) {
				if ( isspace(str[k1]) || isblank(str[k1]))
					break;
				data[i].config_param_name[k1] = str[k1];
			}
            remaining_spaces_cnt= 0;
            remaining_spaces_cnt = ministr - &str[k1-1];
            strncpy(data[i].pre_string, &str[k1], remaining_spaces_cnt);
			ministr++;
			data[i].config_param_name[k1] = '\0';
			if((tmp = strchr(ministr, ';')) != NULL) {
				int len;
				len = tmp - ministr;
				strncpy(data[i].config_param_value,ministr,len);
                char *last = NULL;
                last = strchr(tmp, '\n');
                if (!last)
                    last = strchr(tmp, '\0');

				strncpy(data[i].post_string, tmp, last - tmp);

			} else if ((tmp = strchr(ministr, '#')) != NULL) {
				int m2 = 0, m1;
                for (m1=0;m1<tmp-ministr;m1++) {
					data[i].config_param_value[m2] = ministr[m1];
					m2++;
				}
				data[i].config_param_value[m2] = '\0';
                char *last = NULL;
                last = strchr(tmp, '\n');
                if (!last)
                    last = strchr(tmp, '\0');

				strncpy(data[i].post_string, tmp, last - tmp);
			} else if ((tmp = strchr(ministr, '\n')) != NULL) {
				int m2 = 0, m1;
                for (m1=0;m1<tmp-ministr;m1++) {
                    data[i].config_param_value[m2] = ministr[m1];
                    m2++;
                }
                data[i].config_param_value[m2] = '\0';
			}
		
        }
		i++;
    }
    fclose(fp);

    int lines_;
	FILE  *fp2;
	fp2 = fopen(backup_file_name, "w");
    for (lines_ = 0; lines_ < i; lines_++) {
		fprintf(fp2, "%s%s%s%s%s\n", data[lines_].config_param_name, data[lines_].pre_string, data[lines_].config_param_value, data[lines_].post_string, data[lines_].comment);
		fflush(fp2);
	}
    fclose(fp2);


	int LLL = i;
	int cc;

	char input[200];
	int z = 0, z1;
	int brk = 0;
	int found = 0;

	while(!brk) {

		printf("Press tab to get all possible configuration parameters\n OR \nStart typing config parameter name and press tab it will\nsuggest possible configuration parameter names\n");
		system ("/bin/stty raw");
		fflush(stdin);
		fflush(stdout);
		printf(RED "##################################$ " RESET);
		z = 0;
		memset(input, 0, sizeof(input));
		while (((cc = getchar()) != '\n') && !brk) {
			if(cc == '\r') {
				int y;
				char *sr= NULL, st[100]= {0};
				sr = strchr(input, '=');
				if (sr == NULL) {
					break;
				}
				for(y=0;y<LLL;y++) {
					strncpy(st, input, sr-input);
					if (!strcmp(st, data[y].config_param_name)) {
						strcpy(data[y].config_param_value, sr+1);
						break;
					}
				}
				break;
			}
			int l1 = 0;
			char found_str[500]= {0};
			found = 0;
			if (cc == '	') {

				for (z1 = 0; z1 < LLL; z1++) {
					if(((z > 0) && !strncmp(input, data[z1].config_param_name,z)) || (z == 0)) {
						//printf("%s\n",data[z1].config_param_name);
						found++;				
						strcpy(found_str, data[z1].config_param_name);
						if (strlen(data[z1].config_param_name) > 0)
						printf(RED "\r\n%s = %s" RESET, data[z1].config_param_name, data[z1].config_param_value);
					}
				}
                if (found == 0) {
					break;
				} else if (found == 1) {
					printf("\r\n");
					int fnt;
					for (fnt=0;fnt<strlen(input);fnt++) {
						printf("%c", '\b');
					}
					strcpy(input, found_str);
					z = strlen(found_str);
				} else {
					printf("\r\n");
				}
				printf("\r\n%s",input);
				system ("/bin/stty raw");
			} else {
                 input[z] = cc;
           		 z++;
            }
			if (!strcmp(input, "exit")) {
				brk = 1;
				break;
			}
		}
		system ("/bin/stty cooked");
		printf("\n");
        if (strcmp(input, "exit") || strcmp(input, ""))
    		printf("Configuring ( %s )\n", input);
	}
	system ("/bin/stty cooked");
	printf("\n");
	/*for (i=0;i<lines;i++) {
		printf("@@@@ %s =%s\n", data[i].config_param_name,data[i].config_param_value);
	}*/

    int lines1_;
    FILE  *fp3;
    if (is_encrypted == 1) {
    	fp3 = fopen(TEMP_DECRYPTED_MODIFIED_FILE, "w");
	    for (lines1_ = 0; lines1_ < LLL; lines1_++) {
    	    fprintf(fp3, "%s%s%s%s%s\n", data[lines1_].config_param_name, data[lines1_].pre_string, data[lines1_].config_param_value, data[lines1_].post_string, data[lines1_].comment);
			fflush(fp3);
	    }
		sprintf(cmd, "openssl enc -e -aes-256-cbc -in %s -out %s -pass file:key", TEMP_DECRYPTED_MODIFIED_FILE, input_filename);
		system(cmd);
    } else {
        fp3 = fopen(input_filename, "w");
    	for (lines1_ = 0; lines1_ < LLL; lines1_++) {
        	fprintf(fp3, "%s%s%s%s%s\n", data[lines1_].config_param_name, data[lines1_].pre_string, data[lines1_].config_param_value, data[lines1_].post_string, data[lines1_].comment);
		    fflush(fp3);
    	}
    }
}
