//
//  CPRMController.cpp
//  CPRM
//
//  Created by Michael Sachs on 9/8/11.
//  Copyright 2011 Me. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <sstream>

#include "CPRMController.h"

void CPRMController::initialize(int n , double beta_1, double beta_2, double user_field_strength, double resource_field_strength, double resource_field_increment, std::string output_directory) {
    
    _model.initilize(n);
    _model.beta1(beta_1);
    _model.beta2(beta_2);
    
    _model.user_field_strength(user_field_strength);
    _model.resource_field_strength(resource_field_strength);
    _model.resource_field_increment(resource_field_increment);
    
    _average_user_state_sum = 0.0;
    _average_user_state_sum_squared = 0.0;
    
    _average_resource_state_sum = 0.0;
    _average_resource_state_sum_squared = 0.0;
    
    _conserve_user_state_sum = 0.0;
    _conserve_user_state_sum_squared = 0.0;
    
    _neutral_user_state_sum = 0.0;
    _neutral_user_state_sum_squared = 0.0;
    
    _deplete_user_state_sum = 0.0;
    _deplete_user_state_sum_squared = 0.0;
    
    _decreasing_variance_sweep_threshold = 4000;
    _max_sweep_threshold = 300000;
    _variance_fluctuation_threshold = 0.00001;
    
    _sweeps_between_samples = 1000;
    
    _output_file_prefix = output_directory;
    //_output_file_prefix = "";
}

