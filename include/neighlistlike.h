#pragma once
#ifndef NEIGHLISTLIKE_H
#define NEIGHLISTLIKE_H

#include "unpickle.h"

class NeighListLike {
  public:
    NeighListLike(struct params *input);
    ~NeighListLike();

    //default sizes
    static const int PGDELTA;
    int maxlocal;
    int maxpage;
    int pgsize;
    int oneatom;

    //list datastructures
    int      inum;
    int     *ilist;
    int     *numneigh;
    int    **firstneigh;
    double **firstdouble;
    int    **firsttouch;
    int    **pages;
    double **dpages;
    int    **tpages;

    void restore();
    void copy_into(double **&, double **&, int **&, int **&);

  private:
    void allocate(int N);
    void add_pages();
    void fill(struct params *input);
    void test_against(struct params *input);

    double **original_dpages;
    int    **original_tpages;
    void backup();
};

#endif
