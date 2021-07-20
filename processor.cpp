#include "processor.hpp"
#include <string.h>
#include <queue>		// std::queue
#include <unordered_map>
#include <utility>
#include <unistd.h>
#include <list>
#include <bits/stdc++.h>

/**
 * initializing the processor. 
 *
 * @k0 Number of k0 FUs
 * @k1 Number of k1 FUs
 * @k2 Number of k2 FUs
 * @f Number of instructions to fetch
 * @r reservation units
 */

ALU *aALU;
MLU *aMLU;
LSU *aLSU;

CDB cdb[64];

processor_inst_t instset[BUFSIZE];
queue < int >s0, s1, s2, s3, s4;
queue < int >t0, t1, t2, t3, t4;
queue < int >disp_queue, bq;
uint64_t ak0 = 3, ak1 = 1, ak2 = 2;
uint64_t out = 0;
uint64_t ar = 1, af;
uint64_t clocknum = 0, instn, s0pe, instp;
pair < int, int >pretable;
unordered_map < int, pair < int, int >>phash;
int GHR = 0;

//Display instruction.
void show_inst(processor_inst_t * in)
{
	cout << in->op_code << " " << in->
	    dest_reg << in->src_reg[0] << in->src_reg[1] << endl;
}

//Read instruction information from array.
void get_inst(int w, processor_inst_t * in)
{
	memcpy(in, instset + w, sizeof(processor_inst_t));
}

//Allocate dynamic memory for functional units.
void setup_processor(uint64_t r, uint64_t k0, uint64_t k1, uint64_t k2,
		     uint64_t f)
{
	ar = r;
	ak0 = k0 * r;
	ak1 = k1 * r;
	ak2 = k2 * r;
	af = f;
	if ((aALU = (ALU *) malloc(sizeof(ALU) * ak0)) == NULL) {
		cout << "no enough memory and exit" << endl;
		exit(1);
	}
	memset(aALU, 0xff, sizeof(ALU) * ak0);

	if ((aMLU = (MLU *) malloc(sizeof(MLU) * ak1)) == NULL) {
		cout << "no enough memory and exit" << endl;
		exit(1);
	}
	memset(aMLU, 0xff, sizeof(MLU) * ak1);

	if ((aLSU = (LSU *) malloc(sizeof(LSU) * ak2)) == NULL) {
		cout << "no enough memory and exit" << endl;
		exit(1);
	}
	memset(aLSU, 0xff, sizeof(LSU) * ak2);

	memset(cdb, 0xff, sizeof(cdb));
}

//instruction with branch address to branch prediction table 
//pretab = prediction table
//Use a 128 entry table of 2-bit bimodal GSelect predictor where the initial state is 01 for each counter. 
//The GHR (Global history register) is set to be 3 bits wide.
//The instruction address is hashed to get an index into the Branch Target Buffer using this function: 
//((Address >> 2) % 16)
int add_pretab(int branch)
{
	int old = clocknum;
	int addr, haddr, taken;
	addr = instset[branch].instruction_address;
	haddr = ((addr >> 2) % 16) << 3 | GHR;
	unordered_map < int, pair < int, int >>::const_iterator got =
	    phash.find(haddr);
	if (got == phash.end()) {
		pair < int, int >p = { 1, clocknum };
		phash[haddr] = p;
		taken = 1;
	} else
		taken = got->second.first;
	if (phash.size() > HASHSIZE) {
	      for (pair < int, pair < int, int >>element:phash)
		{
			if (element.second.second < old) {
				old = element.second.second;
				addr = element.first;
			}
		}
		if (old < clocknum) {
			phash.erase(addr);
		}
	}
	return taken;
}