void CPRMController::run_for_n_sweeps(int sweeps, bool ave_states_to_console, bool ave_states_to_file, bool states_to_file) {
    double sweep_average_user_state, sweep_average_resource_state, sweep_parity;
    double sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction;
    double sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction;
    char ave_states_file_name[100], states_file_name[100];
    std::string ave_states_file_name_str, states_file_name_str;
    
    sprintf(states_file_name, "%sCPRM_FPR_states_%i_%i_%.2f_%.2f_%.2f_%.2f_%.2f.dat", _output_file_prefix.c_str(), sweeps, _model.n(), _model.beta1(), _model.beta2(), _model.user_field_strength(), _model.resource_field_strength(), _model.resource_field_increment());
    states_file_name_str = states_file_name;
    std::ofstream states_file;
    
    sprintf(ave_states_file_name, "%sCPRM_FPR_aveStates_%i_%i_%.2f_%.2f_%.2f_%.2f_%.2f.dat", _output_file_prefix.c_str(), sweeps, _model.n(), _model.beta1(), _model.beta2(), _model.user_field_strength(), _model.resource_field_strength(), _model.resource_field_increment());
    ave_states_file_name_str = ave_states_file_name;
    std::ofstream ave_states_file;
    
    if (ave_states_to_file || ave_states_to_console) {
        
        sweep_parity = _model.average_states(sweep_average_user_state, sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, sweep_average_resource_state, sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction);
        
        update_state_sums(sweep_parity, sweep_average_user_state, sweep_average_resource_state, sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction);
    }
    
    if (states_to_file) {
        states_file.open(states_file_name_str.c_str());
        
        states_file << _model.n() << std::endl;
        _model.print_states_to_file(states_file);
    }
    
    if (ave_states_to_file) {
        ave_states_file.open(ave_states_file_name_str.c_str());
        ave_states_file << "sweep ave_user_state user_variance user_frac_conserve user_frac_conserve_variance user_frac_neutral user_frac_neutral_variance user_frac_deplete user_frac_deplete_variance ave_resource_state resource_variance resource_frac_conserve resource_frac_conserve_variance resource_frac_neutral resource_frac_neutral_variance resource_frac_deplete resource_frac_deplete_variance parity total_cost" << std::endl;
        ave_states_file << 0 << " " 
                        << fabs(sweep_average_user_state) << " " 
                        << 0 << " " 
                        << sweep_user_conserve_fraction << " " 
                        << 0 << " " 
                        << sweep_user_neutral_fraction << " " 
                        << 0 << " "
                        << sweep_user_deplete_fraction << " " 
                        << 0 << " "
                        << fabs(sweep_average_resource_state) << " "
                        << 0 << " "
                        << sweep_resource_conserve_fraction << " " 
                        << 0 << " " 
                        << sweep_resource_neutral_fraction << " " 
                        << 0 << " "
                        << sweep_resource_deplete_fraction << " " 
                        << 0 << " "
                        << sweep_parity << " " 
                        << _model.total_cost() 
                        << std::endl;
        
    }
    
    if (ave_states_to_console) {
        std::cout << 0 << " " 
                        << fabs(sweep_average_user_state) << " " 
                        << 0 << " " 
                        << sweep_user_conserve_fraction << " " 
                        << 0 << " " 
                        << sweep_user_neutral_fraction << " " 
                        << 0 << " "
                        << sweep_user_deplete_fraction << " " 
                        << 0 << " "
                        << fabs(sweep_average_resource_state) << " "
                        << 0 << " "
                        << sweep_resource_conserve_fraction << " " 
                        << 0 << " " 
                        << sweep_resource_neutral_fraction << " " 
                        << 0 << " "
                        << sweep_resource_deplete_fraction << " " 
                        << 0 << " "
                        << sweep_parity << " " 
                        << _model.total_cost() 
                        << std::endl;
    }
    
    /* sweeps*/ 
    for (int sweep = 1; sweep<=sweeps; sweep++) {
        for (int i = 0; i < _model.N(); i++) {
            _model.try_new_state();
        }
        
        if (ave_states_to_file || ave_states_to_console) {
            sweep_parity = _model.average_states(sweep_average_user_state, sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, sweep_average_resource_state, sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction);
            
            update_state_sums(sweep_parity, sweep_average_user_state, sweep_average_resource_state, sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction);
        }
        
        if (ave_states_to_console) {
           std::cout    << sweep << " " 
                        << fabs(sweep_average_user_state) << " " 
                        << user_variance(sweep) << " " 
                            << sweep_user_conserve_fraction << " " 
                            << user_conserve_variance(sweep) << " " 
                            << sweep_user_neutral_fraction << " " 
                            << user_neutral_variance(sweep) << " "
                            << sweep_user_deplete_fraction << " " 
                            << user_deplete_variance(sweep) << " "
                        << fabs(sweep_average_resource_state) << " "
                        << resource_variance(sweep) << " " 
                            << sweep_resource_conserve_fraction << " " 
                            << resource_conserve_variance(sweep) << " " 
                            << sweep_resource_neutral_fraction << " " 
                            << resource_neutral_variance(sweep) << " "
                            << sweep_resource_deplete_fraction << " " 
                            << resource_deplete_variance(sweep) << " "
                        << sweep_parity << " "
                        << _model.total_cost() 
                        << std::endl;
        }
        
        if (ave_states_to_file) {
            ave_states_file << sweep << " " 
                            << fabs(sweep_average_user_state) << " " 
                            << user_variance(sweep) << " " 
                                << sweep_user_conserve_fraction << " " 
                                << user_conserve_variance(sweep) << " " 
                                << sweep_user_neutral_fraction << " " 
                                << user_neutral_variance(sweep) << " "
                                << sweep_user_deplete_fraction << " " 
                                << user_deplete_variance(sweep) << " "
                            << fabs(sweep_average_resource_state) << " "
                            << resource_variance(sweep) << " " 
                                << sweep_resource_conserve_fraction << " " 
                                << resource_conserve_variance(sweep) << " " 
                                << sweep_resource_neutral_fraction << " " 
                                << resource_neutral_variance(sweep) << " "
                                << sweep_resource_deplete_fraction << " " 
                                << resource_deplete_variance(sweep) << " "
                            << sweep_parity << " "
                            << _model.total_cost() 
                            << std::endl;
        }
        
        if (states_to_file) {
           _model.print_states_to_file(states_file);
        }
        //model->print_states();
    }
    
    if (states_to_file) {
        states_file.close();
    }
    if (ave_states_to_file) {
        ave_states_file.close();
    }

}

/*
void CPRMController::set_current_parameters(Parameter sweep_parameter, double sweep_parameter_start, double sweep_parameter_end) {
    Parameter all_params[5] = {BETA1,BETA2,USER_FIELD_STRENGTH,RESOURCE_FIELD_STRENGTH,RESOURCE_FIELD_INCREMENT};
    _current_sweep_parameter_1 = sweep_parameter;
    _current_sweep_parameter_start_1 = sweep_parameter_start;
    _current_sweep_parameter_end_1 = sweep_parameter_end;
    
    for (int i = 0; i < 5; i++) {
        if (sweep_parameter != all_params[i]) {
            <#statements#>
        }
    }
    
    _current_fixed_parameter_value_1
}
*/

