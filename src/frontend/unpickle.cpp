#include "unpickle.h"

//unpickle array
template<class T>
inline void fill_array(std::ifstream &file, T *array, int num_elements) {
  if (file.eof()) {
    std::cout << "Error unexpected eof!\n";
    exit(-1);
  }
  for (int i=0; i<num_elements; i++) {
    file.read(reinterpret_cast<char *>(&(array[i])), sizeof(array[i]));
  }
}

//unpickle file
struct params *parse_file(std::string fname) {
  const char *fname_c_str = fname.c_str();
  std::ifstream file (fname_c_str, std::ifstream::in | std::ifstream::binary);
  if (!file.is_open()) {
    std::cout << "Could not open [" << fname << "]\n";
    exit(-1);
  }
  if (file.bad()) {
    std::cout << "Error with file [" << fname << "]\n";
    exit(-1);
  }

  unsigned int MAGIC;
  file.read(reinterpret_cast<char *>(&MAGIC), sizeof(MAGIC));
  if (MAGIC != MAGIC_NUMBER) {
    printf("Error with file [%s]: magic number is [%x]; expecting [%x]\n",
      fname.c_str(), MAGIC, MAGIC_NUMBER);
    exit(-1);
  }

  struct params *result = new params;
  int ntype;
  int nnode;
  int nedge;

  //constants
  file.read(reinterpret_cast<char *>(&(result->dt)), sizeof(result->dt));
  file.read(reinterpret_cast<char *>(&(result->nktv2p)),
    sizeof(result->nktv2p));
  file.read(reinterpret_cast<char *>(&(ntype)), sizeof(ntype));
  result->ntype = ntype;
  result->yeff       = new double[ntype*ntype];
  result->geff       = new double[ntype*ntype];
  result->betaeff    = new double[ntype*ntype];
  result->coeffFrict = new double[ntype*ntype];
  fill_array(file, result->yeff,       (ntype*ntype));
  fill_array(file, result->geff,       (ntype*ntype));
  fill_array(file, result->betaeff,    (ntype*ntype));
  fill_array(file, result->coeffFrict, (ntype*ntype));

  //node data
  file.read(reinterpret_cast<char *>(&(nnode)), sizeof(nnode));
  result->nnode = nnode;
  result->x      = new double[nnode*3];
  result->v      = new double[nnode*3];
  result->omega  = new double[nnode*3];
  result->radius = new double[nnode  ];
  result->mass   = new double[nnode  ];
  result->type   = new int[nnode];
  result->force  = new double[nnode*3];
  result->torque = new double[nnode*3];
  fill_array(file, result->x,      nnode*3);
  fill_array(file, result->v,      nnode*3);
  fill_array(file, result->omega,  nnode*3);
  fill_array(file, result->radius, nnode);
  fill_array(file, result->mass,   nnode);
  fill_array(file, result->type,   nnode);
  fill_array(file, result->force,  nnode*3);
  fill_array(file, result->torque, nnode*3);

  //edge data
  file.read(reinterpret_cast<char *>(&(nedge)), sizeof(nedge));
  result->nedge = nedge;
  result->edge = new int[nedge*2];
  result->shear = new double[nedge*3];
  fill_array(file, result->edge,  nedge*2);
  fill_array(file, result->shear, nedge*3);

  //expected results
  result->expected_force  = new double[nnode*3];
  result->expected_torque = new double[nnode*3];
  result->expected_shear = new double[nedge*3];
  fill_array(file, result->expected_force,  nnode*3);
  fill_array(file, result->expected_torque, nnode*3);
  fill_array(file, result->expected_shear, nedge*3);

  return result;
}

void delete_params(struct params *p) {
  delete[] p->yeff;
  delete[] p->geff;
  delete[] p->betaeff;
  delete[] p->coeffFrict;

  delete[] p->x;
  delete[] p->v;
  delete[] p->omega;
  delete[] p->radius;
  delete[] p->mass;
  delete[] p->type;
  delete[] p->force;
  delete[] p->torque;

  delete[] p->edge;
  delete[] p->shear;

  delete[] p->expected_force;
  delete[] p->expected_torque;
  delete[] p->expected_shear;

  delete p;
}

