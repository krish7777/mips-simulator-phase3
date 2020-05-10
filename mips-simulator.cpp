#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <map>
#include <cmath>

using namespace std;

vector<int> mem(1024);
int PC = 0;
int la1 = 0, la2 = 0, la3 = 0, la4 = 0;
int bin[11] = {0};
int stall = 0, cycles = 0, inst = 0, chit1 = 0, cmiss1 = 0, chit2 = 0, cmiss2 = 0, stallc = 0;
map<string, int> reg;
map<string, int> labels;

//Phase 3 cache implementation just go to lw and sw function and make changes to include phase 3

//PHASE 3 CODE

int cz1 = 4;            // level 1 cache size
int cz2 = 16;           // level 2 cache size
int bz = 1;             // block size for both level is same
int ct1 = 2;            // clock time level 1 cache
int ct2 = 10;           // clock time level 2 cache
int mt = 100;           // clock time memory
int ca[4][3];           // level 1 cache ca[cz1][2+bz]
int ca2[16][3];         // level 2 cache ca2[cz2][2+bz]
int casv = 4;           // cache associativity level 1 cache if casv=1 for directly mapped and casv=cz1 for fully associative
int casv1 = 16;         // cache associativity level 2 cache
int cas = cz1 / casv;   // No of sets cache level 1 cas=cz1/casv
int cas1 = cz2 / casv1; // no of sets cache level 2

//binary to decimal conversion
int binary2(int n, int e)
{
    int num = n;
    int dec_value = 0;

    int base = 1;
    int last_digit;
    while (num != e + n)
    {
        last_digit = bin[num];
        num++;

        dec_value += last_digit * base;

        base = base * 2;
    }

    return dec_value;
}

// address binary conversion
void binary(int n)
{
    int i;
    for (i = 0; n > 0; i++)
    {
        bin[i] = n % 2;
        n = n / 2;
    }
}

// address = tag+index+offset

