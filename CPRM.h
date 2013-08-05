/*
 *  CPRM.h
 *  CPRM
 *
 *  Created by Michael Sachs on 7/5/11.
 *  Copyright 2011 Me. All rights reserved.
 *
 */

#include <map>
#include <math.h>
#include <fstream>
//typedef double DiscreteResourceState;
//typedef	unsigned int DiscreteUserState;

//typedef	double ContinuousUserState;
//typedef double ContinuousResourceState;

typedef unsigned int LatticeIndex;

enum ModelType {
	DISCRETE,
	CONTINUOUS,
};

enum DiscreteState {
    DEPLETE,
    NEUTRAL,
    CONSERVE
};

struct HInfo {
    double H_user;
    double H_resource;
};

struct DiscreteResourceState {
    DiscreteState state;
    double strength;
};

typedef DiscreteState DiscreteUserState;

enum Parameter {
	BETA1,
	BETA2,
    USER_FIELD_STRENGTH,
    RESOURCE_FIELD_STRENGTH,
    RESOURCE_FIELD_INCREMENT,
};

struct StateChangeInfo {
    //ContinuousResourceState C_resource_state;
    //ContinuousUserState     C_user_state;
    DiscreteResourceState   D_resource_state;
    DiscreteUserState       D_user_state;
    //ModelType               model_type;
    LatticeIndex            index;
    bool                    state_changed;
};

/*
class CPRMStateChange {
public:
    CPRMStateChange(ModelType in_parent_model_type) {
        _state_changed = false;
        _parent_type = in_parent_model_type;
    };
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Getters
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    bool state_changed() {return _state_changed;};
    int index() {return _index;};
    int user_state() {
        if (_parent_type == DISCRETE) {
            return _d_user_state;
        } else {
            return _c_user_state;
        }
        
    };
    int resource_state() {return _resource_state;};
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Setters
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    void index(int in_index) {_index = in_index;};
    void user_state(DiscreteUserState in_user_state) {_d_user_state = in_user_state;};
    void user_state(ContinuousUserState in_user_state) {_c_user_state = in_user_state;};
    void resource_state(ResourceState in_resource_state) {_resource_state = in_resource_state;};
    void set_values(int in_index, DiscreteUserState in_user_state, ResourceState in_resource_state) {
        _index          = in_index;
        _d_user_state   = in_user_state;
        _resource_state = in_resource_state;
    }
    void set_values(int in_index, ContinuousUserState in_user_state, ResourceState in_resource_state) {
        _index          = in_index;
        _c_user_state   = in_user_state;
        _resource_state = in_resource_state;
    }
private:
    int                 _index;
    DiscreteUserState   _d_user_state;
    ContinuousUserState _c_user_state;
    ResourceState       _resource_state;
    bool                _state_changed;
    ModelType           _parent_type;
};
*/

