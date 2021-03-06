#include <allocators.h>

#include <rcl/rcl.h>
#include <rcl_action/rcl_action.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/int32.h>
#include "example_interfaces/srv/add_two_ints.h"

#include <rmw_uros/options.h>

#include <stdio.h>
#include <unistd.h>

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);  vTaskDelete(NULL);;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

void appMain(void *argument)
{
  rcl_init_options_t options = rcl_get_zero_initialized_init_options();

  RCCHECK(rcl_init_options_init(&options, rcl_get_default_allocator()))

  // Optional RMW configuration 
  rmw_init_options_t* rmw_options = rcl_init_options_get_rmw_init_options(&options);
  RCCHECK(rmw_uros_options_set_client_key(0xBA5EBA11, rmw_options))

  rcl_context_t context = rcl_get_zero_initialized_context();
  RCCHECK(rcl_init(0, NULL, &options, &context))

  rcl_node_options_t node_ops = rcl_node_get_default_options();

  rcl_node_t node = rcl_get_zero_initialized_node();
  RCCHECK(rcl_node_init(&node, "int32_publisher", "", &context, &node_ops))

  rcl_publisher_options_t publisher_ops = rcl_publisher_get_default_options();
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  RCCHECK(rcl_publisher_init(&publisher, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), "/int32_publisher", &publisher_ops))

  std_msgs__msg__Int32 msg;
  msg.data = 0;
  
  printf("Free heap post uROS configuration: %d bytes\n", xPortGetFreeHeapSize());
  printf("uROS Used Memory %d bytes\n", usedMemory);
  printf("uROS Absolute Used Memory %d bytes\n", absoluteUsedMemory);

  rcl_ret_t rc;
  do {
    rcl_ret_t rc = rcl_publish(&publisher, (const void*)&msg, NULL);

    if (RCL_RET_OK == rc ) {
        printf("Sent: '%i'\n", msg.data++);
    }

    usleep(10000);
  } while (true );

  RCCHECK(rcl_publisher_fini(&publisher, &node))
  RCCHECK(rcl_node_fini(&node))

  vTaskDelete(NULL);
}