//Update prediction table
//Same logic as "add_pretab(int branch)"
//pretab = prediction table
int update_pretab(int branch, int act)
{
	int old = clocknum;
	int addr, haddr, taken;
	addr = instset[branch].instruction_address;
	haddr = ((addr >> 2) % 16) << 3 | GHR;
	unordered_map < int, pair < int, int >>::const_iterator got =
	    phash.find(haddr);
	if (got == phash.end()) {
		pair < int, int >p = { 1, clocknum };
		phash[haddr] = p;
		taken = 1;
	} else {
		taken = got->second.first;
		if (taken == 0 && act == 0)
			taken = 0;
		else if (taken == 0 && act == 1)
			taken = 1;
		else if (taken == 1 && act == 0)
			taken = 0;
		else if (taken == 1 && act == 1)
			taken = 3;
		else if (taken == 2 && act == 1)
			taken = 3;
		else if (taken == 2 && act == 0)
			taken = 0;
		else if (taken == 3 && act == 1)
			taken = 3;
		else if (taken == 3 && act == 0)
			taken = 2;
		else
			taken = 1;
		phash.erase(haddr);
		pair < int, int >p = { taken, clocknum };
		phash[haddr] = p;
	}
	if (phash.size() > HASHSIZE) {
	      for (pair < int, pair < int, int >>element:phash)
		{
			if (element.second.second < old) {
				old = element.second.second;
				addr = element.first;
			}
		}
		if (old < clocknum) {
			phash.erase(addr);
		}
	}
	return taken;
}

//Stage 0: Fetch instructions, and put them to queue of stage 1.
//Update finish information to statistics and instruction array.
void fetch_inst_s0(processor_stats_t * p_stats)
{
	int taken;
	if (!bq.empty()) {
		s0pe = bq.front();
		while (!bq.empty())
			bq.pop();
		if (s0pe < instn)
			s0pe++;
	}
	if (s0pe < instn) {
		//if (instset[s0pe].branch_addr > 0) {
		//	p_stats->branch_instruction++;
		//}
		s0.push(s0pe);
		t0.push(clocknum);
		instset[s0pe].stage = 0;
		instset[s0pe].fetch = clocknum;
		if (instset[s0pe].branch_addr > 0) {
			instset[s0pe].pretaken = taken = add_pretab(s0pe);
		}
	}
}

//Stage 1: Dispatch: dispatch instructions as many as possible.
//Update finish information to statistics and instruction array.
void dispatch_inst_s1(processor_stats_t * p_stats)
{
	int i, j, found;
	processor_inst_t inst;
	queue < int >s1t, t1t;
	if (!bq.empty())
		return;
	while (!s0.empty()) {
		s1.push(s0.front());
		t1.push(t0.front());
		s0.pop();
		t0.pop();
	}
	disp_queue.push(s1.size());
	while (!s1.empty()) {
		if (t1.front() + 2 > clocknum) {
			s1t.push(s1.front());
			t1t.push(t1.front());
			s1.pop();
			t1.pop();
			continue;
		}
		i = s1.front();
		get_inst(i, &inst);
		found = 0;
		switch (inst.op_code) {
		case 0:
			for (j = 0; j < ak0; j++) {
				if (aALU[j].available) {
					aALU[j].instn = i;
					aALU[j].available = 0;
					instset[i].stage = 1;
					instset[i].position = j;
					instset[i].dispatch = clocknum;
					found = 1;
					break;
				}
			}
			break;
		case 1:
			for (j = 0; j < ak1; j++) {
				if (aMLU[j].available) {
					aMLU[j].instn = i;
					aMLU[j].available = 0;
					instset[i].stage = 1;
					instset[i].position = j;
					instset[i].dispatch = clocknum;
					found = 1;
					break;
				}
			}
			break;
		case 2:
			for (j = 0; j < ak2; j++) {
				if (aLSU[j].available) {
					aLSU[j].instn = i;
					aLSU[j].available = 0;
					instset[i].stage = 1;
					instset[i].position = j;
					instset[i].dispatch = clocknum;
					found = 1;
					break;
				}
			}
			break;
		default:
			break;
		}
		if (found) {
			p_stats->issued_instruction++;
			s2.push(s1.front());
			t2.push(clocknum);
			s1.pop();
			t1.pop();
		} else {
			s1t.push(s1.front());
			t1t.push(t1.front());
			s1.pop();
			t1.pop();
		}
	}
	while (!s1t.empty()) {
		s1.push(s1t.front());
		t1.push(t1t.front());
		s1t.pop();
		t1t.pop();
	}
}

