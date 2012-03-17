#pragma once
#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "timer.h"
#include "unpickle.h"
#include "neighlistlike.h"

#include <cmath>
#include <sstream>
#include <vector>

// TIMERS
std::vector<Timer> one_time;
std::vector<Timer> nl_refresh;
std::vector<Timer> per_iter;
std::vector<std::vector<double> > per_iter_timings;

/*
 * Each implementation must define this procedure.
 *
 * The implementation is responsible for assigning costs to three buckets:
 *   - [one_time]
 *   - [nl_refresh]
 *   - [per_iter]
 */
extern void run(struct params *input);

void print_usage(const char *progname);
int parse_cmdline(struct params *&input, int argc, char **argv);
void process_results(struct params *p);

int main(int argc, char **argv) {
  struct params *p;

  int parse_error = parse_cmdline(p, argc, argv);
  if (parse_error) {
    printf("ERROR: during parse\n");
    return parse_error;
  }
  run(p);
  process_results(p);

  delete_params(p);
  return 0;
}

void print_usage(const char *progname) {
  printf("Usage: %s <stepfile> [options]\n", progname);
  printf("Options:\n");
  printf("   -n <int>     number of runs\n");
  printf("   -c           checks off\n");
  printf("   -d           debug on\n");
  printf("   -v           be verbose\n");
  printf("   -a <errfile> error output\n");
  printf("   -b <rawfile> raw sample output\n");
  printf("   -k TPA|BPA   set cl kernel    ]            \n");
  printf("   -w <int>     set cl blocksize ]            \n");
  printf("   -x <int>     set cl platform  ]            \n");
  printf("   -y <int>     set cl device    ] OpenCL only\n");
  printf("   -z <str>     set cl flags     ]            \n");
}

int parse_cmdline(struct params *&p, int argc, char **argv) {
  // mandatory arguments
  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }
  std::string step_filename(argv[1]);
  p = parse_file(step_filename);
  p->progname = argv[0];
  argc--;
  argv++;

  // defaults 
  p->num_iter = 100;
  p->check = true;
  p->debug = false;
  p->verbose = false;
  p->errfile = NULL;
  p->rawfile = NULL;
  p->cl_kernel = 0;
  p->cl_blocksize = 32;
  p->cl_platform = 0;
  p->cl_device = 0;
  p->cl_flags = NULL;

  int c;
  while ((c = getopt (argc, argv, "hn:cdva:b:k:w:x:y:z:")) != -1) {
    switch (c) {
      case 'h':
        print_usage(argv[0]);
        return 1;
      case 'n':
        p->num_iter = atoi(optarg);
        break;
      case 'c':
        p->check = false;
        break;
      case 'd':
        p->debug = true;
        break;
      case 'v':
        p->verbose = true;
        break;
      case 'a':
        p->errfile = optarg;
        break;
      case 'b':
        p->rawfile = optarg;
        break;
      case 'k':
        // use some magic values
        if      (strcmp(optarg, "TPA") == 0) p->cl_kernel = 0;
        else if (strcmp(optarg, "BPA") == 0) p->cl_kernel = 1;
        else {
          printf("Unknown kernel type [%s]\n", optarg);
          return 1;
        }
        break;
      case 'w':
        p->cl_blocksize = atoi(optarg);
        break;
      case 'x':
        p->cl_platform = atoi(optarg);
        break;
      case 'y':
        p->cl_device = atoi(optarg);
        break;
      case 'z':
        p->cl_flags = optarg;
        break;
      case '?':
        if (optopt == 'n' ||
            optopt == 'a' || optopt == 'b' ||
            optopt == 'k' ||optopt == 'w' ||
            optopt == 'x' || optopt == 'y' || optopt == 'z')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
              "Unknown option character `\\x%x'.\n",
              optopt);
        return 1;
      default:
        printf("Aborting\n");
        abort();
    }
  }

  return 0;
}

