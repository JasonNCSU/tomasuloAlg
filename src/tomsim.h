//
// Created by Jason on 12/11/2019.
//

#ifndef PROJECT_TOMSIM_H
#define PROJECT_TOMSIM_H

//pipeline stages
typedef enum {INVALID, IF, ID, IS, EX, WB} pipeline_t;

class Instr {
private:
    //methods============================


    //variables============================
    int pc;
    int op_code;
    int dest_reg;
    int src1_reg;
    int src2_reg;

    pipeline_t state;

    int tag;
    int ifStartCycle;
    int idStartCycle;
    int isStartCycle;
    int exStartCycle;
    int wbStartCycle;

public:
    //methods============================
    Instr(void);
    Instr(int p, int op, int dest, int src1, int src2);

    void setTag(int instrNum);
    void setIFCycle(int cycle);
    void setIDCycle(int cycle);
    void setISCycle(int cycle);
    void setEXCycle(int cycle);
    void setWBCycle(int cycle);

    void updateState(void);
    pipeline_t getState(void);

    void print(void);

    //variables============================
};
class TomSim {
private:
    //methods============================


    //variables============================
    int sched_queue_size;
    int n_peak_rate;
    int instr_num;
    int cycle_num;

    int pc;
    int op_code;
    int dest_reg;
    int src1_reg;
    int src2_reg;

public:
    //methods============================
    TomSim(void);
    TomSim(int sched_queue_size, int n_peak_rate);

    void instrReadIn(int p, int op, int dest, int src1, int src2);
    void incrInstrCount(void);
    void incrCycleCount(void);
    int getCycleCount(void);
    void print(void);
    //variables============================

};

#endif //PROJECT_TOMSIM_H