/*
void CPRMController::run_for_n_sweeps_no_output(int sweeps) {
    for (int sweep = 1; sweep<=sweeps; sweep++) {
        for (int i = 0; i < _model.N(); i++) {
            _model.try_new_state();
        }
    }
}
*/


void CPRMController::two_parameter_sweep(Parameter parameter1, double start_value1, double end_value1, double increment1, Parameter parameter2, double start_value2, double end_value2, double increment2, int number_of_single_run_samples, int number_of_fixed_parameter_runs) {
    
    char file_name[100];
    std::string file_name_str;
    std::string parameter1_name = get_parameter_name(parameter1);
    std::string parameter2_name = get_parameter_name(parameter2);
    
    sprintf(file_name, "%sCPRM_TPS_%s-%.2f_%s-%.2f__%i_%.2f_%.2f_%.2f_%.2f_%.2f.dat", _output_file_prefix.c_str(), parameter1_name.c_str(), end_value1, parameter2_name.c_str(), end_value2, _model.n(), _model.beta1(), _model.beta2(), _model.user_field_strength(), _model.resource_field_strength(), _model.resource_field_increment());
    
    file_name_str = file_name;
    
    std::ofstream file (file_name_str.c_str());
    
    double parity;
    double sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, average_user_state;
    double sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction, average_resource_state;
    
    file << "beta1 beta2 user_field_strength resource_field_strength resource_field_increment run ";
    
    for (int p = 0; p < number_of_single_run_samples; p++) {
        file << "sample" << p+1 << "_user sample" << p+1 << "_user_conserve_fraction sample" << p+1 << "_user_neutral_fraction sample" << p+1 << "_user_deplete_fraction sample" << p+1 <<"_resource sample" << p+1 << "_resource_conserve_fraction sample" << p+1 << "_resource_neutral_fraction sample" << p+1 << "_resource_deplete_fraction sample" << p+1 << "_parity ";
    }
    
    file << std::endl;
    
    double curr_value1 = start_value1;
    
    //outer loop
    for (int i = 0; curr_value1 <= end_value1; i++) {
        
        //std::cout << curr_value1 << " " << end_value1 << std::endl << std::flush;
        
        _model.set_parameter(curr_value1, parameter1);
        
        //std::cout << get_parameter_name(parameter1) << ", " << _model.get_parameter(parameter1) << ", " << std::flush;
        
        double curr_value2 = start_value2;
        //inner loop
        for (int p = 0; curr_value2 <= end_value2; p++) {
            
            _model.set_parameter(curr_value2, parameter2);
            
            std::cout << get_parameter_name(parameter1) << ", " << _model.get_parameter(parameter1) << ", " << get_parameter_name(parameter2) << ", " << _model.get_parameter(parameter2) << ", " << std::flush;
            
            int fixed_parameter_run = 0;
            while (fixed_parameter_run < number_of_fixed_parameter_runs) {
                std::cout << fixed_parameter_run << ": " << std::flush;
                
                int equib_sweep;
                if (run_till_equilibrium(equib_sweep)) {
                   
                    std::cout << equib_sweep << " :" << std::flush;
                    
                    parity = _model.average_states(average_user_state, sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, average_resource_state, sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction);
                    
                    file << _model.beta1() << " " << _model.beta2() << " " << _model.user_field_strength() << " " << _model.resource_field_strength() << " " << _model.resource_field_increment() << " " << fixed_parameter_run << " " << average_user_state << " " << sweep_user_conserve_fraction << " " << sweep_user_neutral_fraction << " " << sweep_user_deplete_fraction << " " << average_resource_state << " " << sweep_resource_conserve_fraction << " " << sweep_resource_neutral_fraction << " " << sweep_resource_deplete_fraction << " " << parity << " ";
                    
                    int single_run_sample = 1;
                    
                    while (single_run_sample < number_of_single_run_samples) {
                        std::cout << "." << std::flush;
                        run_for_n_sweeps(_sweeps_between_samples);
                        
                        parity = _model.average_states(average_user_state, sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, average_resource_state, sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction);
                        
                        file << average_user_state << " " << sweep_user_conserve_fraction << " " << sweep_user_neutral_fraction << " " << sweep_user_deplete_fraction << " " << average_resource_state << " " << sweep_resource_conserve_fraction << " " << sweep_resource_neutral_fraction << " " << sweep_resource_deplete_fraction << " " << parity << " ";
                        single_run_sample ++;
                    }
                    
                    file << std::endl;
                    fixed_parameter_run ++;
                }
                _model.randomize();
            }
            std::cout << std::endl;
            
            curr_value2 += increment2;
        }
        
        curr_value1 += increment1;
    }
    
    file.close();
}

