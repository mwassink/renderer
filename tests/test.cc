#include <stdio.h>




struct data {
    int failed;
    int succeeded;
    char* succeeded_functions[500];
    char* failed_functions[500];
    char* all_tests[500];
    char results[500];
    int total;
    FILE* fp;
};


struct data global_data;

void fail(char* failed_test) {
    global_data.failed_functions[global_data.failed++] = failed_test;
    global_data.all_tests[global_data.total] = failed_test;
    global_data.results[global_data.total++] = 0;
    printf("\nFAILED TEST %s\n", failed_test);
}
void success(char* success) {
    global_data.succeeded_functions[global_data.succeeded++] = success;
    global_data.all_tests[global_data.total] = success;
    global_data.results[global_data.total++] = 1;
    printf("\nSUCCEEDED TEST %s.\n", success);
}

void html_print_str_arr(int num, char** list, char* color) {
    for (int i = 0; i < num; ++i) {
        fprintf(global_data.fp, "<p style=\"color:%s;\"><b>%s</b></p>\n", color, list[i]);
    }
}

void html_print_all(int ignore_succ, int ignore_fails) {
    for (int i = 0; i < global_data.total; ++i) {
        if (global_data.results[i]) {
            if (!ignore_succ)
            fprintf(global_data.fp, "<p style=\"color:Black;\"><b style=\"color:Black\";>%s</b><span style=\"color: Green\";> &check;</span></p>\n", global_data.all_tests[i]);
        }
        else {
            if (!ignore_fails) {
                  fprintf(global_data.fp, "<p style=\"color:Black\";><b style=\"color:Black\";>%s</b> <span style=\"color: Red\";> &#x2717;</span></p>\n", global_data.all_tests[i]);
            }
        }
    }
}
    
void print_report(char* name, const char* title) {
    char* file_name = name;
    if (!name) file_name = "default.html";
    if (!title) title = "results";
    global_data.fp = fopen(file_name, "w");
    fprintf(global_data.fp, "<!DOCTYPE html>");
    fprintf(global_data.fp, "<html>\n<head>\n<title>%s</title>\n</head>\n", title);
    fprintf(global_data.fp, "<body>\n\n");
    fprintf(global_data.fp, "<h3> <b>------------------------------------------ </b></h3>");
    fprintf(global_data.fp, "<h1>Results</h1>\n");
    fprintf(global_data.fp, "<h3> <b>------------------------------------------ </b></h3>");
    fprintf(global_data.fp, "<h2 style=\"color:Red;\">Failures</h2>\n");
    html_print_all(1, 0);
    fprintf(global_data.fp, "<h2 style=\"color:Green;\">Successes</h2>\n");
    html_print_all(0,1);
    fprintf(global_data.fp, "<h3> <b>------------------------------------------ </b></h3>");
    fprintf(global_data.fp, "<h1>Summary</h1>\n");
    fprintf(global_data.fp, "<h3> <b>------------------------------------------ </b></h3>");
    html_print_all(0, 0);
    fprintf(global_data.fp, "<h3>Passed a total of %d of %d tests</h3>\n", global_data.succeeded, global_data.total);
    
    fprintf(global_data.fp, "</body>\n");
    fprintf(global_data.fp, "</html>");
    fclose(global_data.fp);
}
