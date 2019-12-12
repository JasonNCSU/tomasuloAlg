//
// Created by Jason on 12/11/2019.
//

#include "tomsim.h"
#include <iostream>
#include <iomanip>

using namespace std;

//Default Constructor
Instr::Instr() {
    pc = 0;
    op_code = 0;
    dest_reg = 0;
    src1_reg = 0;
    src2_reg = 0;

    state = INVALID;

    tag = 0;
    ifStartCycle = 0;
    idStartCycle = 0;
    isStartCycle = 0;
    exStartCycle = 0;
    wbStartCycle = 0;

    exCycleCount = 0;

    ready_flag = false;
}
//Custom Constructor
Instr::Instr(int p, int op, int dest, int src1, int src2) {
    pc = p;
    op_code = op;
    dest_reg = dest;
    src1_reg = src1;
    src2_reg = src2;

    state = INVALID;

    tag = 0;
    ifStartCycle = 0;
    idStartCycle = 0;
    isStartCycle = 0;
    exStartCycle = 0;
    wbStartCycle = 0;

    exCycleCount = 0;

    ready_flag = false;
}
//Getters and Setters
void Instr::setTag(int instrNum) {
    tag = instrNum;
}
int Instr::getTag() {
    return tag;
}
void Instr::updateEXCurrCycle() {
    exCycleCount++;
}
int Instr::getEXCurrCycle() {
    return exCycleCount;
}
//SET and GET CYCLE WHEN STATE STARTS
void Instr::setIFCycle(int cycle) {
    ifStartCycle = cycle;
}
int Instr::getIFCycle() {
    return ifStartCycle;
}
void Instr::setIDCycle(int cycle) {
    idStartCycle = cycle;
}
int Instr::getIDCycle() {
    return idStartCycle;
}
void Instr::setISCycle(int cycle) {
    isStartCycle = cycle;
}
int Instr::getISCycle() {
    return isStartCycle;
}
void Instr::setEXCycle(int cycle) {
    idStartCycle = cycle;

    switch (op_code) {
        case 0:
            exCycleCount = 1;
            break;
        case 1:
            exCycleCount = 2;
            break;
        case 2:
            exCycleCount = 5;
            break;
        default:
            exCycleCount = 0;
            break;
    }
}
int Instr::getEXCycle() {
    return exStartCycle;
}
void Instr::setWBCycle(int cycle) {
    wbStartCycle = cycle;
}
int Instr::getWBCycle() {
    return wbStartCycle;
}
//SET CYCLE WHEN STATE STARTS
//Next State
void Instr::updateState() {
    switch (state) {
        case 0:
            state = IF;
            break;
        case 1:
            state = ID;
            break;
        case 2:
            state = IS;
            break;
        case 3:
            state = EX;
            break;
        case 4:
            state = WB;
            break;
        case 5:
            state = INVALID;
            break;
        default:
            state = INVALID;
            break;
    }
}
//Get State
pipeline_t Instr::getState() {
    return state;
}
//Print Instruction Specific Info
void Instr::print() {
    int exLength = 0;
    switch (op_code) {
        case 0:
            exLength = 1;
            break;
        case 1:
            exLength = 2;
            break;
        case 2:
            exLength = 5;
            break;
        default:
            exLength = 0;
            break;
    }

    cout << tag << " fu{" << op_code << "}";
    cout << " src{" << src1_reg << "," << src2_reg << "}";
    cout << " dst{" << dest_reg << "}";
    cout << " IF{" << ifStartCycle << "," << 1 << "}";
    cout << " ID{" << idStartCycle << "," << isStartCycle - idStartCycle << "}";
    cout << " IS{" << isStartCycle << "," << exStartCycle - isStartCycle << "}";
    cout << " EX{" << exStartCycle << "," << exLength << "}";
    cout << " WB{" << wbStartCycle << "," << 1 << "}" << endl;
}




//Default Constructor
TomSim::TomSim() {
    sched_queue_size = 0;
    n_peak_rate = 0;
    instr_num = 0;
    cycle_num = 0;

    pc = 0;
    op_code = 0;
    dest_reg = 0;
    src1_reg = 0;
    src2_reg = 0;
}
//Custom Constructor
TomSim::TomSim(unsigned int s, unsigned int n) {
    sched_queue_size = s;
    n_peak_rate = n;
    instr_num = 0;
    cycle_num = 0;

    pc = 0;
    op_code = 0;
    dest_reg = 0;
    src1_reg = 0;
    src2_reg = 0;
}
//Instruction Entry
void TomSim::instrReadIn(int p, int op, int dest, int src1, int src2) {
    pc = p;
    op_code = op;
    dest_reg = dest;
    src1_reg = src1;
    src2_reg = src2;
}
//Increment Instruction Count
void TomSim::incrInstrCount() {
    instr_num++;
}
//Increment Cycle Count
void TomSim::incrCycleCount() {
    cycle_num++;
}
//Get the Current Cycle
int TomSim::getCycleCount() {
    return cycle_num;
}
//Print General Info
void TomSim::print() {
    double ipc = (double) instr_num / (double) cycle_num;

    cout << "CONFIGURATION" << endl;
    cout << " superscalar bandwidth (N) = " << n_peak_rate << endl;
    cout << " dispatch queue size (2*N) = " << 2*n_peak_rate << endl;
    cout << " schedule queue size (S)   = " << sched_queue_size << endl;
    cout << "RESULTS" << endl;
    cout << " number of instructions = " << instr_num << endl;
    cout << " number of cycles       = " << cycle_num << endl;
    cout << setprecision(2) << fixed << " IPC                    = " << ipc << endl;
}