void CPRMController::one_parameter_sweep(Parameter parameter, double start_value, double end_value, double increment, int number_of_single_run_samples, int number_of_fixed_parameter_runs) {
    
    char file_name[100];
    std::string file_name_str;
    std::string parameter_name = get_parameter_name(parameter);
    
    sprintf(file_name, "%sCPRM_OPS_%s-%.2f__%i_%.2f_%.2f_%.2f_%.2f_%.2f.dat", _output_file_prefix.c_str(), parameter_name.c_str(), end_value, _model.n(), _model.beta1(), _model.beta2(), _model.user_field_strength(), _model.resource_field_strength(), _model.resource_field_increment());
    
    file_name_str = file_name;
    
    std::ofstream file (file_name_str.c_str());
    
    double parity;
    double sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, average_user_state;
    double sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction, average_resource_state;
    
    //std::vector< std::vector<std::pair <double,double> > > _sweep_output;
    double curr_value = start_value;
    
    file << "beta1 beta2 user_field_strength resource_field_strength resource_field_increment run ";
    
    for (int p = 0; p < number_of_single_run_samples; p++) {
        file << "sample" << p+1 << "_user sample" << p+1 << "_user_conserve_fraction sample" << p+1 << "_user_neutral_fraction sample" << p+1 << "_user_deplete_fraction sample" << p+1 <<"_resource sample" << p+1 << "_resource_conserve_fraction sample" << p+1 << "_resource_neutral_fraction sample" << p+1 << "_resource_deplete_fraction sample" << p+1 << "_parity ";
    }
    
    file << std::endl;

    for (int i = 0; curr_value <= end_value; i++) {
        
        //_sweep_output.clear();
        
        curr_value += double(i) * increment;
        
        _model.set_parameter(curr_value, parameter);
        
        std::cout << get_parameter_name(parameter) << ", " << _model.get_parameter(parameter) << ", " << std::flush;
        
        //file << _model.beta1() << " " << _model.beta2() << " " << _model.user_field_strength() << " " << _model.resource_field_strength() << " " << _model.resource_field_increment() << " ";
        
        int fixed_parameter_run = 0;
        while (fixed_parameter_run < number_of_fixed_parameter_runs) {
            std::cout << fixed_parameter_run << ": " << std::flush;
            
            int equib_sweep;
            
            if (run_till_equilibrium(equib_sweep)) {
                
                std::cout << equib_sweep << " :" << std::flush;
                //if(run_till_equilibrium()) {
                    //std::cout << _model.average_user_state() << ", " << _model.average_resource_state() << ", " << std::flush;
                    
                    //std::vector<std::pair<double, double> > row;
                    ///std::pair<double, double> values;
                    
                    //row.clear();
                    
                    //values = std::make_pair (_model.average_user_state(),_model.average_resource_state());
                    
                    //row.push_back(values);
                    
                parity = _model.average_states(average_user_state, sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, average_resource_state, sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction);
                
                file << _model.beta1() << " " << _model.beta2() << " " << _model.user_field_strength() << " " << _model.resource_field_strength() << " " << _model.resource_field_increment() << " " << fixed_parameter_run << " " << average_user_state << " " << sweep_user_conserve_fraction << " " << sweep_user_neutral_fraction << " " << sweep_user_deplete_fraction << " " << average_resource_state << " " << sweep_resource_conserve_fraction << " " << sweep_resource_neutral_fraction << " " << sweep_resource_deplete_fraction << " " << parity << " ";
                
                int single_run_sample = 1;
                
                while (single_run_sample < number_of_single_run_samples) {
                    std::cout << "." << std::flush;
                    run_for_n_sweeps(_sweeps_between_samples);
                    
                    parity = _model.average_states(average_user_state, sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, average_resource_state, sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction);
                    
                    file << average_user_state << " " << sweep_user_conserve_fraction << " " << sweep_user_neutral_fraction << " " << sweep_user_deplete_fraction << " " << average_resource_state << " " << sweep_resource_conserve_fraction << " " << sweep_resource_neutral_fraction << " " << sweep_resource_deplete_fraction << " " << parity << " ";
                    //values = std::make_pair (_model.average_user_state(),_model.average_resource_state());
                    
                    //row.push_back(values);
                    
                    single_run_sample ++;
                }
                
                file << std::endl;
                //_sweep_output.push_back(row);
                fixed_parameter_run ++;
                //}
             }
            _model.randomize();
        }
        
        std::cout << std::endl;
        
        
        //_model_output[curr_value] = _sweep_output;
        
        //std::cout << parameter << ", " << _model.get_parameter(parameter) << ", " << _model.average_user_state() << ", " << _model.average_resource_state() << std::endl;
        
        //_model.randomize();
    }
    std::cout << std::endl;
    
    file.close();
}