//Find the time when FUs finish execution to update status for schedule.
int get_endclock(int w)
{
	int position;
	position = instset[w].position;
	switch (instset[w].op_code) {
	case 0:
		return aALU[position].end;
	case 1:
		return aMLU[position].end;
	case 2:
		return aLSU[position].end;
	default:
		return -1;
	}
}

//Logic of Tomasulo's Algorithm: See whether there is data hazard. If no, let the instruction go to execution stage.
//Assume the bus has infinite capcaity. FUs are ready for execution if there is no data dependency.
int cdb_logic(int r1, int r2, int r3, int w, int position)
{
	list < int >t;
	int or1, or2, or3, e;
	int i, found = 0;
	t.clear();
	for (i = 0; i < ak0; i++) {
		if (aALU[i].available == 0 && aALU[i].instn < w) {
			t.push_back(aALU[i].instn);
		}
	}
	for (i = 0; i < ak1; i++) {
		if (aMLU[i].available == 0 && aMLU[i].instn < w) {
			t.push_back(aMLU[i].instn);
		}
	}
	for (i = 0; i < ak2; i++) {
		if (aLSU[i].available == 0 && aLSU[i].instn < w) {
			t.push_back(aLSU[i].instn);
		}
	}
	t.sort();
	//cout<<"check "<<w ;
	while (!t.empty()) {
		i = t.back();
		or1 = instset[i].dest_reg;
		or2 = instset[i].src_reg[0];
		or3 = instset[i].src_reg[1];
		e = get_endclock(i);
		if (r2 > 0 && (r2 == or1) && (e > clocknum || e == -1)) {
			return 0;
		}
		if (r3 > 0 && (r3 == or1) && (e > clocknum || e == -1)) {
			return 0;
		}
		t.pop_back();
	}
	return 1;

}

list < int >s2q, t2q;

bool cmp(pair < int, int >&a, pair < int, int >&b)
{
	return a.first < b.first;
}

void sort(map < int, int >&M)
{
	vector < pair < int, int >>A;
      for (auto & it:M) {
		A.push_back(it);
	}
	sort(A.begin(), A.end(), cmp);
      for (auto & it:A) {
		s2q.push_back(it.first);
		t2q.push_back(it.second);
	}
}

//Sub-function of schedule_inst_s2 (Stage 2: Schedule)
//Arrange to check data dependence. 
void rearrange_inst()
{
	int i, j;
	map < int, int >tmp;
	while (!s2q.empty()) {
		i = s2q.front();
		j = t2q.front();
		tmp.emplace(i, j);
		s2q.pop_front();
		t2q.pop_front();
	}
	sort(tmp);
}


