//
// Created by Jason on 12/11/2019.
//

#ifndef PROJECT_TOMSIM_H
#define PROJECT_TOMSIM_H

//pipeline stages
typedef enum {INVALID, IF, ID, IS, EX, WB} pipeline_t;

class RegFile {
private:
    //methods============================


    //variables============================
    bool ready_flag;
    int reg_name;

public:
    //methods============================
    RegFile(void);
    void setFlag(bool f);

    //variables============================
};
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

    unsigned int tag;
    int ifStartCycle;
    int idStartCycle;
    int isStartCycle;
    int exStartCycle;
    int wbStartCycle;

    int exCycleCount;

public:
    //methods============================
    Instr(void);
    Instr(int p, int op, int dest, int src1, int src2);

    void setTag(unsigned int instrNum);
    int getTag(void);
    int getDestReg(void);
    int getSrc1Reg(void);
    int getSrc2Reg(void);

    void setIFCycle(int cycle);
    int getIFCycle(void);
    void setIDCycle(int cycle);
    int getIDCycle(void);
    void setISCycle(int cycle);
    int getISCycle(void);
    void setEXCycle(int cycle);
    int getEXCycle(void);
    void setWBCycle(int cycle);
    int getWBCycle(void);

    int getEXCurrCycle(void);
    void updateEXCurrCycle(void);

    void updateState(void);
    pipeline_t getState(void);

    void print(void);

    //variables============================
};
class TomSim {
private:
    //methods============================


    //variables============================
    unsigned int sched_queue_size;
    unsigned int n_peak_rate;
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
    TomSim(unsigned int s, unsigned int n);

    void instrReadIn(int p, int op, int dest, int src1, int src2);
    void incrInstrCount(void);
    void incrCycleCount(void);
    int getCycleCount(void);
    void print(void);
    //variables============================

};

#endif //PROJECT_TOMSIM_H
