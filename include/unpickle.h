#pragma once
#ifndef UNPICKLE_H
#define UNPICKLE_H

#include <fstream>
#include <iostream>
#include <string>

const unsigned int MAGIC_NUMBER = 0xDEADBEEF;

//datastructure from serialized data (input and expected_output)
struct params {
  //constants
  double dt;
  double nktv2p;
  int ntype;
  double *yeff;
  double *geff;
  double *betaeff;
  double *coeffFrict;

  //node data
  int nnode;
  double *x;
  double *v;
  double *omega;
  double *radius;
  double *mass;
  int    *type;
  double *force;
  double *torque;

  //edge data
  int nedge;
  int *edge;
  double *shear;

  //expected results
  double *expected_force;
  double *expected_torque;
  double *expected_shear;

  //command-line arguments
  const char *progname;
  int         num_iter;
  bool        check;
  bool        debug;
  bool        verbose;
  const char *errfile;
  const char *rawfile;
  int         cl_kernel;
  int         cl_blocksize;
  int         cl_platform;
  int         cl_device;
  const char *cl_flags;
};

template<class T>
inline void fill_array(std::ifstream &file, T *array, int num_elements);

struct params *parse_file(std::string fname);

void delete_params(struct params *p);

#endif