//Stage 2: Schedule:
//service them in program order, and based on the availability of function units
//if there are multiple independent instructions ready to fire during the same 
//cycle then put them in operation.
//Update finish information to statistics and instruction array.
void schedule_inst_s2(processor_stats_t * p_stats)
{
	int i, r1, r2, r3, position;
	int ready, w;
	processor_inst_t inst;
	queue < int >s2t, t2t;
	//cout <<"s2: "<< s2.size() <<" : " ; 
	while (!s2.empty()) {
		if (t2.front() + 2 > clocknum) {
			s2t.push(s2.front());
			t2t.push(t2.front());
			s2.pop();
			t2.pop();
			continue;
		}
		s2q.push_back(s2.front());
		t2q.push_back(t2.front());
		s2.pop();
		t2.pop();
	}
	rearrange_inst();
	while (!s2q.empty()) {

		w = s2q.front();
		get_inst(w, &inst);
		r1 = inst.dest_reg;
		r2 = inst.src_reg[0];
		r3 = inst.src_reg[1];
		position = inst.position;
		//cout <<w <<" ";
		ready = cdb_logic(r1, r2, r3, w, position);
		switch (inst.op_code) {
		case 0:
			if (ready) {
				aALU[position].status = 1;
			}
			break;
		case 1:
			if (ready) {
				aMLU[position].status = 1;
			}
			break;
		case 2:
			if (ready) {
				aLSU[position].status = 1;
			}
			break;
		default:
			break;
		}
		if (ready) {
			s3.push(w);
			t3.push(clocknum);
			s2q.pop_front();
			t2q.pop_front();
			instset[w].stage = 2;
			instset[w].schedule = clocknum;
		} else {
			s2t.push(s2q.front());
			t2t.push(t2q.front());
			s2q.pop_front();
			t2q.pop_front();
		}
	}
	while (!s2t.empty()) {
		s2.push(s2t.front());
		t2.push(t2t.front());
		s2t.pop();
		t2t.pop();
	}
	//cout<<endl;
}

//CDB broadcast: Update CDB and register file and broadcast information.
void cdb_bcast(int w)
{
	int r1 = instset[w].dest_reg;
	int r2 = instset[w].src_reg[0];
	int r3 = instset[w].src_reg[1];
	int position = instset[w].position;
	int m = w;
	switch (instset[w].op_code) {
	case 0:
		memset(&aALU[position], 0xff, sizeof(ALU));
		break;
	case 1:
		memset(&aMLU[position], 0xff, sizeof(MLU));
		break;
	case 2:
		memset(&aLSU[position], 0xff, sizeof(LSU));
		break;
	default:
		break;
	}
	if (r1 > -1 && cdb[r1].instn == w)
		memset(&cdb[r1], 0xff, sizeof(CDB));
}

void rerange_q(queue < int >*s, queue < int >*t, int clearres);
int clear_inst();
//Check if the prediction is correct
int pre_correct(int w)
{
	if ((instset[w].taken == 1 && instset[w].pretaken == 1)
	    || (instset[w].taken == 1 && instset[w].pretaken == 0)
	    || (instset[w].taken == 0 && instset[w].pretaken == 3)
	    || (instset[w].taken == 0 && instset[w].pretaken == 2)) {
		return 0;
	}
	return 1;
}

