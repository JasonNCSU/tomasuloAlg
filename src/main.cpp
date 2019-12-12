//
// Created by Iason Katsaros
// Student ID: 200089673
//

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "tomsim.h"

#define MAX_INSTR 100000


using namespace std;

int main(int argc, char **argv) {

    //declare values for program start
    int sched_queue_size = 0;
    int n_peak_rate = 0;
    int blocksize = 0;
    int l1_size = 0;
    int l1_assoc = 0;
    int l2_size = 0;
    int l2_assoc = 0;
    char *trace_file = nullptr;

    //initialize values from program start
    sched_queue_size = strtol(argv[1], nullptr, 10);
    n_peak_rate = strtol(argv[2], nullptr, 10);
    blocksize = strtol(argv[3], nullptr, 10);
    l1_size = strtol(argv[4], nullptr, 10);
    l1_assoc = strtol(argv[5], nullptr, 10);
    l2_size = strtol(argv[6], nullptr, 10);
    l2_assoc = strtol(argv[7], nullptr, 10);
    trace_file = argv[8];

    //create object with queue size and peak rate
    TomSim tomSim(sched_queue_size, n_peak_rate);
    vector<Instr> instrVector;

    //dispatch queue is fetch + dispatch, thus 2N
    int dispatch_size = 2*n_peak_rate;

    //read in file
    int pc = 0;
    int op_code = 0;
    int dest_reg = 0;
    int src1_reg = 0;
    int src2_reg = 0;
    int mem_addr = 0;

    //variables for reading in file
    string data_segment;
    string temp;
    int data_count = 0;
    ifstream input(trace_file);
    //read in all of the instructions into the instruction vector before doing anything
    while (getline(input, data_segment)) {

        //separates the line input by the space so we get all required inputs
        istringstream ss(data_segment);
        do {
            ss >> temp;

            switch (data_count) {
                case 0:
                    pc = strtoul(temp.c_str(), nullptr, 16);
                    data_count++;
                    break;
                case 1:
                    op_code = strtol(temp.c_str(), nullptr, 10);
                    data_count++;
                    break;
                case 2:
                    dest_reg = strtol(temp.c_str(), nullptr, 10);
                    data_count++;
                    break;
                case 3:
                    src1_reg = strtol(temp.c_str(), nullptr, 10);
                    data_count++;
                    break;
                case 4:
                    src2_reg = strtol(temp.c_str(), nullptr, 10);
                    data_count++;
                    break;
                case 5:
                    mem_addr = strtoul(temp.c_str(), nullptr, 16);
                    data_count++;
                    break;
                case 6:
                    data_count = 0;
                    break;
            }
        } while (ss);

        //add instruction to the instruction vector
        instrVector.emplace_back(pc, op_code, dest_reg, src1_reg, src2_reg);
        tomSim.incrInstrCount();
    }

    //schedule and dispatch queue
    vector<Instr> scheduleQueue;
    vector<Instr> dispatchQueue;
    //only N from dispatch -> sched per cycle MAX
    int moved = 0;
    //# of instructions
    int instrVectorLength = instrVector.size();
    //loop variables
    int i = 0;
    int j = 0;
    while (j < instrVectorLength) {
        //FakeRetire============================


        //Execute===============================


        //Issue=================================


        //Dispatch==============================
        if (!dispatchQueue.empty() && scheduleQueue.size() != sched_queue_size) {
            moved = 0;
            for (i = 0; i < dispatchQueue.size(); i++) {
                if (scheduleQueue.size() == sched_queue_size || moved >= n_peak_rate) {
                    break;
                }
                if (dispatchQueue.at(i).getState() == 2) {
                    dispatchQueue.at(i).updateState();
                    dispatchQueue.at(i).setISCycle(tomSim.getCycleCount());
                    scheduleQueue.push_back(dispatchQueue.at(i));
                    dispatchQueue.erase(dispatchQueue.begin() + i);
                    i--;
                    moved++;
                }

            }
        }

        //Fetch=================================
        if (!dispatchQueue.empty()) {
            for (i = 0; i < dispatchQueue.size(); i++) {
                if (dispatchQueue.at(i).getState() == 1) {
                    dispatchQueue.at(i).updateState();
                    dispatchQueue.at(i).setIDCycle(tomSim.getCycleCount());
                }
            }
        }
        for (i = 0; i < n_peak_rate; i++) {
            if ((dispatchQueue.size() == dispatch_size) || (j >= instrVectorLength)) {
                break;
            }

            instrVector.at(j).setTag(j);
            instrVector.at(j).updateState();
            instrVector.at(j).setIFCycle(tomSim.getCycleCount());
            dispatchQueue.push_back(instrVector.at(j));
            j++;
        }
        tomSim.incrCycleCount();
    }

    //call the print function
    for (i = 0; i < instrVectorLength; i++) {
        instrVector.at(i).print();
    }
    tomSim.print();

    return 0;
}