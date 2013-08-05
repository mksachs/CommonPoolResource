//
//  CPRMController.h
//  CPRM
//
//  Created by Michael Sachs on 9/8/11.
//  Copyright 2011 Me. All rights reserved.
//

//#ifndef CPRM_CPRMController_h
//#define CPRM_CPRMController_h



//#endif

#include "CPRM.h"
#include <map>
#include <vector>
#include <utility>
#include <string>

class CPRMController {
public:
    void initialize(int n, double beta_1, double beta_2, double user_field_strength, double resource_field_strength, double resource_field_increment, std::string output_directory);
    void one_parameter_sweep(Parameter parameter, double start_value, double end_value, double increment, int number_of_single_run_samples, int number_of_fixed_parameter_runs);
    void two_parameter_sweep(Parameter parameter1, double start_value1, double end_value1, double increment1, Parameter parameter2, double start_value2, double end_value2, double increment2, int number_of_single_run_samples, int number_of_fixed_parameter_runs);
    void run_for_n_sweeps(int sweeps, bool ave_states_to_console=false, bool ave_states_to_file=false, bool states_to_file=false);
    bool run_till_equilibrium(int &equilib_sweep);
    //void print_one_parameter_sweep_results(char * file_name_addendum);
    //void run_for_n_sweeps_no_output(int sweeps);
    
    //double user_variance();
    //double resource_variance();
private:
    CPRMDiscreteModel _model;
    //int _sweep;
    int _decreasing_variance_sweep_threshold;
    int _max_sweep_threshold;
    int _sweeps_between_samples;
    double _variance_fluctuation_threshold;
    std::string _output_file_prefix;
    //Parameter _current_sweep_parameter_1;
    //Parameter _current_sweep_parameter_2;
    //Parameter _current_fixed_parameter_1;
    //Parameter _current_fixed_parameter_2;
    //Parameter _current_fixed_parameter_3;
    //Parameter _current_fixed_parameter_4;
    //double _current_sweep_parameter_start_1;
    //double _current_sweep_parameter_end_1;
    //double _current_sweep_parameter_start_2;
    //double _current_sweep_parameter_end_2;
    //double _current_fixed_parameter_values[4];
    
    std::string get_parameter_name(Parameter parameter);
    
    //void run_for_one_correlation_time();
    //void set_current_parameters(Parameter sweep_parameter, double sweep_parameter_start, double sweep_parameter_end);
    
    // for calculating variances
    void update_state_sums(double sweep_parity, double sweep_average_user_state, double sweep_average_resource_state, double sweep_user_conserve_fraction, double sweep_user_neutral_fraction, double sweep_user_deplete_fraction, double sweep_resource_conserve_fraction, double sweep_resource_neutral_fraction, double sweep_resource_deplete_fraction);
    double user_variance(int sweep);
    double resource_variance(int sweep);
    double user_deplete_variance(int sweep);
    double user_neutral_variance(int sweep);
    double user_conserve_variance(int sweep);
    double resource_deplete_variance(int sweep);
    double resource_neutral_variance(int sweep);
    double resource_conserve_variance(int sweep);
    
    double _parity_sum;
    
    double _average_user_state_sum;
    double _average_user_state_sum_squared;
    double _average_resource_state_sum;
    double _average_resource_state_sum_squared;
    
    double _deplete_user_state_sum;
    double _deplete_user_state_sum_squared;
    
    double _neutral_user_state_sum;
    double _neutral_user_state_sum_squared;
    
    double _conserve_user_state_sum;
    double _conserve_user_state_sum_squared;
    
    double _deplete_resource_state_sum;
    double _deplete_resource_state_sum_squared;
    
    double _neutral_resource_state_sum;
    double _neutral_resource_state_sum_squared;
    
    double _conserve_resource_state_sum;
    double _conserve_resource_state_sum_squared;
    //std::map<double,std::vector< std::vector< std::pair <double,double> > > > _model_output;
};
