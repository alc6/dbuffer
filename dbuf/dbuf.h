/*
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef DBUF_H
#define DBUF_H

#include <stddef.h>
#include <stdbool.h>

#define DBUF_TEST

#ifdef __cplusplus
extern "C" {
#endif

#define DBUFFER_SIZE     1024           /* 1024 bytes for each buffer */
#define DBUFFER_NUM 	   2		/* 2 buffers */
#define DBUFFER_TRIGGER  800

   
     
/* Result to operations */
typedef enum {
    DBUFFER_OP_OK,
    DBUFFER_OP_WOULD_OVERFLOW,
    DBUFFER_OP_EL_BUSY,        
    DBUFFER_OP_ERROR
} dbuffer_op_result;

/* Status of a buffer : */
typedef enum {
    DBUFFER_STATUS_AVAILABLE,      /* Free to write on */
    DBUFFER_STATUS_LOCKED,           /* Busy, reading operations */
    DBUFFER_STATUS_ERROR           /* Error occurred, unavailable buffer */
} dbuffer_status;

typedef struct dbuffer_struct_t{
    char    buffer[DBUFFER_NUM][DBUFFER_SIZE];
    size_t  numElems[DBUFFER_NUM];
    dbuffer_status status[DBUFFER_NUM];
    
    void (*trigger_func[DBUFFER_NUM])(); /* Pointers of functions to call if trigger is reached */
    
    int triggerValue;
    int currentIndex;
    
    
    int               (*get_trigger)(struct dbuffer_struct_t *     );
    dbuffer_op_result (*set_trigger)(struct dbuffer_struct_t *, int);
    
    dbuffer_op_result (*set_next_buffer)(struct dbuffer_struct_t *);
    
    dbuffer_op_result (*write_char_array)(struct dbuffer_struct_t *, char *, size_t);
    
    dbuffer_op_result (*unlockIndex)(struct dbuffer_struct_t *, int);
    dbuffer_op_result (*lockIndex)(struct dbuffer_struct_t *, int);
    dbuffer_op_result (*setNextBuffer)(struct dbuffer_struct_t *);
    dbuffer_op_result (*lockCurrent)(struct dbuffer_struct_t *);
    
} dbuffer_struct_t ;



static int getTriggerImpl(dbuffer_struct_t * self);
static dbuffer_op_result setTriggerImpl(dbuffer_struct_t * self, int value);

static size_t            db_num_elts_in_current_buffer(const dbuffer_struct_t * self);
static dbuffer_op_result writeCharArrayImpl(dbuffer_struct_t * self, char * buf, size_t len);
static bool              db_would_overflow(const dbuffer_struct_t * self, size_t len);
static dbuffer_op_result unlockIndexImpl (struct dbuffer_struct_t * self, int n);
static dbuffer_op_result lockIndexImpl (struct dbuffer_struct_t * self, int n);

static dbuffer_op_result lockCurrentImpl (struct dbuffer_struct_t * self);
static dbuffer_op_result setNextBufferImpl (struct dbuffer_struct_t * self);


#ifdef DBUF_TEST
    void dbuffer_test_error_handler();
    int dbuffer_test();
#endif


#ifdef __cplusplus
}
#endif

#endif /* DBUF_H */
