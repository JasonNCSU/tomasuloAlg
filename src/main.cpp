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

using namespace std;

int main(int argc, char **argv) {

    //declare values for program start
    unsigned int sched_queue_size = 0;
    unsigned int n_peak_rate = 0;
    //int blocksize = 0;
    //int l1_size = 0;
    //int l1_assoc = 0;
    //int l2_size = 0;
    //int l2_assoc = 0;
    char *trace_file = nullptr;

    //initialize values from program start
    sched_queue_size = strtol(argv[1], nullptr, 10);
    n_peak_rate = strtol(argv[2], nullptr, 10);
    //blocksize = strtol(argv[3], nullptr, 10);
    //l1_size = strtol(argv[4], nullptr, 10);
    //l1_assoc = strtol(argv[5], nullptr, 10);
    //l2_size = strtol(argv[6], nullptr, 10);
    //l2_assoc = strtol(argv[7], nullptr, 10);
    trace_file = argv[8];

    //create object with queue size and peak rate
    TomSim tomSim(sched_queue_size, n_peak_rate);
    vector<Instr> instrVector;

    //dispatch queue is fetch + dispatch, thus 2N
    unsigned int dispatch_size = 2*n_peak_rate;

    //read in file
    int pc = 0;
    int op_code = 0;
    int dest_reg = 0;
    int src1_reg = 0;
    int src2_reg = 0;
    //int mem_addr = 0;

    //flag for going back 1 after erasing value in vector
    bool erased = false;

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
                    //mem_addr = strtoul(temp.c_str(), nullptr, 16);
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

    //schedule and dispatch and fakeROB queues
    RegFile regFile [128];
    vector<Instr> fakeROB; //holds all of the instr, will be checked during ex and fakeretire
    vector<Instr> executeQueue; //holds all instr during EX phase, determines when it's ready for WB
    vector<Instr> scheduleQueue; //holds the instr during IS phase, determines when it's clear to go to EX
    vector<Instr> dispatchQueue; //holds the instr during IF and ID phase, determines when it's clear to go to IS
    //check matching tag for inside the ROB
    int tempTag = 0;
    //only N from dispatch -> sched per cycle MAX
    unsigned int moved = 0;
    //# of instructions
    unsigned int instrVectorLength = instrVector.size();
    //loop variables
    unsigned long k = 0;
    unsigned long i = 0;
    unsigned int j = 0;
    while (j < instrVectorLength || !fakeROB.empty()) {
        //FakeRetire============================
        if (!fakeROB.empty()) {
            //copy over final details from fakeROB back to the original array
            //this is for the final print statement that occurs after this while loop
            for (i = 0; i < fakeROB.size(); i++) {
                if (erased) {
                    erased = false;
                    i--;
                }
                if (fakeROB.at(i).getState() == 5) {
                    tempTag = fakeROB.at(i).getTag();
                    instrVector.at(tempTag).setIFCycle(fakeROB.at(i).getIFCycle());
                    instrVector.at(tempTag).setIDCycle(fakeROB.at(i).getIDCycle());
                    instrVector.at(tempTag).setISCycle(fakeROB.at(i).getISCycle());
                    instrVector.at(tempTag).setEXCycle(fakeROB.at(i).getEXCycle());
                    instrVector.at(tempTag).setWBCycle(fakeROB.at(i).getWBCycle());
                    fakeROB.erase(fakeROB.begin() + i);
                    erased = true;
                }
            }
        }

        //Execute===============================
        if (!executeQueue.empty()) {
            for (i = 0; i < executeQueue.size(); i++) {
                if (erased) {
                    erased = false;
                    i--;
                }
                //update current execution cycle number by doing cycle - 1
                //if we are at cycle 1, we need to change the state to WB
                //we also need to update the register file's ready flag
                //also remove the instruction from executeQueue vector as it's done
                //need to update info in fakeROB as well
                //break after we find the one instr that matches tags, no need to check the rest
                if (executeQueue.at(i).getEXCurrCycle() == 1) {
                    //update register file ready flag
                    regFile[executeQueue.at(i).getDestReg()].setFlag(true);

                    tempTag = executeQueue.at(i).getTag();
                    for (k = 0; k < fakeROB.size(); k++) {
                        if (fakeROB.at(k).getTag() == tempTag) {
                            fakeROB.at(k).updateEXCurrCycle();
                            fakeROB.at(k).updateState();
                            fakeROB.at(k).setWBCycle(tomSim.getCycleCount());
                            break;
                        }
                    }

                    executeQueue.erase(executeQueue.begin() + i);
                    erased = true;
                } else {
                    executeQueue.at(i).updateEXCurrCycle();

                    tempTag = executeQueue.at(i).getTag();
                    for (k = 0; k < fakeROB.size(); k++) {
                        if (fakeROB.at(k).getTag() == tempTag) {
                            fakeROB.at(k).updateEXCurrCycle();
                            break;
                        }
                    }
                }
            }
        }

        //Issue=================================
        //TODO Move things from scheduleQueue to executeQueue when the register-
        //TODO file says that the source registers are ready to go.
        //TODO We also need to updateState on the ROB
        //

        //Dispatch==============================
        //First check that the dispatch queue is not empty and that the schedule queue is not full
        if (!dispatchQueue.empty() && scheduleQueue.size() != sched_queue_size) {
            moved = 0;
            for (i = 0; i < dispatchQueue.size(); i++) {
                if (erased) {
                    erased = false;
                    i--;
                }
                //once the queue is full or we have moved max allowed instr from dispatch to
                //schedule queue, then we end the loop early
                if (scheduleQueue.size() == sched_queue_size || moved >= n_peak_rate) {
                    break;
                }
                //change state to IS, set the cycle that the IS happened on, add to schedule queue
                //take it away from dispatch queue
                //need to update info in fakeROB as well
                //break after we find the one instr that matches tags, no need to check the rest
                if (dispatchQueue.at(i).getState() == 2) {
                    dispatchQueue.at(i).updateState();
                    dispatchQueue.at(i).setISCycle(tomSim.getCycleCount());

                    scheduleQueue.push_back(dispatchQueue.at(i));

                    tempTag = dispatchQueue.at(i).getTag();
                    for (k = 0; k < fakeROB.size(); k++) {
                        if (fakeROB.at(k).getTag() == tempTag) {
                            fakeROB.at(k).updateState();
                            fakeROB.at(k).setISCycle(tomSim.getCycleCount());
                            break;
                        }
                    }

                    dispatchQueue.erase(dispatchQueue.begin() + i);
                    erased = true;
                    moved++;
                }

            }
        }

        //Fetch=================================
        //if the dispatch queue is empty, no need to update any IF to ID
        //need to update the fakeROB as well
        //break after we find the one instr that matches tags, no need to check the rest
        if (!dispatchQueue.empty()) {
            for (i = 0; i < dispatchQueue.size(); i++) {
                if (dispatchQueue.at(i).getState() == 1) {
                    dispatchQueue.at(i).updateState();
                    dispatchQueue.at(i).setIDCycle(tomSim.getCycleCount());
                    tempTag = dispatchQueue.at(i).getTag();
                    for (k = 0; k < fakeROB.size(); k++) {
                        if (fakeROB.at(k).getTag() == tempTag) {
                            fakeROB.at(k).updateState();
                            fakeROB.at(k).setIDCycle(tomSim.getCycleCount());
                            break;
                        }
                    }
                }
            }
        }
        //we can add up to N instr to the dispatch queue in the IF state.
        //if we reach max size, break without changing the next instruction we are going to read
        //we want to continue from this point next time we get back here
        //we set the tag, set state to IF, set the IF cycle, as well as add the
        //instr to dispatchQueue and the fakeROB
        for (i = 0; i < n_peak_rate; i++) {
            if ((dispatchQueue.size() == dispatch_size) || (j >= instrVectorLength)) {
                break;
            }

            instrVector.at(j).setTag(j);
            instrVector.at(j).updateState();
            instrVector.at(j).setIFCycle(tomSim.getCycleCount());
            dispatchQueue.push_back(instrVector.at(j));
            fakeROB.push_back(instrVector.at(j));
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