int phase3(int ad)
{

    int fo = 0, fo1 = 0, ip, i, k, j, k5, bsad, qval;
    int sa = 0, lr = 0, sa1 = 0, i9;

    for (int sv1 = 0; sv1 < 11; sv1++)
        bin[sv1] = 0;

    int nob = log2(bz); // no of offset bit
    int ib = log2(cas); // no of index bits level 1

    int ib1 = log2(cas1); // no of index bits level 2

    //binary of address
    binary(ad);

    int offval = binary2(0, nob); // offset value
    int asval = binary2(nob, ib); // associativity set value level 1

    int asval1 = binary2(nob, ib1); // associativity set value level 2

    //int tag=binary2(ib+nob,11-ib-nob); 	// tag value l1 cache
    qval = ad / bz;
    bsad = qval * bz;
    int tag = bsad; // tag value l1 cache
    //int tag1=binary2(ib1+nob,11-ib1-nob);	// tag value l2 cache

    int ss = cz1 / cas;   // set size level 1 cache
    int ss1 = cz2 / cas1; // set size level 2 cache

    int ll = asval * ss; // level 1 cache lower value of set index
    int ul = ll + ss;    // level 1 cache upper value of set index

    int ll1 = asval1 * ss1; // level 2 cache lower value of set index
    int ul1 = ll1 + ss1;    // level 2 cache lower value of set index

    //check in level 1 cache
    for (i = ll; i < ul; i++)
    {
        if (ca[i][0] == tag)
        {
            cycles = cycles + ct1 - 1;
            stall = stall + ct1 - 1;
            stallc = stallc + ct1 - 1;
            fo = 1;
            break;
        }
    }

    //found in level 1 cache
    if (fo == 1)
    {
        chit1++;
        ip = ca[i][1];
        ca[i][1] = 1;
        for (j = ll; j < ul; j++)
        {
            if (ca[j][1] < ip && j != i && ca[j][1] != -1)
            {
                ca[j][1] = ca[j][1] + 1;
            }
        }
        return ca[i][2 + offval];
    }

    //Not found in level 1 cache
    else
    {
        cmiss1++;

        //check wheter space is there in level 1 cache
        for (k = ll; k < ul; k++)
        {
            if (ca[k][0] == -1)
            {
                sa = 1;
                break;
            }
        }

        //space available level 1 cache
        if (sa == 1)
        {
            ca[k][0] = tag;

            //check in level 2 cache
            for (i = ll1; i < ul1; i++)
            {
                if (ca2[i][0] == tag)
                {
                    cycles = cycles + ct1 - 1;
                    cycles = cycles + ct2;
                    stall = stall + ct2 + ct1 - 1;
                    stallc = stallc + ct2 + ct1 - 1;
                    fo1 = 1;
                    break;
                }
            }

            //found in level 2 cache
            if (fo1 == 1)
            {
                chit2++;
                ip = ca2[i][1];
                ca2[i][1] = 1;
                for (j = ll1; j < ul1; j++)
                {
                    if (ca2[j][1] > ip && j != i && ca2[j][1] != -1)
                    {
                        ca2[j][1] = ca2[j][1] - 1;
                    }
                }
                for (int q1 = 0; q1 < bz; q1++)
                {
                    ca[k][2 + q1] = ca2[i][2 + q1];
                }

                for (int h1 = 0; h1 < 2 + bz; h1++)
                    ca2[i][h1] = -1;

                ip = ca[k][1];
                ca[k][1] = 1;
                for (int j7 = ll; j7 < ul; j7++)
                {
                    if (j7 != k && ca[j7][1] != -1)
                    {
                        ca[j7][1] = ca[j7][1] + 1;
                    }
                }

                return ca[k][2 + offval];
            }

            //not found in level 2 cache
            else
            {
                cmiss2++;
                cycles = cycles + ct1 - 1;
                cycles = cycles + ct2 + mt;

                stall = stall + ct1 - 1 + mt + ct2;
                stallc = stallc + ct1 - 1 + mt + ct2;

                qval = ad / bz;
                bsad = qval * bz; // memory block starting address
                for (int q1 = 0; q1 < bz; q1++)
                {
                    ca[k][2 + q1] = mem[bsad + q1];
                }
                ip = ca[k][1];
                ca[k][1] = 1;
                for (j = ll; j < ul; j++)
                {
                    if (j != k && ca[j][1] != -1)
                    {
                        ca[j][1] = ca[j][1] + 1;
                    }
                }
                return ca[k][2 + offval];
            }
        }

        //space not available
        else
        {

            for (j = ll; j < ul; j++)
            {
                if (ca[j][1] == ss)
                {
                    //ca[j][0]=tag;

                    //check in level 2 cache
                    for (i = ll1; i < ul1; i++)
                    {
                        if (ca2[i][0] == tag)
                        {
                            cycles = cycles + ct1 - 1;
                            cycles = cycles + ct2;
                            stall = stall + ct2 + ct1 - 1;
                            stallc = stallc + ct2 + ct1 - 1;
                            fo1 = 1;
                            break;
                        }
                    }

                    //found in level 2 cache
                    if (fo1 == 1)
                    {
                        chit2++;
                        ip = ca2[i][1];
                        ca2[i][1] = 1;
                        for (int j23 = ll1; j23 < ul1; j23++)
                        {
                            if (ca2[j23][1] > ip && j23 != i && ca2[j23][1] != -1)
                            {
                                ca2[j23][1] = ca2[j23][1] - 1;
                            }
                        }
                        ca[j][0] = tag;
                        for (int q1 = 0; q1 < bz; q1++)
                        {
                            ca[j][2 + q1] = ca2[i][2 + q1];
                        }

                        for (int h1 = 0; h1 < 2 + bz; h1++)
                            ca2[i][h1] = -1;

                        ip = ca[j][1];
                        ca[j][1] = 1;
                        for (int j1 = ll; j1 < ul; j1++)
                        {
                            if (ca[j1][1] < ip && j1 != j && ca[j1][1] != -1)
                            {
                                ca[j1][1] = ca[j1][1] + 1;
                            }
                        }

                        return ca[j][2 + offval];
                    }

                    //not found in l2 cache
                    else
                    {
                        cmiss2++;
                        cycles = cycles + ct1 - 1;
                        cycles = cycles + ct2 + mt;

                        stall = stall + ct1 - 1 + mt + ct2;
                        stallc = stallc + ct1 - 1 + mt + ct2;

                        // check space available in l2
                        for (k5 = ll1; k5 < ul1; k5++)
                        {
                            if (ca2[k5][0] == -1)
                            {
                                sa1 = 1;
                                break;
                            }
                        }

                        // space available l2 cache
                        if (sa1 == 1)
                        {
                            ca2[k5][0] = ca[j][0];
                            for (int q1 = 0; q1 < bz; q1++)
                            {
                                ca2[k5][2 + q1] = ca[j][2 + q1];
                            }

                            ca2[k5][1] = 1;
                            for (int j7 = ll1; j7 < ul1; j7++)
                            {
                                if (j7 != k5 && ca2[j7][1] != -1)
                                {
                                    ca2[j7][1] = ca2[j7][1] + 1;
                                }
                            }
                        }

                        // space not available level 2 cache
                        else
                        {

                            for (i9 = ll1; i9 < ul1; i9++)
                            {
                                if (ca2[i9][1] == ss1)
                                {
                                    ca2[i9][0] = ca[j][0];
                                    for (int q1 = 0; q1 < bz; q1++)
                                    {
                                        ca2[i9][2 + q1] = ca[j][2 + q1];
                                    }

                                    ip = ca2[i9][1];
                                    ca2[i9][1] = 1;
                                    for (int j1 = ll1; j1 < ul1; j1++)
                                    {
                                        if (ca2[j1][1] < ip && j1 != i9 && ca2[j1][1] != -1)
                                        {
                                            ca2[j1][1] = ca2[j1][1] + 1;
                                        }
                                    }
                                    break;
                                }
                            }
                        }

                        ca[j][0] = tag;
                        qval = ad / bz;
                        bsad = qval * bz; // memory block starting address
                        for (int q1 = 0; q1 < bz; q1++)
                        {
                            ca[j][2 + q1] = mem[bsad + q1];
                        }
                        ip = ca[j][1];
                        ca[j][1] = 1;
                        for (int j1 = ll; j1 < ul; j1++)
                        {
                            if (ca[j1][1] < ip && j1 != j && ca[j1][1] != -1)
                            {
                                ca[j1][1] = ca[j1][1] + 1;
                            }
                        }
                    }
                    break;
                }
            }
            return ca[j][2 + offval];
        }
    }
}

