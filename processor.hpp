#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>

#define DEFAULT_K0 1
#define DEFAULT_K1 2
#define DEFAULT_K2 3
#define DEFAULT_R 2
#define DEFAULT_F 4
#define BUFSIZE 0x30000
#define HASHSIZE 128

using namespace std;
typedef struct _processor_inst_t {
	uint32_t instruction_address;
	int32_t op_code;
	int32_t src_reg[2];
	int32_t dest_reg;
	int32_t branch_addr;
	int32_t taken;
	int32_t pretaken;
	int32_t stage;		//stage 
	int16_t position;	//ALU MLU SLU positon
	int16_t state;		//finished or not
	int32_t fetch;
	int32_t dispatch;
	int32_t schedule;
	int32_t execute;
	int32_t update;
} processor_inst_t;

typedef struct _processor_stats_t {
	float avg_inst_fire;
	unsigned long fired_instruction;
	unsigned long retired_instruction;
	unsigned long issued_instruction;
	unsigned long cycle_count;
	unsigned long branch_instruction;
	unsigned long correct_predicted_instruction;
	float avg_ipc;
	float perc_branch_pred;
} processor_stats_t;

typedef struct _ALU {
	//processor_inst_t in;
	int instn;
	int available;
	int status;
	int begin, end;
} ALU;

typedef struct _MLU {
	//processor_inst_t in;
	int instn;
	int available;
	int status;
	int begin, end;
} MLU;

typedef struct _LSU {
	//processor_inst_t in;
	int instn;
	int available;
	int status;
	int begin, end;
} LSU;

typedef struct _CDB {
	//uint32_t reg;
	int instn;
	int available;
	int type;		//
	int device;		//alu mlu slu
	int status;
} CDB;


bool read_instructs(processor_inst_t * p_inst);

void setup_processor(uint64_t r, uint64_t k0, uint64_t k1, uint64_t k2,
		     uint64_t f);
void run_processor(processor_stats_t * p_stats);
void complete_processor(processor_stats_t * p_stats);

#endif				/* PROCESSOR_HPP */