/*
void CPRMController::run_for_one_correlation_time() {
    
}
*/

/*
void CPRMController::print_one_parameter_sweep_results(char * file_name_addendum) {
    std::map<double,std::vector< std::vector< std::pair <double,double> > > >::iterator map_it;
    std::vector< std::vector< std::pair <double,double> > >::iterator vector_outer_it;
    std::vector< std::pair <double,double> >::iterator vector_inner_it;
    char file_name[100];
    int run = 1;
    
    sprintf(file_name, "%sCPRM_oneParamSweep_%s.dat", _output_file_prefix.c_str(), file_name_addendum);
    std::ofstream file (file_name);
    
    for (map_it =_model_output.begin(); map_it != _model_output.end(); map_it++) {
        std::cout << (*map_it).first << ",";
        
        _average_user_state_sum = 0.0;
        _average_user_state_sum_squared = 0.0;
        _average_resource_state_sum = 0.0;
        _average_resource_state_sum_squared = 0.0;
        
        for (vector_outer_it = (*map_it).second.begin(); vector_outer_it != (*map_it).second.end(); vector_outer_it++) {
            file << (*map_it).first << " " << run << " ";
            
            double single_run_average_user_state_sum = 0.0;
           // double single_run_average_user_state_sum_squared = 0.0;
            double single_run_average_resource_state_sum = 0.0;
           // double single_run_average_resource_state_sum_squared = 0.0;
            
            int single_run_samples = 0;
            for (vector_inner_it = (*vector_outer_it).begin(); vector_inner_it != (*vector_outer_it).end(); vector_inner_it++) {
                
                single_run_average_user_state_sum += (*vector_inner_it).first;
                //single_run_average_user_state_sum_squared += (*vector_inner_it).first * (*vector_inner_it).first;
                
                single_run_average_resource_state_sum += (*vector_inner_it).second;
                //single_run_average_resource_state_sum_squared += (*vector_inner_it).second * (*vector_inner_it).second;
                
                file << (*vector_inner_it).first << " " << (*vector_inner_it).second << " ";
                
                single_run_samples ++;
                
                //_average_user_state_sum += fabs((*vector_inner_it).first);
                //_average_user_state_sum_squared += (*vector_inner_it).first * (*vector_inner_it).first;
                //_average_resource_state_sum += fabs((*vector_inner_it).second);
                //_average_resource_state_sum_squared += (*vector_inner_it).second * (*vector_inner_it).second;
            }
            
            //double average_user_state = _average_user_state_sum/double(sweep);
            //return _average_user_state_sum_squared/double(sweep) - average_user_state * average_user_state;
            
            update_state_sums( fabs(single_run_average_user_state_sum/double(single_run_samples)), fabs(single_run_average_resource_state_sum/double(single_run_samples)));
            
            file << std::endl;
        }
        
        run ++;
        
        
        //for (int i = 0; i < (*map_it).second.size(); i++) {
        //    for (int j = 0; j < (*outer_it).second[i].size(); j++) {
        //        std::cout << (*outer_it).second[i][j].first << "," << (*outer_it).second[i][j].second;
        //    }
        //    
       // }
        
        std::cout << _average_user_state_sum/(*map_it).second.size() << ", " << pow((user_variance((*map_it).second.size())),0.5) << ", " << _average_resource_state_sum/(*map_it).second.size() << "," << pow((resource_variance((*map_it).second.size())),0.5) << std::endl;
        
    }
        //cout << (*it).first << " => " << (*it).second << endl;
}
*/