// address = tag+index+offset

void phase3w(int ad, int val)
{

    int fo = 0, fo1 = 0, ip, i, k, j, k5, bsad, qval;
    int sa = 0, lr = 0, sa1 = 0, i9;

    for (int sv1 = 0; sv1 < 11; sv1++)
        bin[sv1] = 0;

    int nob = log2(bz); // no of offset bit
    int ib = log2(cas); // no of index bits

    //binary of address
    binary(ad);

    int offval = binary2(0, nob); // offset value
    int asval = binary2(nob, ib); // associativity set value
    //int tag=binary2(ib+nob,11-ib-nob); 	// tag value

    qval = ad / bz;
    bsad = qval * bz;
    int tag = bsad;

    int ss = cz1 / cas;  // set size level 1 cache
    int ss1 = cz2 / cas; // set size level 2 cache

    int ll = asval * ss; // level 1 cache lower value of set index
    int ul = ll + ss;    // level 1 cache upper value of set index

    int ll1 = asval * ss1; // level 2 cache lower value of set index
    int ul1 = ll1 + ss1;   // level 2 cache lower value of set index

    //check in level 1 cache
    for (i = ll; i < ul; i++)
    {
        if (ca[i][0] == tag)
        {
            cycles = cycles + ct1 - 1 + mt;
            stall = stall + ct1 - 1 + mt;
            stallc = stallc + ct1 - 1 + mt;
            fo = 1;
            break;
        }
    }

    //found in level 1 cache
    if (fo == 1)
    {
        chit1++;
        ip = ca[i][1];
        ca[i][1] = 1;
        for (j = ll; j < ul; j++)
        {
            if (ca[j][1] < ip && j != i && ca[j][1] != -1)
            {
                ca[j][1] = ca[j][1] + 1;
            }
        }
        ca[i][2 + offval] = val;
        mem[ad] = val;
        return;
        //return ca[i][2+offval];
    }

    //Not found in level 1 cache
    else
    {
        cmiss1++;

        mem[ad] = val;

        //check wheter space is there in level 1 cache
        for (k = ll; k < ul; k++)
        {
            if (ca[k][0] == -1)
            {
                sa = 1;
                break;
            }
        }

        //space available level 1 cache
        if (sa == 1)
        {
            ca[k][0] = tag;

            //check in level 2 cache
            for (i = ll1; i < ul1; i++)
            {
                if (ca2[i][0] == tag)
                {
                    cycles = cycles + ct1 - 1;
                    cycles = cycles + ct2 + mt;
                    stall = stall + ct2 + ct1 - 1 + mt;
                    stallc = stallc + ct2 + ct1 - 1 + mt;
                    fo1 = 1;
                    break;
                }
            }

            //found in level 2 cache
            if (fo1 == 1)
            {
                chit2++;
                ip = ca2[i][1];
                ca2[i][1] = 1;
                for (j = ll1; j < ul1; j++)
                {
                    if (ca2[j][1] > ip && j != i && ca2[j][1] != -1)
                    {
                        ca2[j][1] = ca2[j][1] - 1;
                    }
                }
                for (int q1 = 0; q1 < bz; q1++)
                {
                    ca[k][2 + q1] = ca2[i][2 + q1];
                }

                for (int h1 = 0; h1 < 2 + bz; h1++)
                    ca2[i][h1] = -1;

                ip = ca[k][1];
                ca[k][1] = 1;
                for (int j7 = ll; j7 < ul; j7++)
                {
                    if (j7 != k && ca[j7][1] != -1)
                    {
                        ca[j7][1] = ca[j7][1] + 1;
                    }
                }
                ca[k][2 + offval] = val;
                //mem[ad]=val;
                return;
                //return ca2[i][2+offval];
            }

            //not found in level 2 cache
            else
            {
                cmiss2++;
                cycles = cycles + ct1 - 1;
                cycles = cycles + ct2 + mt;

                stall = stall + ct1 - 1 + mt + ct2;
                stallc = stallc + ct1 - 1 + mt + ct2;

                qval = ad / bz;
                bsad = qval * bz; // memory block starting address
                for (int q1 = 0; q1 < bz; q1++)
                {
                    ca[k][2 + q1] = mem[bsad + q1];
                }
                ip = ca[k][1];
                ca[k][1] = 1;
                for (j = ll; j < ul; j++)
                {
                    if (j != k && ca[j][1] != -1)
                    {
                        ca[j][1] = ca[j][1] + 1;
                    }
                }
                return;
                //return ca[k][2+offval];
            }
        }

        //space not available
        else
        {
            for (j = ll; j < ul; j++)
            {
                if (ca[j][1] == ss)
                {
                    //ca[j][0]=tag;

                    //check in level 2 cache
                    for (i = ll1; i < ul1; i++)
                    {
                        if (ca2[i][0] == tag)
                        {
                            cycles = cycles + ct1 - 1;
                            cycles = cycles + ct2 + mt;
                            stall = stall + ct2 + ct1 - 1 + mt;
                            stallc = stallc + ct2 + ct1 - 1 + mt;
                            fo1 = 1;
                            break;
                        }
                    }

                    //found in level 2 cache
                    if (fo1 == 1)
                    {
                        chit2++;
                        ip = ca2[i][1];
                        ca2[i][1] = 1;
                        for (int j23 = ll1; j23 < ul1; j23++)
                        {
                            if (ca2[j23][1] > ip && j23 != i && ca2[j23][1] != -1)
                            {
                                ca2[j23][1] = ca2[j23][1] - 1;
                            }
                        }
                        ca[j][0] = tag;
                        for (int q1 = 0; q1 < bz; q1++)
                        {
                            ca[j][2 + q1] = ca2[i][2 + q1];
                        }

                        for (int h1 = 0; h1 < 2 + bz; h1++)
                            ca2[i][h1] = -1;

                        ip = ca[j][1];
                        ca[j][1] = 1;
                        for (int j1 = ll; j1 < ul; j1++)
                        {
                            if (ca[j1][1] < ip && j1 != j && ca[j1][1] != -1)
                            {
                                ca[j1][1] = ca[j1][1] + 1;
                            }
                        }
                        ca[j][2 + offval] = val;
                        //mem[ad]=val;
                        return;

                        //return ca2[i][2+offval];
                    }

                    //not found in l2 cache
                    else
                    {
                        cmiss2++;
                        cycles = cycles + ct1 - 1;
                        cycles = cycles + ct2 + mt;

                        stall = stall + ct1 - 1 + mt + ct2;
                        stallc = stallc + ct1 - 1 + mt + ct2;

                        // check space available in l2
                        for (k5 = ll1; k5 < ul1; k5++)
                        {
                            if (ca2[k5][0] == -1)
                            {
                                sa1 = 1;
                                break;
                            }
                        }

                        // space available l2 cache
                        if (sa1 == 1)
                        {
                            ca2[k5][0] = ca[j][0];
                            for (int q1 = 0; q1 < bz; q1++)
                            {
                                ca2[k5][2 + q1] = ca[j][2 + q1];
                            }

                            ca2[k5][1] = 1;
                            for (int j7 = ll1; j7 < ul1; j7++)
                            {
                                if (j7 != k5 && ca2[j7][1] != -1)
                                {
                                    ca2[j7][1] = ca2[j7][1] + 1;
                                }
                            }
                        }

                        // space not available
                        else
                        {

                            for (i9 = ll1; i9 < ul1; i9++)
                            {
                                if (ca2[i9][1] == ss1)
                                {
                                    ca2[i9][0] = ca[j][0];
                                    for (int q1 = 0; q1 < bz; q1++)
                                    {
                                        ca2[i9][2 + q1] = ca[j][2 + q1];
                                    }

                                    ip = ca2[i9][1];
                                    ca2[i9][1] = 1;
                                    for (int j1 = ll1; j1 < ul1; j1++)
                                    {
                                        if (ca2[j1][1] < ip && j1 != i9 && ca2[j1][1] != -1)
                                        {
                                            ca2[j1][1] = ca2[j1][1] + 1;
                                        }
                                    }
                                    break;
                                }
                            }
                        }

                        ca[j][0] = tag;
                        qval = ad / bz;
                        bsad = qval * bz; // memory block starting address
                        for (int q1 = 0; q1 < bz; q1++)
                        {
                            ca[j][2 + q1] = mem[bsad + q1];
                        }
                        ip = ca[j][1];
                        ca[j][1] = 1;
                        for (int j1 = ll; j1 < ul; j1++)
                        {
                            if (ca[j1][1] < ip && j1 != j && ca[j1][1] != -1)
                            {
                                ca[j1][1] = ca[j1][1] + 1;
                            }
                        }
                    }
                    break;
                }
                //return ca[j][2+offval];
            }
            return;
        }
    }
}