//Stage 3: Execution
//updates the reservation station and put instruction to queue of update stage if instruction completes execution.
//Update finish information to statistics and instruction array.
void exec_inst_s3(processor_stats_t * p_stats)
{
	int i, w, ready, position;
	processor_inst_t inst;
	queue < int >s3t, t3t;
	//cout <<"s3:";
	while (!s3.empty()) {
		if (t3.front() + 2 > clocknum) {
			s3t.push(s3.front());
			t3t.push(t3.front());
			s3.pop();
			t3.pop();
			continue;
		}
		w = s3.front();
		get_inst(w, &inst);
		ready = 0;
		p_stats->fired_instruction++;
		position = inst.position;
		//cout << w <<" ";
		switch (inst.op_code) {
		case 0:
			if (aALU[position].status == 2) {
				if (aALU[position].end <= clocknum)
					ready = 1;
				if (instset[w].branch_addr > 0)
					ready = 2;
			}
			if (aALU[position].status == 1) {
				aALU[position].status = 2;
				aALU[position].begin = clocknum;
				aALU[position].end = clocknum + 2;
				instset[w].stage = 3;
				instset[w].execute = clocknum;
				//ready=1;
			}
			break;
		case 1:
			if (aMLU[position].status == 2
			    || instset[w].stage == 3) {
				if (aMLU[position].end <= clocknum)
					ready = 1;
				if (instset[w].branch_addr > 0)
					ready = 2;
			}
			if (aMLU[position].status == 1) {
				aMLU[position].status = 2;
				aMLU[position].begin = clocknum;
				aMLU[position].end = clocknum + 6;
				instset[w].stage = 3;
				instset[w].execute = clocknum;
				//ready=1;
			}
			break;
		case 2:
			if (aLSU[position].status == 2
			    || instset[w].stage == 3) {
				if (aLSU[position].end <= clocknum)
					ready = 1;
				if (instset[w].branch_addr > 0)
					ready = 2;
			}
			if (aLSU[position].status == 1) {
				aLSU[position].status = 2;
				aLSU[position].begin = clocknum;
				aLSU[position].end = clocknum + 20;
				instset[w].stage = 3;
				instset[w].execute = clocknum;
				//ready=1;
			}
			break;
		default:
			break;
		}
		switch (ready) {
		case 2:
			GHR = (GHR << 1 | (instset[w].taken & 0x1)) & 0x7;
			if (!pre_correct(w)) {
				bq.push(w);
				s4.push(w);
				t4.push(clocknum);
				s3.pop();
				t3.pop();
			} else {
				s4.push(w);
				t4.push(clocknum);
				s3.pop();
				t3.pop();
			}
			break;
		case 1:
			s4.push(w);
			t4.push(clocknum);
			s3.pop();
			t3.pop();
			break;
		default:
			s3t.push(s3.front());
			t3t.push(t3.front());
			s3.pop();
			t3.pop();
		}
	}
	while (!s3t.empty()) {
		s3.push(s3t.front());
		t3.push(t3t.front());
		s3t.pop();
		t3t.pop();
	}

}

//Stage 4: Status Update
//broadcast execution result to CDB.
//Update finish information to statistics and instruction array.
void update_inst_s4(processor_stats_t * p_stats)
{
	int i, w, status, position;
	processor_inst_t inst;
	queue < int >s4t, t4t;
	list < int >t;
	while (!s4.empty()) {
		if (t4.front() > clocknum) {
			s4t.push(s4.front());
			t4t.push(t4.front());
			s4.pop();
			t4.pop();
			continue;
		}
		t.push_back(s4.front());
		s4.pop();
		t4.pop();
	}
	t.sort();
	//cout <<"s4:";
	while (!t.empty()) {
		w = t.front();
		get_inst(w, &inst);
		status = 0;
		instset[w].stage = 4;
		position = inst.position;
		p_stats->retired_instruction++;
		//cout << w<< " ";  
		if (w == 1125 || w == 1120) {
			int m = 1;
		}
		cdb_bcast(w);
		instset[w].stage = 5;
		instset[w].update = clocknum;
		t.pop_front();
		if (instset[w].branch_addr > 0) {
			if (!pre_correct(w)) {
				instset[w].state=-1;
				auto it = t.begin();
				while (it != t.end()) {
					if (*it > w) {
						cdb_bcast(*it);
						instset[*it].stage =
						    instset[*it].state =
						    instset[*it].position =
						    instset[*it].pretaken =
						    -1;
						instset[*it].fetch =
						    instset[*it].dispatch =
						    instset[*it].schedule =
						    instset[*it].execute =
						    instset[*it].update =
						    -1;

						it = t.erase(it);
					} else
						it++;
				}
				rerange_q(&s1, &t1, 0);
				rerange_q(&s2, &t2, 1);
				rerange_q(&s3, &t3, 1);
				//rerange_q(&s4, &t4, 1);
				rerange_q(&s4t, &t4t, 1);
				clear_inst();
				update_pretab(w, instset[w].taken);
				//bq.pop();
			}
			else {
				instset[w].state=0;
			}
		}
	}
	while (!s4t.empty()) {
		s4.push(s4t.front());
		t4.push(t4t.front());
		s4t.pop();
		t4t.pop();
	}
}

//Update fetch pointer.
void update_point()
{
	instp++;		// s0
	if (!bq.size())
		s0pe++;		// s1

}

