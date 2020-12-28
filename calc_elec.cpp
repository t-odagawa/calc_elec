//#define DEBUG

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

const double sigmax = 2.60; // mm
const double sigmay = 0.16; // mm
const double sigmaz = 20.0; // mm

const double kinetic_energy = 6.5e6; // keV
const double electron_mass = 511.; // keV
const double gamma_factor = kinetic_energy / electron_mass;
const double beta = sqrt(1. - 1. / gamma_factor / gamma_factor);

const double electron_charge = 1.602e-19; // Coulomb
const double current = 60.e-3; // A
const double frequency = 508.6e6; // Hz
const double number_of_electron = current / frequency / electron_charge;

const double electric_constant = 8.854e-15; // Falad mm-1

double ChargeDensityAt(double xi, double eta, double zeta) {
  return electron_charge * number_of_electron
    * exp(- (xi   * xi)   / (2 * sigmax * sigmax)
	  - (eta  * eta)  / (2 * sigmay * sigmay)
	  - (zeta * zeta) / (2 * sigmaz * sigmaz))
    / (2 * pow(M_PI, 1.5) * sigmax * sigmay * sigmaz);
}

double FieldContributionFrom(double x, double y, double z,
			     double xi, double eta, double zeta) {
  return gamma_factor / (4 * M_PI * electric_constant)
    * (x - xi) * ChargeDensityAt(xi, eta, zeta)
    / pow((gamma_factor*gamma_factor*(z-zeta)*(z-zeta) + (y-eta)*(y-eta) + (x-xi)*(x-xi)), 1.5);
}

double ElectricFieldAt(double x, double y, double z,
		       double sigma_range, int number_of_cells) {

  double cellx = sigma_range * sigmax / number_of_cells;
  double celly = sigma_range * sigmay / number_of_cells;
  double cellz = sigma_range * sigmaz / number_of_cells;
  
  double electric_field = 0;
  for(int ixi = -number_of_cells; ixi < number_of_cells; ixi++) {
    for(int ieta = -number_of_cells; ieta < number_of_cells; ieta++) {
      for(int izeta = -number_of_cells; izeta < number_of_cells; izeta++) {
	
	double xi   = cellx * ((double)ixi   + 0.);
	double eta  = celly * ((double)ieta  + 0.);
	double zeta = cellz * ((double)izeta + 0.);
	electric_field += FieldContributionFrom(x, y, z, xi, eta, zeta)
	  * (cellx * celly * cellz);
#ifdef DEBUG
	cout << xi << " " << eta << " " << zeta << endl;
	cout << FieldContributionFrom(x,y,z,xi,eta,zeta) * (cellx * celly * cellz) << endl;
	cout << electric_field << endl;	
#endif
      }
    }
  }

  return electric_field;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    cerr << "Usage : " << argv[0]
	 << " <sigma range> <number of cells in one dimension>" << endl;
    exit(1);
  }

  string common_file_name("electric_field_");
  string adder("_");
  string txt_extension(".txt");
  string output_file_name  = common_file_name + argv[1]
    + adder + argv[2] + txt_extension;
  cerr << output_file_name << endl;
  ofstream ofs(output_file_name);

  double sigma_range = atoi(argv[1]);
  int number_of_cells = atoi(argv[2]);

  double x = 5. * sigmax;
  double y = 0.;
  double z;

  cout << "Loop start!" << endl;
  for(int it = -100; it < 100; it++) { // mm (time * beta)
#ifdef DEBUG
    cerr <<  it << " [mm]";
#endif
    z = (double)it;
    ofs << it << "\t" << ElectricFieldAt(x, y, z, sigma_range, number_of_cells) << endl;
  }

  cout << "Finish!!" << endl;
  exit(0);
}