void li(string regName, int val)
{
    la2 = val;
    reg[regName] = val;
}

void la(string regName1, int val, string regName2)
{
    int address = reg[regName2] + val;
    la2 = address;
    reg[regName1] = address;
}

void lui(string regName, int val)
{
    la2 = val;
    reg[regName] = val;
}

void lw(string regName1, int val, string regName2)
{
    int address = reg[regName2] + val;
    //reg[regName1] = mem[address];
    reg[regName1] = phase3(address); //--uncomment this line for PHASE 3 IMPLEMENTATION
    la3 = reg[regName1];
}

void sw(string regName1, int val, string regName2)
{
    int address = reg[regName2] + val;
    //mem[address] = reg[regName1];
    phase3w(address, reg[regName1]); //--uncomment this line for PHASE 3 IMPLEMENTATION
}

void add(string r1, string r2, string r3)
{
    la2 = reg[r2] + reg[r3];
    reg[r1] = reg[r2] + reg[r3];
}
void addi(string r1, string r2, int r3)
{
    la2 = reg[r2] + r3;
    reg[r1] = reg[r2] + r3;
}

void sub(string r1, string r2, string r3)
{
    int number;
    istringstream iss(r3);
    iss >> number;
    if (number == 0)
    {
        la2 = reg[r2] - reg[r3];
        reg[r1] = reg[r2] - reg[r3];
    }
    else
    {
        la2 = reg[r2] - number;
        reg[r1] = reg[r2] - number;
    }
}