//Do fetch and dispatch in first half of clock cycle.
void firsthalf(processor_stats_t * p_stats)
{
	fetch_inst_s0(p_stats);
	dispatch_inst_s1(p_stats);
	update_point();
}

//If there is prediction wrong clear instruction queue
int clear_inst()
{
	int i, w, k = 0;
	w = bq.front();
	for (i = w + 1; i < instn; i++) {
		if (instset[i].stage != -1) {
			instset[i].pretaken = instset[i].fetch =
			    instset[i].dispatch = instset[i].schedule =
			    instset[i].execute = instset[i].update = 0;
			instset[i].stage = instset[i].position =
			    instset[i].state = -1;
			k++;
		}
	}
	return 1;
}

//If there is prediction wrong clear message queue
void rerange_q(queue < int >*s, queue < int >*t, int clearres)
{
	queue < int >st, tt;
	int i, j, n;
	n = bq.front();
	while (!(s->empty())) {
		if ((i = s->front()) < n) {
			st.push(s->front());
			tt.push(t->front());
			s->pop();
			t->pop();
			continue;
		}
		if (clearres) {
			cdb_bcast(i);
			instset[i].stage = instset[i].state =
			    instset[i].position = instset[i].pretaken = -1;
			instset[i].fetch = instset[i].dispatch =
			    instset[i].schedule = instset[i].execute =
			    instset[i].update = -1;
		}
		s->pop();
		t->pop();
	}
	while (!st.empty()) {
		s->push(st.front());
		t->push(tt.front());
		st.pop();
		tt.pop();
	}
}

//Do schedule, execution and update in second half of clock cycle.
void secondhalf(processor_stats_t * p_stats)
{
	schedule_inst_s2(p_stats);
	exec_inst_s3(p_stats);
	update_inst_s4(p_stats);
}

//Increment clock cycle count as instructions are processed one by one.
void clock(processor_stats_t * p_stats)
{
	firsthalf(p_stats);
	clocknum++;
	secondhalf(p_stats);
	clocknum++;
}

//Check if all instructions finish processing.
int do_finish()
{
	int i;
	for (i = 0; i < instn; i++) {
		if (instset[i].stage < 5) {
			break;
		}
	}
	if (i < instn) {
		return 0;
	}
	return 1;
}

/**
 * Simulates the processor.
 * fetch instructions as appropriate, until all instructions have executed
 *
 * @p_stats Pointer to the statistics structure
 */
void run_processor(processor_stats_t * p_stats)
{
	while (!do_finish()) {
		clock(p_stats);
	}
}

/**
 * Subroutine for cleaning up any outstanding instructions and calculating overall statistics
 * such as average IPC or branch prediction percentage
 *
 * @p_stats Pointer to the statistics structure
typedef struct _processor_stats_t
{
    float avg_inst_fire;
    unsigned long retired_instruction;
    unsigned long cycle_count;
    float avg_ipc;
    float perc_branch_pred;
} processor_stats_t;

 */
void complete_processor(processor_stats_t * p_stats)
{
	int i;

	cout << "Result:" << endl;
	for (i = 0; i < instn; i++) {
		instset[i].fetch = instset[i].fetch >> 1;
		instset[i].dispatch = instset[i].dispatch >> 1;
		instset[i].schedule = instset[i].schedule >> 1;
		instset[i].execute = instset[i].execute >> 1;
		instset[i].update = instset[i].update >> 1;

		if(instset[i].branch_addr>0) {
		     p_stats->branch_instruction++;
		}
		if(instset[i].state>=0) {
		     p_stats->correct_predicted_instruction++;
		}
		cout << i << " " << instset[i].fetch << " " <<
		    instset[i].dispatch << " " << instset[i].schedule <<
		    " " << instset[i].execute << " " << instset[i].
		    update << endl;
	}
	p_stats->cycle_count = clocknum >> 1;
	p_stats->avg_inst_fire =
	    1.0 * p_stats->fired_instruction / p_stats->cycle_count;
}
