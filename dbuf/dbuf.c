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

#define DBUFFER_TEST
#ifdef DBUFFER_TEST

#include <stdio.h>
#include <stdbool.h>
#include "dbuf.h"


/* Definitions */
/* In header */

dbuffer_op_result dbuffer_init(dbuffer_struct_t * self)
{
    int i;
    
    for (i = 0; i < DBUFFER_NUM ; i++)
    {
        self->trigger_func[i] = NULL;
        self->numElems[i]     = 0;
        self->status[i]       = DBUFFER_STATUS_AVAILABLE;
        
        self->buffer[i][0] = '\0';
    }
    
    self->triggerValue = DBUFFER_TRIGGER;
    self->currentIndex = 0;
   
    self->get_trigger = &getTriggerImpl;
    self->set_trigger = &setTriggerImpl;
    self->write_char_array = &writeCharArrayImpl;
    self->lockCurrent = &lockCurrentImpl;
    self->lockIndex = &lockIndexImpl;
    self->unlockIndex = &unlockIndexImpl;
    self->setNextBuffer = &setNextBufferImpl;
    
    
    
    return 1;
}

  static int getTriggerImpl(dbuffer_struct_t * self)
 {
     printf("%s\n", __func__);
     return self->triggerValue;
 }
 
  static dbuffer_op_result setTriggerImpl(dbuffer_struct_t * self, int value)
 {
      self->triggerValue = value;
     return DBUFFER_OP_OK;
 }
  
  static dbuffer_op_result writeCharArrayImpl(dbuffer_struct_t * self, char * buf, size_t len)
 {
      int i;
      if (db_would_overflow(self, len))
      {
          return DBUFFER_OP_WOULD_OVERFLOW;
      }
      
      if ((self->status[self->currentIndex]) == DBUFFER_STATUS_LOCKED)
      {
          printf("Oops, current index is set at %d, this buffer seems busy\n", self->currentIndex);
          return DBUFFER_OP_EL_BUSY;
      }
      
      for (i = 0; i < len; i++)
      {
          self->buffer[self->currentIndex][db_num_elts_in_current_buffer(self) + i] = buf[i];
      }
      
      self->numElems[self->currentIndex] += len;
#if 1
      if (self->numElems[self->currentIndex] >= self->triggerValue)
      {
          //printf("triggered index:%d %d %d\n", self->currentIndex, self->triggerValue, self->numElems[self->currentIndex]);
          self->trigger_func[self->currentIndex](self);
          
      }
#endif
     return DBUFFER_OP_OK;
 }
  
  static bool db_would_overflow(const dbuffer_struct_t * self, size_t len)
  {
      return ( (len > DBUFFER_SIZE) || 
                    ((len + db_num_elts_in_current_buffer(self))> DBUFFER_SIZE));
  }
  
  static size_t db_num_elts_in_current_buffer(const dbuffer_struct_t * self)
  {
      return self->numElems[self->currentIndex];
  }
#if 1
  static dbuffer_op_result setNextBufferImpl (struct dbuffer_struct_t * self)
  {
      self->currentIndex++;
      self->currentIndex %= 2;
      return DBUFFER_OP_OK;
  }
  
  static dbuffer_op_result lockCurrentImpl (struct dbuffer_struct_t * self)
  {
      self->status[self->currentIndex] = DBUFFER_STATUS_LOCKED;
      return DBUFFER_OP_OK;
  }
  
  static dbuffer_op_result lockIndexImpl (struct dbuffer_struct_t * self, int n)
  {
      //test if n > num of buf
      self->status[n] = DBUFFER_STATUS_AVAILABLE;
      return DBUFFER_OP_OK;
  }
  
  
  static dbuffer_op_result unlockIndexImpl (struct dbuffer_struct_t * self, int n)
  {
      //test if n > num of buf
      self->status[n] = DBUFFER_STATUS_AVAILABLE;
      return DBUFFER_OP_OK;
  }
#endif
  
/*
dbuffer_op_result dbuffer_init(dbuffer_struct_t *dbp)
{
    int i;
    for (i = 0; i < DBUFFER_NUM; i++)
    {
        memset(dbp->buffer + i*(DBUFFER_SIZE+1), 0, DBUFFER_SIZE);
    }
    
}
*/


#ifdef DBUF_TEST
void test_callback_0(dbuffer_struct_t * self);
void test_callback_1(dbuffer_struct_t * self);
  
int dbuffer_test()
{
    int a;
    int index0, index1; 
    
    const int setting_value = 5;
    dbuffer_struct_t dbuffer_test;
    
    printf("After declaration\n");
    
    dbuffer_init(&dbuffer_test);
    printf("After init\n");
    dbuffer_test.trigger_func[0] = test_callback_0;
    dbuffer_test.trigger_func[1] = test_callback_1;
    
    
    a = dbuffer_test.get_trigger(&dbuffer_test);
    printf("Polling trigger value / result: %d\n", a);
    
    printf("Setting trigger value to %d...\n", setting_value);
    dbuffer_test.set_trigger(&dbuffer_test, setting_value);
    
     a = dbuffer_test.get_trigger(&dbuffer_test);
    printf("Polling trigger value / result: %d\n", a);
    
    printf("Current index is %d\n", dbuffer_test.currentIndex);
    
    
    dbuffer_test.write_char_array(&dbuffer_test, "test12345", sizeof("test12345"));
    dbuffer_test.write_char_array(&dbuffer_test, "test12345", sizeof("test12345"));
    
    dbuffer_test.write_char_array(&dbuffer_test, "test123456789", sizeof("test123456789")); 
    index0 = dbuffer_test.get_trigger(&dbuffer_test);
    dbuffer_test.unlockIndex(&dbuffer_test, 0);
    
    dbuffer_test.write_char_array(&dbuffer_test, "test123456789", sizeof("test123456789")); 
   
    return 1;
}

void test_callback_0(dbuffer_struct_t * self)
{
    int rxdIndex;
    
    rxdIndex = self->currentIndex;
    printf("rxd index: %d\n", rxdIndex);
    printf("Locking\n");
    self->lockCurrent(self);

    printf("Setting next buffer\n");
    self->setNextBuffer(self);
    
    printf("Send RTOS Notification to the SD Write task\n");
    
    printf("Index is now: %d\n", self->currentIndex);
    
    printf("%s\n", __func__);
    
}

void test_callback_1(dbuffer_struct_t * self)
{
    int rxdIndex;
    
    rxdIndex = self->currentIndex;
    printf("rxd index: %d\n", rxdIndex);
    printf("Locking\n");
    self->lockCurrent(self);

    printf("Setting next buffer\n");
    self->setNextBuffer(self);
    
    printf("Index is now: %d\n", self->currentIndex);
    
    printf("%s\n", __func__);
}
#endif

#endif
