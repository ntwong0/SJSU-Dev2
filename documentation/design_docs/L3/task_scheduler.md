# FreeRTOS Task Scheduler Design Document

- [FreeRTOS Task Scheduler Design Document](#freertos-task-scheduler-design-document)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [API](#api)
  - [Configuring the Scheduler Size](#configuring-the-scheduler-size)
  - [Adding a Task to the Scheduler](#adding-a-task-to-the-scheduler)
  - [Starting the Scheduler](#starting-the-scheduler)
  - [Task Execution](#task-execution)
  - [Retrieving a Task](#retrieving-a-task)
  - [Removing a Task from the Scheduler](#removing-a-task-from-the-scheduler)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme](#unit-testing-scheme)
  - [Demonstration Project](#demonstration-project)

# Location
`L3 Application`

# Type
Implementation

# Background
The `TaskScheduler` along with the
[`Task interface`](/documentation/design_docs/L3/task_interface.md) provide an
abstraction layer for the management of FreeRTOS tasks.

# Overview
The TaskScheduler is a FreeRTOS task scheduler responsible for scheduling tasks
that inherit the `Task` interface. The scheduler utilizes the FreeRTOS Event
Group to ensure all tasks have been successfully initialized before proceeding
to the execution any task.

# Detailed Design
## API
```c++
namespace sjsu
{
namespace rtos
{
class TaskInterface;

class TaskSchedulerInterface
{
 public:
  virtual EventGroupHandle_t GetPreRunEventGroupHandle() const = 0;
  virtual EventBits_t GetPreRunSyncBits() const                = 0;
  virtual uint8_t GetTaskCount() const                         = 0;
  virtual void AddTask(TaskInterface * task)                   = 0;
  virtual void RemoveTask(const char * task_name)              = 0;
  virtual TaskInterface * GetTask(const char * task_name)      = 0;
  virtual uint8_t GetTaskIndex(const char * task_name) const   = 0;
  virtual TaskInterface * const * GetAllTasks() const          = 0;
};

class TaskScheduler : public TaskSchedulerInterface
{
 public:
  TaskScheduler();
  EventGroupHandle_t GetPreRunEventGroupHandle() const override;
  EventBits_t GetPreRunSyncBits() const override;
  uint8_t GetTaskCount() const override;
  void AddTask(TaskInterface * task) override;
  void RemoveTask(const char * task_name) override;
  TaskInterface * GetTask(const char * task_name) override;
  uint8_t GetTaskIndex(const char * task_name) const override;
  TaskInterface * const * GetAllTasks() const override;
  void Start();

 private:
  static void RunTask(void * task_pointer);
  void InitializeAllTasks();

  TaskInterface * task_list_[config::kTaskSchedulerSize];
  uint8_t task_count_;
  StaticEventGroup_t pre_run_event_group_buffer_;
  EventGroupHandle_t pre_run_event_group_handle_;
  EventBits_t pre_run_sync_bits_;
};
}  // namespace rtos
}  // namespace sjsu
```

## Configuring the Scheduler Size
By default the `TaskScheduler`'s size is configured to schedule a maximum of 16
tasks in `config.hpp`.
```c++
#if !defined SJ2_TASK_SCHEDULER_SIZE
#define SJ2_TASK_SCHEDULER_SIZE 16
#endif // !defined SJ2_TASK_SCHEDULER_SIZE
SJ2_DECLARE_CONSTANT(TASK_SCHEDULER_SIZE, uint32_t, kTaskSchedulerSize);
```
> **Note**: This configuration can be changed by defining your own
> `SJ2_TASK_SCHEDULER_SIZE` in the `project_config.hpp`.

## Adding a Task to the Scheduler
All tasks inheriting the `Task` interface is automatically added to the
scheduler when constructed.

```c++
void AddTask(TaskInterface * task) override
```
Adds a task to the scheduler. If the scheduler is full, an error is asserted to
recommend an increase in the default task scheduler size.

## Starting the Scheduler
The `TaskScheduler` can be started by invoking the `Start()` method.

```c++
void Start() override
```
Invoked to start the scheduler. The following sequence is performed:
1. Check the task_list_ to ensure that there is at least one task added to the
   scheduler. If no tasks are added, output a warning message to notify the user
   that there a currently no scheduled tasks.
2. Invoke `InitializeAllTasks()` to initialize all scheduled tasks.
3. Invoke `vTaskStartScheduler()` to start executing the scheduled tasks.

```c++
void InitializeAllTasks() override
```
The following sequence is performed when initializing scheduled tasks:
1. Statically create a FreeRTOS task through the `xTaskCreateStatic()` method
   for each task scheduled in the `task_list_`.
2. Check the returned handle from `xTaskCreateStatic()`. If the handle is a
   `nullptr`, then the task was created unsuccessfully and an error is asserted.
3. If the task was created successfully, invoke `Setup()` of each task. If a
   task fails to complete `Setup()`, terminate initialization and assert an
   error through `SJ2_ASSERT_FATAL`.
4. If the task was successfully created and successfully completes `Setup()`, set
   the task’s event sync bit in `pre_run_sync_bits_` based on the task’s index
   position in `task_list_`.
5. Statically create the Event Group handler.

## Task Execution
Each task is responsible for completing its own `PreRun()` and wait for the
`PreRun()` of all other tasks to be completed before continuing to execute
`Run()`.

```c++
EventGroupHandle_t pre_run_event_group_handle_;
StaticEventGroup_t pre_run_event_group_buffer_;
EventBits_t pre_run_sync_bits_;
```
A FreeRTOS Event Group is used to synchronize the `PreRun()` of each task. When
a task completes its `PreRun()` it broadcasts a notification by setting its sync
bit and waits for all other task sync bits to be set.

More information regarding FreeRTOS Event Groups can be found
[here](https://www.freertos.org/FreeRTOS-Event-Groups.html).

```c++
static void RunTask(void * task_pointer)
```
The following sequence is performed, where Step 1 is only performed once:
1. Get the PreRun Event Group handle and determine the running task’s sync bit
   by retrieving its task index from the scheduler's task list.
2. Invoke the task’s `PreRun()` and assert a fatal error if the task’s
   `PreRun()` fails to complete. If PreRun() completes successfully, set the
   task’s sync bit in the Event Group and wait for all other task’s sync bits
   to be set.
4. When all sync bits are set, `Run()` can now begin. If a desired delay time is
   set, invoke `vTaskDelayUntil()`.

## Retrieving a Task
```c++
TaskInterface * GetTask(const char * task_name) override
```
Returns a pointer reference of the task by its task name. If the task does not
exist in the scheduler, a `nullptr` is returned.

```c++
uint8_t GetTaskIndex(const char * task_name) const override
```
Returns the index of a task in the `task_list_` by the provided `task_name`.
This method is used by the scheduler the manage a task's PreRun Event Group sync
bit. If the task does not exist in the scheduler, the maximum task count + 1 is
returned. For example, if the maximum task count is 16, then 17 is returned if
the task does not exist.

```c++
TaskInterface * const * GetAllTasks() const override
```
Returns a pointer reference to an immutable array of all currently scheduled
tasks.

## Removing a Task from the Scheduler
```c++
void RemoveTask(const char * task_name) override
```
The following is performed to remove a specified task by its `task_name`:
1. Retrieves the task using `GetTask()` and removes the task by invoking
  `vTaskDelete()`.
2. Sets the reference in `task_list_` to `nullptr`.
3. Decrement the `task_count_`.

# Caveats
N/A

# Future Advancements
N/A

# Testing Plan
## Unit Testing Scheme
A total of 17 tasks shall be mocked and stubbed to be used as test inputs for
the scheduler. Additionally the following methods from the FreeRTOS library
shall be faked:
- `vTaskCreateStatic()`
- `xEventGroupCreateStatic()`
- `vTaskStartScheduler()`
- `vTaskDelete()`

The following functions of the TaskScheduler are tested:
- `AddTask()`
  1. Each mock task shall be added to the scheduler.
     - The task count should be incremented for each added task.
     - The task count should not exceed `16`.
- `GetTask()`
  1. Tasks 1-17 shall be added to the scheduler, with each addition:
     - Should return a scheduled task corresponding to the specified task name.
  2. Task named "Task 17" shall be retrieved.
     - Should return `nullptr`.
- `GetTaskIndex()`
  1. Tasks 1-17 shall be added to the scheduler, with each addition:
     - Should return the index of the task with the matching task name of the
       task that is being added.
     - The task count should be incremented by 1.
  2. The index of task named "Does not exist" shall be retrieved.
     - Should return `17`.
- `RemoveTask()`
  1. The scheduler shall be empty, and a task named "Task A" shall be removed:
     - `vTaskDelete()` should have a call count of `0`.
  2. Mock tasks named "Task 1", "Task 2", "Task 3", and "Task 4" shall be added
     to simulate a scheduler that is not empty:
     - `GetTaskCount()` should return `4`.
     - The mock task at index = 2, "Task 3", shall be removed:
       - `vTaskDelete()` should have a call count of `1`.
       - Total task count should be `3`.
       - Checking index 2 of the task list should be a `nullptr`.
- `Start()`
  1. Tasks 1-16 shall be added to the scheduler:
     - Each mock task's `Setup()` should be invoked once.
     - `vTaskCreateStatic()` should have a call count of `16`.
     - `xEventGroupCreateStatic()` should have a call count of `1`.
     - `GetPreRunEventGroupHandle()` should return the dummy event group
       handler.
     - `GetPreRunSyncBits()` should return `0xFFFF`.
     - `vTaskStartScheduler()` should have a call count of `1`.

## Demonstration Project
A demonstration project can be found
[here](/demos/multiplatform/task_scheduler/source/main.cpp).
