This readme combines Q2 and Q3 
Q2 please refer to line 11
Q3 please refer to line 120

Attention: Please wait patiently for output. 
Assume <Br Taken> = 1 means branch taken; <Br Taken> = 0 means branch untaken
Assume no forwarding in Q.3.

Q2
processor.cpp
Idea:
  Set up global queue variable for four stages:
    queue<int> s0,s1,s2,s3,s4;
    queue<int> t0,t1,t2,t3,t4;
  The stage queue(s*) holds instructions at different stages. 
  The time queue(t*) holds time when the instruction starts execution.
  Pass each instruction to different stages one by one.
  Common actions done in each stage:
    Do nothing if queue of corrsponding stage is empty in order to prevent segmentation fault.
    Execute the instruction if its time tag is earlier than current clock (clocknum).
    Increment issued instruction/retired instruction/fired instruction count, etc. for displaying statstics.


Divide clock into first half and second half to simulate the behavior of the pipeline latches.
First half: fetch, dispatch. clock number is even.
Second half: schedule, execute, update. clock number is odd.

Brief introduction of functions in processor.cpp:
1. setup_processor:
  Set up queues for all functional units.

2.read_instructs:
  Read instruction file from *.trace into instruction array.

Repeat following steps until all instructions are processed.

1. fetch_inst_s0:
  Fetch instructions from instruction array, one cycle one instruction. 

2. dispatch_inst_s1:
  Decode instructions, then, distribute instrutions to relevant functional unit.
  
3. schedule_inst_s2:
  According to Tomasulo algorithm, the instructions are mark to exec if there is no data dependency. 
  Lower address of instructions have higher priority if there is competition.
  Avoid data hazards.

4. exec_inst_s3:
  Call corresponding functional units to execute instruction, record starting time and ending time of execution.
  Push this instruction to update stage if execution ends.
  Latency of FU type 0, 1, 2 are 1, 3, 10 cycles respectively.
  All the functional units are pipelined.
  updates the reservation stations if execution finishes.


5. update_inst_s4:
  Update finished state of instruction. e.g. register file.

6. cdb_bcast:
  If execution in functional unit is finished, boardcast status of corresponding resources available.
  This function is called by update stage: update_inst_s4.

7. cdb_logic
  Detect awaiting instruction(s), monitor the CDB; if the operands to the corresponding slots are available inform
  to put the instruction to execute. Avoid data hazard. 

processor.hpp: Meaning of variables declared are explained below.

  typedef struct _processor_inst_t
  {
    uint32_t instruction_address; //address of instruction.
    int32_t op_code; //Opcode of the instruction.
    int32_t src_reg[2]; //source register(s) required by the instruction.
    int32_t dest_reg; //destination register required by the instruction.
    int32_t stage; //which stage is the instruction in.
    int16_t position; //ALU MLU SLU position
    int16_t state; // useless
    int32_t fetch; //when is the instruction fetched.
    int32_t dispatch; //when is the instruction displatched.
    int32_t schedule; //when is the instruction scheduled.
    int32_t execute; //when is the instruction executed.
    int32_t update; //when is the instruction updated.
  } processor_inst_t;

  typedef struct _ALU {
    int instn; //instruction that the ALU is handling.
    int available; //availability of ALU.
    int status; //status = 1: Execution starts; status = 2: executioin ends.
    int begin,end; //When does ALU starts execution and ends execution.
  } ALU;

  typedef struct _MLU {
    int instn; //Same explanation as struct _ALU
    int available;
    int status;
    int begin,end;
  } MLU;

  typedef struct _LSU {
    int instn; //Same explanation as struct _ALU
    int available;
    int status;
    int begin,end;
  } LSU;


typedef struct _processor_stats_t {
        float avg_inst_fire;
        unsigned long fired_instruction;  // count for fired instruction
        unsigned long retired_instruction; // count for retired instruction
        unsigned long issued_instruction;  // count for issued instruction
        unsigned long cycle_count;         // count for cycle
        unsigned long branch_instruction;   // count for branch instruction
        unsigned long correct_predicted_instruction;  // count for correct predicted instruction
        float avg_ipc;  // useless
        float perc_branch_pred;  //useless
} processor_stats_t;



---------------------------------------------------------------------------------
Q3

Idea:
 Remove least recent record from hash table if hash table is full.
 Combine partial address and global branch history to form a catalog. Each catalog has initial value of 01 (binary).
 If more than one branch exits, serve them in the lowest tag first.
 If a branch instruction is encountered, then find corresponding catalog to predict outcome of branch.
 Update the predictor when resource is freed.
 Dispatch and fetch will stall if there is mispredicted branch.
 Branch outcome known after 1 cycle of execution.
 After the miss-predicted branch has performed state update, then resume dispatch.
 Pipeline continues if branch prediction is correct.
 Branches are predicted in the Instruction Fetch stage.

New functions:
1. fetch_inst_s0:
  Predict branch outcome if the instruction is branch.

2. add prediction table "pretab" 
  Get an index into the Branch Target Buffer using this function: ((Address >> 2) % 16) | GHR
  remove lease recent record from hash table if hash table is full.

3. update_pretab:
  predict branch outcome.
  If previous prediction is correct, then keep prediction unchanged.
  Otherwise, change prediction by two-bit prediction scheme.

Stage 0:
  Add address to the prediction table by calling function add_pretab or update_pretab when information
  received.

Stage 3 (Execution):
  If prediction is correct, then continue execution, keep everything unchanged.
  Otherwise, boardcast to clear queue, keep instructions whose address is lower than branch instruction and 
  remove instructions whose address is higher than branch instruction (done by function "rerange_q", "clear_inst").
  fetch_inst_s0 will restart address from boardcast information.







