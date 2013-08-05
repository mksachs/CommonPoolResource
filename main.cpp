/*
 *  main.cpp
 *  CPRM
 *
 *  Created by Michael Sachs on 7/5/11.
 *  Copyright 2011 Me. All rights reserved.
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <math.h>

#include "CPRMController.h"

Parameter ParameterFromString(std::string parameter_string){
    //beta1 beta2 user_field_strength resource_field_strength resource_field_increment
    Parameter out_param;
    
    if (parameter_string == "beta1") {
        out_param = BETA1;
    }
    else if (parameter_string == "beta2") {
        out_param = BETA2;
    }
    else if (parameter_string == "user_field_strength") {
        out_param = USER_FIELD_STRENGTH;
    }
    else if (parameter_string == "resource_field_strength") {
        out_param = RESOURCE_FIELD_STRENGTH;
    }
    else if (parameter_string == "resource_field_increment") {
        out_param = RESOURCE_FIELD_INCREMENT;
    }
    
    return out_param;
}

int main (int argc, char * const argv[]) {
    
    // establish the initilization variables
    srand ( time(NULL) );
    
    CPRMController * controller;
    controller = new CPRMController;
    
    // initialization variables
    int n;
    double beta_1, beta_2, user_field_strength, resource_field_strength, resource_field_increment;
    
    // fixed parameter run variables
    int number_of_sweeps;
    bool output_average_states(false), output_states(false);
    
    // sweep variables
    Parameter param_name_1, param_name_2;
    double start_value_1, end_value_1, increment_1, start_value_2, end_value_2, increment_2;
    int number_of_single_run_samples, number_of_fixed_parameter_runs;
    
    // the output directory
    std::string output_directory;
    
    // read the parameter file
    std::string param_line;
    std::ifstream param_file ("params");
    int line_number = 0;
    bool FPR(false), OPS(false), TPS(false);
    
    if (param_file.is_open())
    {
        while ( param_file.good() )
        {
            std::getline(param_file,param_line);
            if (param_line.find_first_of("#")!=0)
            {
                if (line_number == 0) {
                    //parse the initilization string
                    int start = 0;
                    size_t found = param_line.find_first_of(" ");
                    
                    std::stringstream(param_line.substr(start, found)) >> n;
                    
                    start = found + 1;
                    found = param_line.find_first_of(" ",start);
                    
                    std::stringstream(param_line.substr(start, found)) >> beta_1;
                    
                    start = found + 1;
                    found = param_line.find_first_of(" ",start);
                    
                    std::stringstream(param_line.substr(start, found)) >> beta_2;
                    
                    start = found + 1;
                    found = param_line.find_first_of(" ",start);
                    
                    std::stringstream(param_line.substr(start, found)) >> user_field_strength;
                    
                    start = found + 1;
                    found = param_line.find_first_of(" ",start);
                    
                    std::stringstream(param_line.substr(start, found)) >> resource_field_strength;
                    
                    start = found + 1;
                    found = param_line.find_first_of(" ",start);
                    
                    std::stringstream(param_line.substr(start, found)) >> resource_field_increment;
                    
                    std::cout << "*** CPRM System ***" << std::endl;
                    std::cout << "    size:                     " << n << std::endl;
                    std::cout << "    beta 1:                   " << beta_1 << std::endl;
                    std::cout << "    beta 2:                   " << beta_2 << std::endl;
                    std::cout << "    user field strength:      " << user_field_strength << std::endl;
                    std::cout << "    resource field strength:  " << resource_field_strength << std::endl;
                    std::cout << "    resource field increment: " << resource_field_increment << std::endl;
                    
                } else if (line_number == 1) {
                    //parse the operation string
                    if (param_line.find("FPR")==0) {
                        // do a fixed parameter sweep
                        FPR = true;
                        
                        int start = 0;
                        size_t found = param_line.find_first_of(" ");
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> number_of_sweeps;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        int output_average_states_in;
                        std::stringstream(param_line.substr(start, found)) >> output_average_states_in;
                        
                        if (output_average_states_in == 1)
                            output_average_states = true;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        int output_states_in;
                        std::stringstream(param_line.substr(start, found)) >> output_states_in;
                        
                        if (output_states_in == 1)
                            output_states = true;
                        
                        std::cout << "*** Fixed Parameter Run ***" << std::endl;
                        std::cout << "    sweeps:                " << number_of_sweeps << std::endl;
                        std::cout << "    output average states: " << output_average_states << std::endl;
                        std::cout << "    output states:         " << output_states << std::endl;
                    } else if (param_line.find("OPS")==0) {
                        // do a one parameter sweep
                        OPS = true;
                        
                        int start = 0;
                        size_t found = param_line.find_first_of(" ",start);
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::string param_name_1_in = param_line.substr(start, found - start);
                        param_name_1 = ParameterFromString(param_name_1_in);
                                                
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> start_value_1;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> end_value_1;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> increment_1;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> number_of_single_run_samples;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> number_of_fixed_parameter_runs;
                        
                        std::cout << "*** One Parameter Sweep ***" << std::endl;
                        std::cout << "    parameter:            " << param_name_1_in << std::endl;
                        std::cout << "    start value:          " << start_value_1 << std::endl;
                        std::cout << "    end value:            " << end_value_1 << std::endl;
                        std::cout << "    increment:            " << increment_1 << std::endl;
                        std::cout << "    single run samples:   " << number_of_single_run_samples << std::endl;
                        std::cout << "    fixed parameter runs: " << number_of_fixed_parameter_runs << std::endl;
                    } 
                    else if (param_line.find("TPS")==0) {
                        // do a two parameter sweep
                        TPS = true;
                        
                        int start = 0;
                        size_t found = param_line.find_first_of(" ",start);
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::string param_name_1_in = param_line.substr(start, found - start);
                        param_name_1 = ParameterFromString(param_name_1_in);
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> start_value_1;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> end_value_1;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> increment_1;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::string param_name_2_in = param_line.substr(start, found - start);
                        param_name_2 = ParameterFromString(param_name_2_in);
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> start_value_2;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> end_value_2;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> increment_2;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> number_of_single_run_samples;
                        
                        start = found + 1;
                        found = param_line.find_first_of(" ",start);
                        
                        std::stringstream(param_line.substr(start, found)) >> number_of_fixed_parameter_runs;
                        
                        std::cout << "*** Two Parameter Sweep ***" << std::endl;
                        std::cout << "    parameter 1:          " << param_name_1_in << std::endl;
                        std::cout << "        start value:      " << start_value_1 << std::endl;
                        std::cout << "        end value:        " << end_value_1 << std::endl;
                        std::cout << "        increment:        " << increment_1 << std::endl;
                        std::cout << "    parameter 2:          " << param_name_2_in << std::endl;
                        std::cout << "        start value:      " << start_value_2 << std::endl;
                        std::cout << "        end value:        " << end_value_2 << std::endl;
                        std::cout << "        increment:        " << increment_2 << std::endl;
                        std::cout << "    single run samples:   " << number_of_single_run_samples << std::endl;
                        std::cout << "    fixed parameter runs: " << number_of_fixed_parameter_runs << std::endl;
                    }
                    
                }
                else if (line_number == 2) {
                    output_directory = param_line;
                    std::cout << "*** Output Directory : " << output_directory << " ***" << std::endl;
                }
                
                line_number ++;
            }
        }
        param_file.close();
    }
    else std::cout << "Unable to open file ";
    
    // initilize the controller
    controller->initialize(n, beta_1, beta_2, user_field_strength, resource_field_strength, resource_field_increment, output_directory);
    
    
    // run the operation
    if (FPR) {
        std::cout << "*** Begin Fixed Parameter Run ***" << std::endl;
        controller->run_for_n_sweeps(number_of_sweeps, false, output_average_states, output_states);
    }
    else if (OPS) {
        std::cout << "*** One Parameter Sweep ***" << std::endl;
        controller->one_parameter_sweep(param_name_1, start_value_1, end_value_1, increment_1, number_of_single_run_samples, number_of_fixed_parameter_runs);
    }
    else if (TPS) {
        std::cout << "*** Two Parameter Sweep ***" << std::endl;
        controller->two_parameter_sweep(param_name_1, start_value_1, end_value_1, increment_1, param_name_2, start_value_2, end_value_2, increment_2, number_of_single_run_samples, number_of_fixed_parameter_runs);
    }
    
    
    
    
    /*
    controller->initialize(100, 0.1, 1.0, 0.1, 1.0, 3.0);
    
    controller->one_parameter_sweep(BETA1, 0.1, 10.0, 0.05, 1, 50);
    */
    
    /* 
    /*initialize(lattice_size, beta1, beta2, user_field_strength, resource_field_strength, resource_field_increment)*/
    //controller->initialize(100, 0.1, 0.1, 1.0, 1.0, 1.0);
    
    //controller->two_parameter_sweep(BETA1, 0.1, 10.1, 1.0, BETA2, 0.1, 10.1, 1.0, 1, 10);
    
    
    //char file_name[] = "beta1_1.0_0.1_1.0_1.0";
    
    //controller->print_one_parameter_sweep_results(file_name);
    
    
    /*
    controller->initialize(100, 50.0, 1.0, 0.1, 1.0, 3.0);
    
    controller->run_for_n_sweeps(100000, true, false, false);
    */
    
    //controller->run_till_equilibrium();
    
    
    /*
    CPRMDiscreteModel * model;
    model               = new CPRMDiscreteModel;
    int sweeps          = 5000;
    
    model->initilize(50);
    model->beta1(7.5);
    model->beta2(1.0);
    
    model->user_field_strength(0.1);
    model->resource_field_increment(1.0);
    */
    //model->print_states();
    /*
    for (int i = 0; i < sweeps; i++) {
       
        StateChangeInfo state_change = model->try_new_state();
        std::cout << << model->average_user_state() << "," << model->average_resource_state();
    }
    */
    /*
    double user_state_sum = 0.0;
    double user_state_sum_squared = 0.0;
    */
    /* sweeps 
    std::cout << "0, " << fabs(model->average_user_state()) << ", " << fabs(model->average_resource_state()) <<"," << model->total_cost() << std::endl;
    //std::cout << "{" << std::endl;
    for (int sweep = 1; sweep<=sweeps; sweep++) {
        for (int i = 0; i < model->N(); i++) {
            model->try_new_state();
        }
        
        double average_user_state = fabs(model->average_user_state());
        
        user_state_sum += average_user_state;
        user_state_sum_squared += average_user_state * average_user_state;
        
        double user_state_average = user_state_sum/double(sweep);
        
        double variance = user_state_sum_squared/double(sweep) - user_state_average * user_state_average;
        std::cout << sweep << ", " << average_user_state << ", " << fabs(model->average_resource_state()) << "," << model->total_cost() << "," << variance << std::endl;
        //model->print_states();
    }
    //std::cout << "}" << std::endl;
    //model->print_states(true);
    */
}