bool CPRMController::run_till_equilibrium(int &equilib_sweep) {
    double sweep_average_user_state, sweep_average_resource_state, sweep_parity;
    double sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction;
    double this_sweep_conserve_variance, previous_sweep_conserve_variance;
    double this_sweep_neutral_variance, previous_sweep_neutral_variance;
    double this_sweep_deplete_variance, previous_sweep_deplete_variance;
    double sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction;
    int curr_sweep;
    //bool break_loop = false;
    
    int decreasing_variance_sweeps = 0;
    curr_sweep = 1;
    
    this_sweep_conserve_variance = 0.0;
    this_sweep_neutral_variance = 0.0;
    this_sweep_deplete_variance = 0.0;
    
    _parity_sum = 0.0;
    
    _average_user_state_sum = 0.0;
    _average_user_state_sum_squared = 0.0;
    
    _average_resource_state_sum = 0.0;
    _average_resource_state_sum_squared = 0.0;
    
    _conserve_user_state_sum = 0.0;
    _conserve_user_state_sum_squared = 0.0;
    
    _neutral_user_state_sum = 0.0;
    _neutral_user_state_sum_squared = 0.0;
    
    _deplete_user_state_sum = 0.0;
    _deplete_user_state_sum_squared = 0.0;

    
    while ((decreasing_variance_sweeps < _decreasing_variance_sweep_threshold) && (curr_sweep < _max_sweep_threshold)) {
        previous_sweep_conserve_variance = this_sweep_conserve_variance;
        previous_sweep_neutral_variance = this_sweep_neutral_variance;
        previous_sweep_deplete_variance = this_sweep_deplete_variance;
        
        for (int i = 0; i < _model.N(); i++) {
            _model.try_new_state();
        }
        
        
        
        sweep_parity = _model.average_states(sweep_average_user_state, sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, sweep_average_resource_state, sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction);

        update_state_sums(sweep_parity, sweep_average_user_state, sweep_average_resource_state, sweep_user_conserve_fraction, sweep_user_neutral_fraction, sweep_user_deplete_fraction, sweep_resource_conserve_fraction, sweep_resource_neutral_fraction, sweep_resource_deplete_fraction);
        
        this_sweep_conserve_variance = user_conserve_variance(curr_sweep);
        this_sweep_neutral_variance = user_neutral_variance(curr_sweep);
        this_sweep_deplete_variance = user_deplete_variance(curr_sweep);
        
        if (curr_sweep % 500 == 0) {
            std::cout   << curr_sweep <<  " " << std::flush;
        }
        
        curr_sweep += 1;
        
        if (    (this_sweep_conserve_variance < previous_sweep_conserve_variance || fabs(this_sweep_conserve_variance - previous_sweep_conserve_variance) < _variance_fluctuation_threshold)
                &&
                (this_sweep_neutral_variance < previous_sweep_neutral_variance || fabs(this_sweep_neutral_variance - previous_sweep_neutral_variance) < _variance_fluctuation_threshold)
                &&
                (this_sweep_deplete_variance < previous_sweep_deplete_variance || fabs(this_sweep_deplete_variance - previous_sweep_deplete_variance) < _variance_fluctuation_threshold)
                ) {
            decreasing_variance_sweeps += 1;
        } else {
            decreasing_variance_sweeps = 0;
        }
    }
    
    equilib_sweep = curr_sweep;
    
    
    if (curr_sweep >= _max_sweep_threshold ) {
        return false;
    } else {
        return true;
    }
    
    
    //std::cout << "equilibrium at sweep " << sweep - 1 << ", " << sweep_average_user_state << ", " << sweep_average_resource_state << "," << _model.total_cost() << "," << user_variance(sweep -1) << ", " << resource_variance(sweep -1 ) << std::endl;
}