void process_results(struct params *p) {
  int num_iter = p->num_iter;
  assert(per_iter.size() == per_iter_timings.size());
  for (int i=0; i<(int)per_iter_timings.size(); i++) {
    assert((int)per_iter_timings[i].size() == num_iter);
  }
  double one_time_total = 0.0f;
  double per_iter_total = 0.0f;
  for (int i=0; i<(int)one_time.size(); i++) {
    one_time_total += one_time[i].get_total_time();
  }
  for (int i=0; i<(int)per_iter.size(); i++) {
    per_iter_total += per_iter[i].get_total_time();
  }

  if (p->verbose) { //print header
    printf("# Program: %s\n", p->progname);
    printf("# Num Iterations: %d\n", p->num_iter);
    printf("# nedge, total_one_time_cost (milliseconds), time_per_iteration");
    for (int i=0; i<(int)one_time.size(); i++) {
      printf(", [%s]", one_time[i].get_name().c_str());
    }
    for (int i=0; i<(int)nl_refresh.size(); i++) {
      printf(", (%s)", nl_refresh[i].get_name().c_str());
    }
    for (int i=0; i<(int)per_iter.size(); i++) {
      printf(", %s, min, max", per_iter[i].get_name().c_str());
    }
    printf("\n");
  }

  printf("%d, %f, %f", p->nedge, one_time_total, per_iter_total / (double) num_iter);
  for (int i=0; i<(int)one_time.size(); i++) {
    printf(", %f", one_time[i].get_total_time());
  }
  for (int i=0; i<(int)nl_refresh.size(); i++) {
    printf(", %f", nl_refresh[i].get_total_time());
  }
  for (int i=0; i<(int)per_iter.size(); i++) {
    double min = *min_element(per_iter_timings[i].begin(), per_iter_timings[i].end());
    double max = *max_element(per_iter_timings[i].begin(), per_iter_timings[i].end());
    printf(", %f, %f, %f", per_iter[i].get_total_time() / (double) num_iter, min, max);
  }
  printf("\n");
  fflush(stdout);

  // print out raw sample data
  if (p->rawfile != NULL) {
    std::ofstream fp;
    fp.open(p->rawfile);
    fp << "# run";
    for (int i=0; i<(int)per_iter.size(); i++) {
      fp << ", " << per_iter[i].get_name();
    }
    fp << std::endl;
    for (int run=0; run<num_iter; run++) {
      fp << run;
      for (int i=0; i<(int)per_iter_timings.size(); i++) {
        fp << ", " << per_iter_timings[i][run];
      }
      fp << std::endl;
    }
  }
}

double percentage_error(double const&expected, double const &computed) {
  if (expected == computed) return 0.0;
  // avoid div0 case (send back sentinel value instead)
  // we know computed is not 0.0 because of case above
  if (expected == 0.0) return 999.9;
  return 100 * fabs((computed - expected) / expected);
}

double compare(const char *tag, double const&expected, double const&computed,
               const double threshold,
               bool verbose, bool die_on_flag, std::ostream &out) {
  static int num_bad = 0;

  double error = percentage_error(expected, computed);
  bool flag = (error > threshold);
  if (flag) {
    num_bad++;
  }
  if (flag || verbose) {
    out << tag << ", "
        << expected << ", "
        << computed << ", "
        << error 
        << std::endl;
  }
  if (flag && die_on_flag) {
    exit(1);
  }
  return error;
}

void check_result(struct params *p, NeighListLike *nl,
                  double *force, double *torque, double **shearlist,
                  const double threshold, bool verbose, bool die_on_flag) {
  std::ostream *out = &std::cout;
  std::ofstream fp;
  std::string taghead;
  if (p->errfile != NULL) {
    fp.open(p->errfile);
    out = &fp;
    verbose = true;
    taghead = "";
  } else {
    taghead = "# ";
  }
  (*out).precision(std::numeric_limits<double>::digits10);
  for (int i=0; i<p->nnode*3; i++) {
    std::stringstream tag;
    tag << taghead << "force[" << i << "]";
    compare(tag.str().c_str(),
            p->expected_force[i], force[i],
            threshold, verbose, die_on_flag, *out);
  }
  for (int i=0; i<p->nnode*3; i++) {
    std::stringstream tag;
    tag << taghead << "torque[" << i << "]";
    compare(tag.str().c_str(),
            p->expected_torque[i], torque[i],
            threshold, verbose, die_on_flag, *out);
  }
  int ptr = 0;
  double *shear_check = new double[p->nedge*3];
  for (int ii=0; ii<nl->inum; ii++) {
    int i = nl->ilist[ii];
    for (int jj=0; jj<nl->numneigh[i]; jj++) {
      double *shear = &(shearlist[i][3*jj]);
      shear_check[(ptr*3)  ] = shear[0];
      shear_check[(ptr*3)+1] = shear[1];
      shear_check[(ptr*3)+2] = shear[2];
      ptr++;
    }
  }
  assert(ptr == p->nedge);
  for (int i=0; i<p->nedge; i++) {
    std::stringstream tag;
    tag << taghead << "shear[" << i << "]";
    compare(tag.str().c_str(),
            p->expected_shear[i], shear_check[i],
            threshold, verbose, die_on_flag, *out);
  }
  delete[] shear_check;
}

#endif
