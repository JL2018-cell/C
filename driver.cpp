#include <cstdio>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "processor.hpp"

FILE *inFile = stdin;
extern processor_inst_t instset[BUFSIZE];
extern uint64_t instn;
extern queue < int >disp_queue;

void print_help_and_exit(void)
{
	printf("procsim [OPTIONS]\n");
	printf("  -k k0\t\tNumber of k0 FUs\n");
	printf("  -l k1\t\tNumber of k1 FUs\n");
	printf("  -m k2\t\tNumber of k2 FUs\n");
	printf("  -f \t\tNumber of instructions to fetch\n");
	printf("  -r \t\tNumber of reservation stations \n");
	printf("  -i traces/file.trace\n");
	printf("  -h\t\tThis helpful output\n");
	exit(0);
}

//
// read_instructs
//
//  returns true if an instruction was read successfully
//
bool read_instructs(processor_inst_t * p_inst)
{
	int ret;
	char buf[256];

	if (p_inst == NULL) {
		fprintf(stderr,
			"Fetch requires a valid pointer to populate\n");
		return false;
	}
	if (fgets(buf, 256, stdin) == NULL)
		return false;
	ret =
	    sscanf(buf, "%x %d %d %d %d %x %d\n",
		   &p_inst->instruction_address, &p_inst->op_code,
		   &p_inst->dest_reg, &p_inst->src_reg[0],
		   &p_inst->src_reg[1], &p_inst->branch_addr,
		   &p_inst->taken);
	if (p_inst->op_code < 0)
		p_inst->op_code = -p_inst->op_code;
	p_inst->stage = p_inst->state = p_inst->position = -1;
	if (ret == 5 || ret == 7) {
		return true;
	}
	return false;
}

void print_statistics(processor_stats_t * p_stats);


int main(int argc, char *argv[])
{
	int opt;
	uint64_t f = DEFAULT_F;
	uint64_t k0 = DEFAULT_K0;
	uint64_t k1 = DEFAULT_K1;
	uint64_t k2 = DEFAULT_K2;
	uint64_t r = DEFAULT_R;

	/* Read arguments */
	// –f 2 –k 4 –l 6 -m 7 -r 3
	while ((opt = getopt(argc, argv, "f:k:l:m:r:i:h")) != -1) {
		switch (opt) {
		case 'r':
			r = atoi(optarg);
			break;
		case 'k':
			k0 = atoi(optarg);
			break;
		case 'l':
			k1 = atoi(optarg);
			break;
		case 'm':
			k2 = atoi(optarg);
			break;
		case 'f':
			f = atoi(optarg);
			break;
		case 'i':
			inFile = fopen(optarg, "r");
			if (inFile == NULL) {
				fprintf(stderr,
					"Failed to open %s for reading\n",
					optarg);
				print_help_and_exit();
			}
			break;
		case 'h':
			/* Fall through */
		default:
			print_help_and_exit();
			break;
		}
	}

	printf("Processor Settings\n");
	printf("R: %" PRIu64 "\n", r);
	printf("k0: %" PRIu64 "\n", k0);
	printf("k1: %" PRIu64 "\n", k1);
	printf("k2: %" PRIu64 "\n", k2);
	printf("F: %" PRIu64 "\n", f);
	printf("\n");

	/* Setup the processor */
	setup_processor(r, k0, k1, k2, f);


	memset(instset, 0, sizeof(processor_inst_t) * BUFSIZE);
	instn = 0;
	while (read_instructs(instset + instn)) {
		processor_inst_t *p_inst = instset + instn;
		//cout << p_inst->instruction_address<< " " <<p_inst->op_code<<" " << p_inst->dest_reg<<" "<< p_inst->src_reg[0] << " "<< p_inst->src_reg[1] <<" "<<p_inst->branch_addr << " " <<p_inst->taken<<endl;
		instn++;
	}


	/* Setup statistics */
	processor_stats_t stats;
	memset(&stats, 0, sizeof(processor_stats_t));

	/* Run the processor */
	run_processor(&stats);

	/* Finalize stats */
	complete_processor(&stats);

	print_statistics(&stats);

	return 0;
}

void print_statistics(processor_stats_t * p_stats)
{
	uint64_t i, j, mq, tq;
	mq = tq = 0;
	j = disp_queue.size();
	while (!disp_queue.empty()) {
		i = disp_queue.front();
		if (i > mq)
			mq = i;
		tq += i;
		disp_queue.pop();
	}
	printf("Processor stats:\n");
	printf("Avg inst fired per cycle: %.2f\n", p_stats->avg_inst_fire);
	printf("Total branch instruction: %lu\n",
	       p_stats->branch_instruction);
	printf("Total correct predicted branch instruction: %lu\n",
	       p_stats->correct_predicted_instruction);
	printf("Prediction accuracy: %.2f\n",
	       1.0 * p_stats->correct_predicted_instruction /
	       p_stats->branch_instruction);
	printf("Avg Dispatch queue size: %.2f\n", 1.0*tq / j);
	printf("Maximum Dispatch queue size: %lu\n", mq);
	printf("Avg inst Issue per cycle: %.2f\n",
	       1.0 * p_stats->issued_instruction / p_stats->cycle_count);
	printf("Avg inst retired per cycle: %.2f\n",
	       1.0 * p_stats->retired_instruction / p_stats->cycle_count);
	printf("Total instructions: %lu\n", instn);
	printf("Total run time (cycles): %lu\n", p_stats->cycle_count);
}
