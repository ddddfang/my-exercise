#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thinfat32.h"

#define NO_ERROR 0
#define FILE_OPEN_ERROR -1
#define DATA_READ_ERROR -2
#define DATA_WRITE_ERROR -3
#define DATA_MISMATCH_ERROR -4

int basic_read(char *input_file) {
    TFFile *fp;
    char data[1024];
    fp = tf_fopen(input_file, "r");
    if (!fp) {
        printf("open error\r\n");
        return FILE_OPEN_ERROR;
    }
    int i = 0;
    printf("read result:\r\n");

    memset(data, 0, 1024);
    while((i = tf_fread(data, 1023, fp)) > 0) {
        printf("%s\r\n", data);
        memset(data, 0, 1024);
    }
    tf_fclose(fp);
    return NO_ERROR;
}

int test_basic_read(char *input_file, char *expected) {
    TFFile *fp;
    char data[20480];
    fp = tf_fopen(input_file, "r");
    int i = 0;

    if(fp) {
        while(tf_fread(&(data[i]), 1, fp) > 0) {
            i += 1;
        }
        data[i+1] = '\x00';
        if(strcmp(data, expected)) {
            tf_fclose(fp);
            printf("\r\n\r\nexpected: %s\r\n", expected);
            printf("\r\n\r\nreceived: %s\r\n", data);
            return DATA_MISMATCH_ERROR;
        } else {
            tf_fclose(fp);
            return NO_ERROR;
        }
    } else {
        return FILE_OPEN_ERROR;
    }
}


int test_basic_write(char *input_file, char *write_string) {
    TFFile *fp;
    int rc;

    fp = tf_fopen(input_file, "w");

    if(fp) {
        printf("\r\n[TEST] writing data to file: %s", write_string);
        rc = tf_fwrite(write_string, strlen(write_string), 1, fp);
        if(rc < 1) {
            tf_fclose(fp);
            return DATA_WRITE_ERROR;
        } else {
            tf_fclose(fp);
            return NO_ERROR;
        }
    } else {
        return FILE_OPEN_ERROR;
    }
}

int test_basic_append(char *input_file, char *write_string) {
    TFFile *fp;
    int rc;

    fp = tf_fopen(input_file, "a");

    if(fp) {
        rc = tf_fwrite(write_string, 1, strlen(write_string), fp);
        if(rc) {
            tf_fclose(fp);
            return DATA_WRITE_ERROR;
        } else {
            tf_fclose(fp);
            return NO_ERROR;
        }
    } else {
        return FILE_OPEN_ERROR;
    }
}

int main(int argc, char **argv) {
    int rc;
    char *ws = "Clock running-time A clock returns the absolute-time according \
    to that clock with gst_clock_get_time (). From the absolute-time is a running-time calculated,\
    which is simply the difference between a previous snapshot of the absolute-time called the base-time. So:\
    running-time = absolute-time - base-time A GStreamer GstPipeline object maintains \
    a GstClock object and a base-time when it goes to the PLAYING state. \
    The pipeline gives a handle to the selected GstClock to each element in\
    the pipeline along with selected base-time. The pipeline will select a base-time in\
    such a way that the running-time reflects the total time spent in the PLAYING state.\
    As a result, when the pipeline is PAUSED, the running-time stands still.\
    Because all objects in the pipeline have the same clock and base-time, they can thus all calculate\
    the running-time according to the pipeline clock.Buffer running-time\
    To calculate a buffer running-time, we need a buffer timestamp and the SEGMENT \
    event that preceded the buffer. First we can convert the SEGMENT event into a \
    GstSegment object and then we can use the gst_segment_to_running_time () function \
    to perform the calculation of the buffer running-time.Synchronization is now \
    a matter of making sure that a buffer with a certain running-time is played when the clock \
    reaches the same running-time. Usually this task is done by sink elements. \
    Sink also have to take into account the latency configured in the pipeline and \
    add this to the buffer running-time before synchronizing to the pipeline clock.\
    Obligations of each element.Let us clarify the contract between GStreamer and each element \
    in the pipeline.Non-live source elements Non-live source elements must place a\
    timestamp in each buffer that they deliver when this is possible.\
    They must choose the timestamps and the values of the SEGMENT event in such a way that the running-time\
    of the buffer starts from 0.Some sources, such as filesrc, is not able to generate timestamps on all buffers.\
    It can and must however create a timestamp on the first buffer (with a running-time of 0).\
    The source then pushes out the SEGMENT event followed by the timestamped buffers.Live source elements\
    Live source elements must place a timestamp in each buffer that they deliver. \
    They must choose the timestamps and the values of the SEGMENT event in such a way that the \
    running-time of the buffer matches exactly the running-time of the pipeline clock when the first byte in the buffer was captured.";

    printf("\r\nFAT32 Filesystem Test\r\n");
    rc = tf_init();

    printf("tf_init return %d\r\n", rc);

    if(rc = basic_read("/subdir/fang.txt")) {// /readers/abc.txt
        printf("failed with error code 0x%x", rc) ;
    } else {
        printf(" PASSED.\r\n");
    }

    printf("\r\n[TEST] Basic LFN write test ") ;
    if(rc = test_basic_write("/test_longfilename0.txt", ws)) {
        printf("failed with error code 0x%x", rc) ;
    } else {
        printf(" PASSED.\r\n");
    }


    printf("\r\n[TEST] Basic LFN read test ") ;
    if(rc = test_basic_read("/test_longfilename0.txt", ws)) {
        printf("failed with error code 0x%x", rc) ;
    } else {
        printf(" PASSED.\r\n");
    }


    if(rc = test_basic_write("/test0.txt", "Hello, World!")) {
        printf("\r\n[TEST] Basic 8.3 write test failed with error code 0x%x", rc);
    } else {
        printf("\r\n[TEST] Basic 8.3 write test PASSED.");
    }


    if(rc = test_basic_read("/test0.txt", "Hello, World!")) {
        printf("\r\n[TEST] Basic 8.3 read test failed with error code 0x%x", rc) ;
    } else {
        printf("\r\n[TEST] Basic 8.3 read test PASSED.\r\n");
    }
    return 0;
}

