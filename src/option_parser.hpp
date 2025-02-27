
#ifndef option_parser_hpp
#define option_parser_hpp

#include <stdio.h>
#include <iostream>
#include <unordered_map> // for std::unordered_map
#include <vector>
#include "mfem.hpp"
#include "option_types.hpp"

class ExaOptions {
   public:

      // mesh variables
      std::string mesh_file;
      MeshType mesh_type;
      double mxyz[3]; // edge dimensions (mx, my, mz)
      int  nxyz[3]; // number of cells on an edge (nx, ny, nz)


      // serial and parallel refinement levels
      int ser_ref_levels;
      int par_ref_levels;

      // polynomial interpolation order
      int order;

      // final simulation time and time step (set each to 1.0 for
      // single step debug)
      double t_final;
      double dt;
      // We have a custom dt flag
      bool dt_cust;
      // Number of time steps to take
      int nsteps;
      // File to read the custom time steps from
      std::string dt_file;
      // Vector to hold custom time steps if there are any
      mfem::Vector cust_dt;

      // visualization input args
      int vis_steps;
      // visualization variable for visit
      bool visit;
      bool conduit;
      bool paraview;
      bool adios2;
      // Where to store the end time step files
      std::string basename;
      // average stress file name
      std::string avg_stress_fname;
      std::string avg_pl_work_fname;
      std::string avg_dp_tensor_fname;
      std::string avg_def_grad_fname;
      bool additional_avgs;

      // newton input args
      double newton_rel_tol;
      double newton_abs_tol;
      int newton_iter;
      NLSolver nl_solver;

      // Integration type
      IntegrationType integ_type;

      // solver input args
      // GMRES is currently set as the default iterative solver
      // until the bug in the PCG solver is found and fixed.
      bool grad_debug;
      double krylov_rel_tol;
      double krylov_abs_tol;
      int krylov_iter;

      KrylovSolver solver;

      // input arg to specify crystal plasticity
      bool cp;

      // The type of mechanical interface that we'll be using
      MechType mech_type;
      // The slip and hardening laws being used for ExaCMech
      SlipType slip_type;
      // Specify the xtal type we'll be using - used if ExaCMech is being used
      XtalType xtal_type;
      // Specify the temperature of the material
      double temp_k;


      // grain input arguments
      std::string ori_file; // grain orientations (F_p_inv for Curt's UMAT?)
      std::string grain_map; // map of grain id to element centroid
      int ngrains;
      OriType ori_type;
      int grain_custom_stride; // TODO check that this is used with "grain_custom"
      int grain_statevar_offset;

      // material properties input arguments
      std::string props_file;
      int nProps; // at least have one dummy property

      // state variables file with constant values used to initialize ALL integration points
      std::string state_file;
      int numStateVars; // at least have one dummy property

      // boundary condition input args
      bool changing_bcs = false;
      std::vector<int> updateStep;
      // vector of velocity components for each attribute in ess_id
      std::unordered_map<int, std::vector<double>> map_ess_vel;
      // component combo (x,y,z = -1, x = 1, y = 2, z = 3,
      // xy = 4, yz = 5, xz = 6, free = 0
      std::unordered_map<int, std::vector<int>> map_ess_comp;
      // essential bc ids for the whole boundary
      std::unordered_map<int, std::vector<int>> map_ess_id;

      // Parse the TOML file for all of the various variables.
      // In other words this is our driver to get all of the values.
      void parse_options(int my_id);

      RTModel rtmodel;
      Assembly assembly;

      ExaOptions(std::string _floc) : floc{_floc}
      {
         // Matl and State Property related variables
         numStateVars = 1;
         nProps = 1;
         state_file = "state.txt";
         props_file = "props.txt";

         // Grain related variables
         grain_statevar_offset = -1;
         grain_custom_stride = 1;
         ori_type = OriType::EULER;
         ngrains = 0;
         grain_map = "grain_map.txt";
         ori_file = "ori.txt";

         // Model related parameters
         cp = false;
         // umat = false;
         // Want all of these to be not set. If they aren't specified
         // then we want other things to fail in our driver file.
         mech_type = MechType::NOTYPE;
         // The slip and hardening laws being used for ExaCMech
         slip_type = SlipType::NOTYPE;
         // Specify the xtal type we'll be using - used if ExaCMech is being used
         xtal_type = XtalType::NOTYPE;
         // Specify the temperature of the material
         temp_k = 298.;

         // Krylov Solver related variables
         // We set the default solver as GMRES in case we accidentally end up dealing
         // with a nonsymmetric matrix for our linearized system of equations.
         solver = KrylovSolver::GMRES;
         krylov_rel_tol = 1.0e-10;
         krylov_abs_tol = 1.0e-30;
         krylov_iter = 200;

         // NR parameters
         newton_rel_tol = 1.0e-5;
         newton_abs_tol = 1.0e-10;
         newton_iter = 25;
         nl_solver = NLSolver::NR;
         grad_debug = false;

         // Integration type parameters
         integ_type = IntegrationType::FULL;

         // Visualization related parameters
         basename = "results/exaconstit";
         visit = false;
         conduit = false;
         paraview = false;
         adios2 = false;
         vis_steps = 1;
         //
         avg_stress_fname = "avg_stress.txt";
         avg_pl_work_fname = "avg_pl_work.txt";
         avg_def_grad_fname = "avg_def_grad.txt";
         avg_dp_tensor_fname = "avg_dp_tensor.txt";
         additional_avgs = false;

         // Time step related parameters
         t_final = 1.0;
         dt = 1.0;
         dt_cust = false;
         nsteps = 1;
         dt_file = "custom_dt.txt";

         // Mesh related variables
         ser_ref_levels = 0;
         par_ref_levels = 0;
         order = 1;
         mesh_file = "../../data/cube-hex-ro.mesh";
         mesh_type = MeshType::OTHER;

         mxyz[0] = 1.0;
         mxyz[1] = 1.0;
         mxyz[2] = 1.0;

         nxyz[0] = 1;
         nxyz[1] = 1;
         nxyz[2] = 1;

         assembly = Assembly::FULL;
         rtmodel = RTModel::CPU;
      } // End of ExaOptions constructor

      virtual ~ExaOptions() {}

   protected:
      std::string floc;
      // From the toml file it finds all the values related to state and mat'l
      // properties
      void get_properties();

      // From the toml file it finds all the values related to the BCs
      void get_bcs();

      // From the toml file it finds all the values related to the model
      void get_model();

      // From the toml file it finds all the values related to the time
      void get_time_steps();

      // From the toml file it finds all the values related to the visualizations
      void get_visualizations();

      // From the toml file it finds all the values related to the Solvers
      void get_solvers();

      // From the toml file it finds all the values related to the mesh
      void get_mesh();

      // Prints out a list of all the options being used
      void print_options();
};




#endif /* option_parser_hpp */
