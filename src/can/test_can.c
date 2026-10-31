#include "utils.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

int test_ack_status_to_str() {
    int errors = 0;
    if (strcmp(ack_status_to_str(ACK_OK), "OK") != 0){
        errors++;
    }
    if (strcmp(ack_status_to_str(ACK_ERR), "ERR") != 0){
        errors++;
    }

    return errors;
}
int test_str_bool() {
    int errors = 0;
    if (strcmp(str_bool(true), "True") != 0){
        errors++;
    }
    if (strcmp(str_bool(false), "False") != 0){
        errors++;
    }

    return errors;
}


int main(void) {
    int errors = 0;
    errors += test_ack_status_to_str();
    errors += test_str_bool();
    if (errors == 0){
        printf("All tests passed\n");
        return 0;
    }

    printf("%d errors occured during tests\n", errors);

}