class CPRMDiscreteModel {
public:
    static ModelType my_type;
    void initilize(int in_n);
    StateChangeInfo try_new_state();
    void print_states();
    void print_states_to_file(std::ofstream & states_file);
    double average_states(double &average_user_state, double &user_conserve_fraction, double &user_neutral_fraction, double &user_deplete_fraction, double &average_resource_state, double &resource_conserve_fraction, double &resource_neutral_fraction, double &resource_deplete_fraction);
    double average_user_state(double &conserve_fraction, double &neutral_fraction, double &deplete_fraction) {
        //return double(_user_state_sum) / double(_N);
        double num_deplete = 0.0;
        double num_neutral = 0.0;
        double num_conserve = 0.0;
        double sum = 0.0;
        
        for (int i = 0; i < _N; i++) {
            switch (_users[i]) {
                case DEPLETE:
                    sum -= 1;
                    num_deplete += 1;
                    break;
                case CONSERVE:
                    sum += 1;
                    num_conserve += 1;
                    break;
                case NEUTRAL:
                    num_neutral += 1;
                    break;
                default:
                    break;
            }
        }
        
        conserve_fraction = num_conserve/double(_N);
        neutral_fraction = num_neutral/double(_N);
        deplete_fraction = num_deplete/double(_N);
        
        return sum / _N;
    }
    double average_resource_state(double &conserve_fraction, double &neutral_fraction, double &deplete_fraction) {
        //return double(_resource_state_sum) / double(_N);
        double num_deplete = 0.0;
        double num_neutral = 0.0;
        double num_conserve = 0.0;
        double sum = 0.0;
        
        for (int i = 0; i < _N; i++) {
            switch (_resources[i].state) {
                case DEPLETE:
                    sum -= _resources[i].strength;
                    num_deplete += 1;
                    break;
                case CONSERVE:
                    sum += _resources[i].strength;
                    num_conserve += 1;
                    break;
                case NEUTRAL:
                    num_neutral += 1;
                    break;
                default:
                    break;
            }
        }
        
        conserve_fraction = num_conserve/double(_N);
        neutral_fraction = num_neutral/double(_N);
        deplete_fraction = num_deplete/double(_N);
        
        return sum / _N;

        /*
        double sum = 0.0;
        for (int i = 0; i < _N; i++) {
            switch (_resources[i].state) {
                case DEPLETE:
                    sum -= _resources[i].strength;
                    break;
                case CONSERVE:
                    sum += _resources[i].strength;
                    break;
                default:
                    break;
            }
        }
        return sum / _N;
        */
    }
    double total_cost() {
        double sum = 0.0;
        for (int i = 0; i < _N; i++) {
            HInfo curr_H = H(i,_users[i],_resources[i]);
            sum += curr_H.H_user + curr_H.H_resource;
        }
        return sum;
    }
    void randomize();
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Getters
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    double beta1() {return _beta1;};
    double beta2() {return _beta2;};
    int N() {return _N;};
    int n() {return _n;};
    double user_field_strength() {return _user_field_strength;};
    double resource_field_strength() {return _resource_field_strength;};
    double resource_field_increment() {return _resource_field_increment;};
    double get_parameter(Parameter parameter);
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Setters
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    void beta1(double in_beta1) {_beta1 = in_beta1;};
    void beta2(double in_beta2) {_beta2 = in_beta2;};
    void user_field_strength(double in_user_field_strength) {_user_field_strength = in_user_field_strength;};
    void resource_field_strength(double in_resource_field_strength) {_resource_field_strength = in_resource_field_strength;};
    void resource_field_increment(double in_resource_field_increment) {_resource_field_increment = in_resource_field_increment;};
    void set_parameter(double parameter_value, Parameter parameter);
private:
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Properties
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // arrays that will hold the states of the system. */
    //array for the user states
    DiscreteUserState * _users;
    //array for the resources
    DiscreteResourceState * _resources;
    // temperatures.
    // for the users
    double _beta1;
    // for the resources
    double _beta2;
    // properties that apply to both lattices
    // size of one side of the square lattice
    int _n;
    // number of total elements in the lattice
    int _N;
    
    double _user_state_sum;
    double _resource_state_sum;
    
    double _user_field_strength;
    double _resource_field_strength;
    
    double _resource_field_increment;
    
    
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Methods
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    HInfo cost_function_change(LatticeIndex try_index, DiscreteUserState user, DiscreteResourceState resource, DiscreteUserState new_user, DiscreteResourceState new_resource);
    int mod(int a, int b);
    double U(DiscreteUserState user, DiscreteResourceState resource);
    double R(DiscreteUserState user, DiscreteResourceState resource);
    void change_state(LatticeIndex index, DiscreteUserState new_user, DiscreteResourceState new_resource);
    HInfo H(LatticeIndex index, DiscreteUserState user, DiscreteResourceState resource);
};

/*
class CPRMContinuousModel {
public:
    static ModelType my_type;
    void initilize(int in_n);
    CPRMStateChange try_new_state();
    void print_states(bool spin = false);
    double average_user_state() {
        //return _user_state_sum / double(_N);
        double sum = 0.0;
        for (int i = 0; i < _N; i++) {
            sum += _users[i];
        }
        
        //return cos(sum / _N);
        return sum / _N;
    }
    double average_resource_state() {
        //return _resource_state_sum / double(_N);
        double sum = 0.0;
        for (int i = 0; i < _N; i++) {
            sum += _resources[i];
        }
        
        //return cos(sum / _N);
        return sum / _N;
        
    }
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Getters
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    double beta1() {return _beta1;};
    double beta2() {return _beta2;};
    int N() {return _N;};
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Setters
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    void beta1(double in_beta1) {_beta1 = in_beta1;};
    void beta2(double in_beta2) {_beta2 = in_beta2;};
private:
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Properties
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // arrays that will hold the states of the system.
    // array for the user states
    ContinuousUserState * _users;
    //array for the resources
    ResourceState * _resources;
    // temperatures.
    // for the users
    double _beta1;
    // for the resources
    double _beta2;
    // properties that apply to both lattices
    // size of one side of the square lattice
    int _n;
    // number of total elements in the lattice
    int _N;
    
    int _user_state_sum;
    double _resource_state_sum;
    
    
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Methods
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    std::map<std::string,double> cost_function_change(int try_index, ContinuousUserState user, ResourceState resource, ContinuousUserState new_user, ResourceState new_resource);
    int mod(int a, int b);
};
 */