// main program

int main()
{

    //PHASE 3 CACHE INITIALIZATION
    for (int jz = 0; jz < cz1; jz++)
    {
        for (int jz1 = 0; jz1 < 2 + bz; jz1++)
        {
            ca[jz][jz1] = -1;
        }
    }

    for (int jz = 0; jz < cz2; jz++)
    {
        for (int jz1 = 0; jz1 < 2 + bz; jz1++)
        {
            ca2[jz][jz1] = -1;
        }
    }

    //INITIALISATION OF REGISTER VALUES
    reg.insert(pair<string, int>("$zero", 0));
    reg.insert(pair<string, int>("$s0", 0));
    reg.insert(pair<string, int>("$s1", 0));
    reg.insert(pair<string, int>("$s2", 0));
    reg.insert(pair<string, int>("$s3", 0));
    reg.insert(pair<string, int>("$s4", 0));
    reg.insert(pair<string, int>("$s5", 0));
    reg.insert(pair<string, int>("$s6", 0));
    reg.insert(pair<string, int>("$s7", 0));
    reg.insert(pair<string, int>("$t0", 0));
    reg.insert(pair<string, int>("$t1", 0));
    reg.insert(pair<string, int>("$t2", 0));
    reg.insert(pair<string, int>("$t3", 0));
    reg.insert(pair<string, int>("$t4", 0));
    reg.insert(pair<string, int>("$t5", 0));
    reg.insert(pair<string, int>("$t6", 0));
    reg.insert(pair<string, int>("$t7", 0));
    reg.insert(pair<string, int>("$t8", 0));
    reg.insert(pair<string, int>("$t9", 0));
    reg.insert(pair<string, int>("$a0", 0));
    reg.insert(pair<string, int>("$a1", 0));
    reg.insert(pair<string, int>("$a2", 0));
    reg.insert(pair<string, int>("$a3", 0));
    reg.insert(pair<string, int>("$v0", 0));
    reg.insert(pair<string, int>("$v1", 0));
    reg.insert(pair<string, int>("$at", 0));
    reg.insert(pair<string, int>("$k0", 0));
    reg.insert(pair<string, int>("$k1", 0));
    reg.insert(pair<string, int>("$gp", 0));
    reg.insert(pair<string, int>("$sp", 0));
    reg.insert(pair<string, int>("$fp", 0));
    reg.insert(pair<string, int>("$ra", 0));

    //READING PROGRAM FROM FILE
    ifstream file("bubble.asm");
    vector<vector<string>> programStack;

    int dataCounter = 0;

    string program;
    int takingData = 0;
    while (getline(file, program))
    {
        replace(program.begin(), program.end(), ',', ' '); //remove commas

        stringstream ss(program);
        string item;
        vector<string> splittedStrings;

        int commentFlag = 0;

        int progstack = 0;
        int finish = 0;

        while (getline(ss, item, ' '))
        {
            if (!item.compare(""))
                continue;
            if (!item.compare(".data"))
            {
                takingData = 2;
                continue;
            }
            if (takingData == 2)
            {
                takingData = 3;
                continue;
            }
            if (takingData == 3)
            {
                takingData = 1;
                continue;
            }
            if (takingData == 1)
            {
                if (!item.compare(".globl") || !item.compare(".text"))
                {
                    takingData = 0;
                    continue;
                }
                else //taking data into memory
                {
                    int number;
                    istringstream iss(item);
                    iss >> number;

                    mem[dataCounter] = number;

                    dataCounter++;
                }
            }

            if ((*item.begin()) == '#') //removing comments
                commentFlag = 1;
            //remove unnecessary spaces
            item.erase(remove_if(item.begin(),
                                 item.end(),
                                 bind(isspace<char>,
                                      placeholders::_1,
                                      locale::classic())),
                       item.end());
            if (*(item.rbegin()) == ':') //storing jump labels
            {

                item = item.substr(0, item.size() - 1);
                labels[item] = PC;

                continue;
            }

            if (commentFlag == 0 && takingData == 0)
            {
                progstack = 1;
                //remove spaces
                item.erase(remove_if(item.begin(),
                                     item.end(),
                                     bind(isspace<char>,
                                          placeholders::_1,
                                          locale::classic())),
                           item.end());
                if (!item.compare("jr"))
                {
                    finish = 1;
                    break;
                }
                splittedStrings.push_back(item);
            }
        }

        if (finish == 1)
            break;

        commentFlag = 0;
        if (progstack == 1)
        {
            programStack.push_back(splittedStrings);
            PC++;
        }
    }

    //  PHASE 2 INCLUDES PIPELINE PLEASE REFER TO README FOR INFORMATION REGARDING STALLS CONSIDERED

    int i = 1;

    //if first index is 1 , then do it. Second index is a like a temporary variable whose value will be assigned to first index at the end of a cycle.
    int RF[2], EX[2], MEM[2], WB[2];
    //first index will store the index of the instruction to be handled. Second index is a like a temporary variable whose value will be assigned to first index at the end of a cycle.
    int RF1[2], EX1[2], MEM1[2], WB1[2];

    for (int zq = 0; zq < 2; zq++)
    {
        RF[zq] = 0;
        EX[zq] = 0;
        MEM[zq] = 0;
        WB[zq] = 0;
    }

    for (int zq = 0; zq < 2; zq++)
    {
        RF1[zq] = -1;
        EX1[zq] = -1;
        MEM1[zq] = -1;
        WB1[zq] = -1;
    }
    //variables to manage stalls
    int z1 = 0, z2, z3, z4 = 0, z5, z7;
    //z1 to check completion of the program
    while (z1 != 1)
    {

        cycles++;

        //WB
        if (WB[0] == 1)
        {
            if (WB1[0] == programStack.size() - 1) //last instruction
            {
                z1 = 1;
            }
        }

        //MEM
        if (MEM[0] == 1)
        {

            if (!programStack[MEM1[0]][0].compare("lw"))
            {

                string subNumber = programStack[MEM1[0]][2].substr(0, programStack[MEM1[0]][2].length() - 5);
                int number;
                istringstream iss(subNumber);
                iss >> number;
                number = number / 4;
                string subReg = programStack[MEM1[0]][2].substr(programStack[MEM1[0]][2].length() - 4, 3);
                lw(programStack[MEM1[0]][1], number, subReg);
            }
            else if (!programStack[MEM1[0]][0].compare("sw"))
            {
                string subNumber = programStack[MEM1[0]][2].substr(0, programStack[MEM1[0]][2].length() - 5);
                int number;
                istringstream iss(subNumber);
                iss >> number;
                number = number / 4;
                string subReg = programStack[MEM1[0]][2].substr(programStack[MEM1[0]][2].length() - 4, 3);
                sw(programStack[MEM1[0]][1], number, subReg);
            }

            WB[1] = 1;
            WB1[1] = MEM1[0];

            if (!programStack[MEM1[0]][0].compare("add") || !programStack[MEM1[0]][0].compare("sub") || !programStack[MEM1[0]][0].compare("addi") || !programStack[MEM1[0]][0].compare("li") || !programStack[MEM1[0]][0].compare("la") || !programStack[MEM1[0]][0].compare("lui") || !programStack[MEM1[0]][0].compare("slt"))
            {
                la3 = la2;
            }
        }

        //EX
        z5 = 0;
        if (EX[0] == 1)
        {
            if (MEM1[0] != -1)
            {

                if (!programStack[MEM1[0]][0].compare("lw"))
                {

                    if (!programStack[EX1[0]][0].compare("add") || !programStack[EX1[0]][0].compare("sub") || !programStack[EX1[0]][0].compare("addi") || !programStack[EX1[0]][0].compare("slt"))
                    {

                        if (!programStack[EX1[0]][2].compare(programStack[MEM1[0]][1]) || !programStack[EX1[0]][3].compare(programStack[MEM1[0]][1]))
                        {
                            //lw r1, r4
                            //add r3, r2, r1
                            //1 stall
                            stall++;
                            z5 = 1;
                        }
                    }
                }
            }

            if (z5 == 1)
            {
                MEM[1] = 0;
                MEM1[1] = -1;
                goto label;
            }
            else
            {
                MEM[1] = 1;
                MEM1[1] = EX1[0];

                if (!programStack[EX1[0]][0].compare("add"))
                {
                    add(programStack[EX1[0]][1], programStack[EX1[0]][2], programStack[EX1[0]][3]);
                }
                else if (!programStack[EX1[0]][0].compare("addi"))
                {
                    int number;
                    istringstream iss(programStack[EX1[0]][3]);
                    iss >> number;
                    addi(programStack[EX1[0]][1], programStack[EX1[0]][2], number);
                }
                else if (!programStack[EX1[0]][0].compare("sub"))
                {
                    sub(programStack[EX1[0]][1], programStack[EX1[0]][2], programStack[EX1[0]][3]);
                }
                else if (!programStack[EX1[0]][0].compare("li"))
                {
                    int number;
                    istringstream iss(programStack[EX1[0]][2]);
                    iss >> number;
                    li(programStack[EX1[0]][1], number);
                }
                else if (!programStack[EX1[0]][0].compare("la"))
                {
                    string subNumber = programStack[EX1[0]][2].substr(0, programStack[EX1[0]][2].length() - 5);
                    int number;
                    istringstream iss(subNumber);
                    iss >> number;
                    number = number / 4;
                    string subReg = programStack[EX1[0]][2].substr(programStack[EX1[0]][2].length() - 4, 3);
                    la(programStack[EX1[0]][1], number, subReg);
                }
                else if (!programStack[EX1[0]][0].compare("lui"))
                {
                    int number;
                    istringstream iss(programStack[EX1[0]][2]);
                    iss >> number;
                    lui(programStack[EX1[0]][1], number);
                }
                else if (!programStack[EX1[0]][0].compare("slt"))
                {
                    int first = reg[programStack[EX1[0]][2]];
                    int second = reg[programStack[EX1[0]][3]];

                    if (first < second)
                    {
                        la2 = 1;
                        reg[programStack[EX1[0]][1]] = 1;
                    }
                    else
                    {
                        la2 = 0;
                        reg[programStack[EX1[0]][1]] = 0;
                    }
                }
                else if (!programStack[EX1[0]][0].compare("lw"))
                {

                    string subNumber = programStack[EX1[0]][2].substr(0, programStack[EX1[0]][2].length() - 5);
                    int number;
                    istringstream iss(subNumber);
                    iss >> number;
                    number = number / 4;
                    string subReg = programStack[EX1[0]][2].substr(programStack[EX1[0]][2].length() - 4, 3);
                    la2 = reg[subReg] + number;
                }
                else if (!programStack[EX1[0]][0].compare("sw"))
                {
                    string subNumber = programStack[EX1[0]][2].substr(0, programStack[EX1[0]][2].length() - 5);
                    int number;
                    istringstream iss(subNumber);
                    iss >> number;
                    number = number / 4;
                    string subReg = programStack[EX1[0]][2].substr(programStack[EX1[0]][2].length() - 4, 3);
                    la2 = reg[subReg] + number;
                }
            }
        }
        else
        {
            MEM[1] = 0;
            MEM1[1] = -1;
        }

        //RF
        z3 = 0;
        z7 = 0;
        if (RF[0] == 1)
        {
            if (!programStack[RF1[0]][0].compare("add") || !programStack[RF1[0]][0].compare("sub") || !programStack[RF1[0]][0].compare("addi") || !programStack[RF1[0]][0].compare("li") || !programStack[RF1[0]][0].compare("la") || !programStack[RF1[0]][0].compare("lui") || !programStack[RF1[0]][0].compare("lw") || !programStack[RF1[0]][0].compare("sw") || !programStack[RF1[0]][0].compare("beq") || !programStack[RF1[0]][0].compare("slt") || !programStack[RF1[0]][0].compare("j") || !programStack[RF1[0]][0].compare("bne"))
            {

                if (EX1[1] != -1)
                {

                    if (!programStack[RF1[0]][0].compare("beq") || !programStack[RF1[0]][0].compare("bne") || !programStack[RF1[0]][0].compare("bgt"))
                    {

                        if (!programStack[EX1[0]][0].compare("add") || !programStack[EX1[0]][0].compare("sub") || !programStack[EX1[0]][0].compare("addi") || !programStack[EX1[0]][0].compare("li") || !programStack[EX1[0]][0].compare("slt"))
                        {

                            if (!programStack[RF1[0]][1].compare(programStack[EX1[0]][1]) || !programStack[RF1[0]][2].compare(programStack[EX1[0]][1]))
                            {
                                //add r1,r2,r3
                                //beq r1, r4, label
                                stall++;
                                z3 = 1;
                            }
                        }

                        if (!programStack[EX1[0]][0].compare("lw"))
                        {

                            if (!programStack[RF1[0]][1].compare(programStack[EX1[0]][1]) || !programStack[RF1[0]][2].compare(programStack[EX1[0]][1]))
                            {
                                //lw r1, 0(r2)
                                //beq r1,r3,label
                                //2 stalls.. z4 will be incremented twice
                                stall++;
                                z4++;
                            }
                        }

                        if (z3 == 0 && z4 == 0 && MEM1[0] != -1)
                        {

                            if (!programStack[MEM1[0]][0].compare("lw"))
                            {

                                if (!programStack[RF1[0]][1].compare(programStack[MEM1[0]][1]) || !programStack[RF1[0]][2].compare(programStack[MEM1[0]][1]))
                                {
                                    //lw r1, 0(r2)
                                    //add r4,r5,r6
                                    //beq r1,r3,label
                                    stall++;
                                    z7 = 1;
                                }
                            }
                        }
                    }
                }

                if (z7 == 1)
                {
                    EX[1] = 0;
                    EX1[1] = -1;
                    goto label;
                }

                if (z3 == 1)
                {
                    EX[1] = 0;
                    EX1[1] = -1;
                    goto label;
                }

                if (z4 == 1)
                {
                    EX[1] = 0;
                    //not changing EX1[1] so it can again come and increment z4
                    goto label;
                }

                if (z4 == 2)
                {
                    EX[1] = 0;
                    EX1[1] = -1;
                    z4 = 0;
                    goto label;
                }

                if (z3 == 0 && z4 == 0 && z7 == 0)
                {
                    EX[1] = 1;
                    EX1[1] = RF1[0];
                    if (!programStack[RF1[0]][0].compare("bgt"))
                    {
                        int first = reg[programStack[RF1[0]][1]];
                        int second = reg[programStack[RF1[0]][2]];
                        if (first > second)
                            i = labels[programStack[RF1[0]][3]];
                    }
                    else if (!programStack[RF1[0]][0].compare("blt"))
                    {
                        int first = reg[programStack[RF1[0]][1]];
                        int second = reg[programStack[RF1[0]][2]];
                        if (first < second)
                            i = labels[programStack[RF1[0]][3]];
                    }
                    else if (!programStack[RF1[0]][0].compare("bne"))
                    {
                        int first = reg[programStack[RF1[0]][1]];
                        int second = reg[programStack[RF1[0]][2]];

                        if (first != second)
                            i = labels[programStack[RF1[0]][3]];
                    }
                    else if (!programStack[RF1[0]][0].compare("beq"))
                    {
                        int first = reg[programStack[RF1[0]][1]];
                        int second = reg[programStack[RF1[0]][2]];

                        if (first == second)
                            i = labels[programStack[RF1[0]][3]];
                    }
                    else if (!programStack[RF1[0]][0].compare("j"))
                    {
                        i = labels[programStack[RF1[0]][1]];
                    }
                }
            }
        }
        else
        {
            EX[1] = 0;
            EX1[1] = -1;
        }

        //IF
        if (i < programStack.size())
        {

            z2 = 0;
            if (!programStack[i][0].compare("add") || !programStack[i][0].compare("sub") || !programStack[i][0].compare("addi") || !programStack[i][0].compare("li") || !programStack[i][0].compare("la") || !programStack[i][0].compare("lui") || !programStack[i][0].compare("lw") || !programStack[i][0].compare("sw") || !programStack[i][0].compare("beq") || !programStack[i][0].compare("slt") || !programStack[i][0].compare("j") || !programStack[i][0].compare("bne"))
            {

                //beq r1,r2 label
                //any instruction
                //1 stall
                if (i != 0)
                {
                    if (RF1[0] != -1)
                    {
                        if (!programStack[RF1[1]][0].compare("beq") || !programStack[RF1[1]][0].compare("bgt") || !programStack[RF1[1]][0].compare("j") || !programStack[RF1[1]][0].compare("bne"))
                        {
                            stall++;
                            z2 = 1;
                        }
                    }
                }

                if (z2 == 0)
                {
                    inst++;
                    RF1[1] = i;
                    RF[1] = 1;
                    i++;
                }
                else
                {
                    RF1[1] = -1;
                    RF[1] = 0;
                }
            }
        }

        else
        {
            RF1[1] = -1;
            RF[1] = 0;
        }

    label:

        //rf update
        RF[0] = RF[1];
        EX[0] = EX[1];
        MEM[0] = MEM[1];
        WB[0] = WB[1];

        //index update
        RF1[0] = RF1[1];
        EX1[0] = EX1[1];
        MEM1[0] = MEM1[1];
        WB1[0] = WB1[1];
    }

    //PHASE 3 PRINT STATEMENTS

    cout << " cache " << endl;

    cout << "level 1 " << endl;
    for (int jz = 0; jz < cz1; jz++)
    {
        cout << "tag " << ca[jz][0] << " reltive access " << ca[jz][1] << " "
             << "data stored :- ";
        for (int jz1 = 2; jz1 < 2 + bz; jz1++)
        {
            cout << ca[jz][jz1] << " ";
        }
        cout << endl;
    }
    cout << "level 2 " << endl;
    for (int jz = 0; jz < cz2; jz++)
    {
        cout << "tag " << ca2[jz][0] << " reltive access " << ca2[jz][1] << " "
             << "data stored :- ";
        for (int jz1 = 2; jz1 < 2 + bz; jz1++)
        {
            cout << ca2[jz][jz1] << " ";
        }
        cout << endl;
    }

    cout << "REGISTERS\n";

    map<std::string, int>::iterator it = reg.begin();

    while (it != reg.end())
    {
        // Accessing KEY from element pointed by it.
        string word = it->first;

        // Accessing VALUE from element pointed by it.
        int count = it->second;

        cout << word << " :: " << count << endl;

        // Increment the Iterator to point to next entry
        it++;
    }

    cout << "\nMEMORY\n";
    for (int i = 0; i < dataCounter; i++)
    {
        cout << mem[i] << "  ";
    }

    cout << endl
         << endl;
    cout << "No of instruction --> " << inst << " " << endl;
    cout << "No of cycles      --> " << cycles << " " << endl;
    cout << "Total Stalls      --> " << stall << " " << endl;
    cout << "CPI               --> " << cycles * 1.0 / inst << endl;
    ;

    //    PHASE 3 PRINT STATEMENTS
    cout << "MEM stalls        --> " << stallc << " " << endl;
    cout << "cache hit level 1 --> " << chit1 << " " << endl;
    cout << "cache miss level 1--> " << cmiss1 << " " << endl;
    cout << "cache hit level 2 --> " << chit2 << " " << endl;
    cout << "cache miss level 2--> " << cmiss2 << " " << endl;
}