void CPRMController::update_state_sums(double sweep_parity, double sweep_average_user_state, double sweep_average_resource_state, double sweep_user_conserve_fraction, double sweep_user_neutral_fraction, double sweep_user_deplete_fraction, double sweep_resource_conserve_fraction, double sweep_resource_neutral_fraction, double sweep_resource_deplete_fraction) {
    
    _parity_sum                             += sweep_parity;
    
    _average_user_state_sum                 += sweep_average_user_state;
    _average_user_state_sum_squared         += sweep_average_user_state * sweep_average_user_state;
    
    _average_resource_state_sum             += sweep_average_resource_state;
    _average_resource_state_sum_squared     += sweep_average_resource_state * sweep_average_resource_state;
    
    _deplete_user_state_sum                 += sweep_user_deplete_fraction;
    _deplete_user_state_sum_squared         += sweep_user_deplete_fraction * sweep_user_deplete_fraction;
    
    _neutral_user_state_sum                 += sweep_user_neutral_fraction;
    _neutral_user_state_sum_squared         += sweep_user_neutral_fraction * sweep_user_neutral_fraction;
    
    _conserve_user_state_sum                += sweep_user_conserve_fraction;
    _conserve_user_state_sum_squared        += sweep_user_conserve_fraction * sweep_user_conserve_fraction;
    
    _deplete_resource_state_sum             += sweep_resource_deplete_fraction;
    _deplete_resource_state_sum_squared     += sweep_resource_deplete_fraction * sweep_resource_deplete_fraction;
    
    _neutral_resource_state_sum             += sweep_resource_neutral_fraction;
    _neutral_resource_state_sum_squared     += sweep_resource_neutral_fraction * sweep_resource_neutral_fraction;
    
    _conserve_resource_state_sum            += sweep_resource_conserve_fraction;
    _conserve_resource_state_sum_squared    += sweep_resource_conserve_fraction * sweep_resource_conserve_fraction;
}

double CPRMController::user_variance(int sweep) {
    double average_user_state = _average_user_state_sum/double(sweep);
    return _average_user_state_sum_squared/double(sweep) - average_user_state * average_user_state;
}

double CPRMController::user_deplete_variance(int sweep) {
    double average_deplete_state = _deplete_user_state_sum/double(sweep);
    return _deplete_user_state_sum_squared/double(sweep) - average_deplete_state * average_deplete_state;
}

double CPRMController::user_neutral_variance(int sweep) {
    double average_neutral_state = _neutral_user_state_sum/double(sweep);
    return _neutral_user_state_sum_squared/double(sweep) - average_neutral_state * average_neutral_state;
}

double CPRMController::user_conserve_variance(int sweep) {
    double average_conserve_state = _conserve_user_state_sum/double(sweep);
    return _conserve_user_state_sum_squared/double(sweep) - average_conserve_state * average_conserve_state;
}

double CPRMController::resource_variance(int sweep) {
    double average_resource_state = _average_resource_state_sum/double(sweep);
    return _average_resource_state_sum_squared/double(sweep) - average_resource_state * average_resource_state;
}
double CPRMController::resource_deplete_variance(int sweep) {
    double average_deplete_state = _deplete_resource_state_sum/double(sweep);
    return _deplete_resource_state_sum_squared/double(sweep) - average_deplete_state * average_deplete_state;
}

double CPRMController::resource_neutral_variance(int sweep) {
    double average_neutral_state = _neutral_resource_state_sum/double(sweep);
    return _neutral_resource_state_sum_squared/double(sweep) - average_neutral_state * average_neutral_state;
}

double CPRMController::resource_conserve_variance(int sweep) {
    double average_conserve_state = _conserve_resource_state_sum/double(sweep);
    return _conserve_resource_state_sum_squared/double(sweep) - average_conserve_state * average_conserve_state;
}

std::string CPRMController::get_parameter_name(Parameter parameter) {
    switch (parameter) {
        case BETA1:
            return "beta1";
            break;
        case BETA2:
            return "beta2";
            break;
        case USER_FIELD_STRENGTH:
            return "UFS";
            break;
        case RESOURCE_FIELD_STRENGTH:
            return "RFS";
            break;
        case RESOURCE_FIELD_INCREMENT:
            return "RFI";
            break;
        default:
            return "";
            break;
    }
}