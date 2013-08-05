/*
 *  CPRM.cpp
 *  CPRM
 *
 *  Created by Michael Sachs on 7/5/11.
 *  Copyright 2011 Me. All rights reserved.
 *
 */

#include "CPRM.h"

#include <time.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <math.h>

#define _USE_MATH_DEFINES

ModelType CPRMDiscreteModel::my_type    = DISCRETE;

/*
ModelType CPRMContinuousModel::my_type  = CONTINUOUS;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Continuous case
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void CPRMContinuousModel::initilize(int in_n) {
    _n = in_n;
    _N = _n*_n;
    
    _users = new (std::nothrow) ContinuousUserState[_N];
    _resources = new (std::nothrow) ResourceState[_N];
    
    _user_state_sum = 0.0;
    _resource_state_sum = 0.0;
    
    if (_users == 0 || _resources == 0)
        std::cout << "Error: memory could not be allocated" << std::endl;
    else
    {
        for (int i = 0; i<_N; i++) {
            _users[i]       = 2.0*M_PI*double(rand())/double(RAND_MAX);
            //_users[i]       = M_PI*double(rand())/double(RAND_MAX);
            _resources[i]   = 2.0*M_PI*double(rand())/double(RAND_MAX);
            //_resources[i]   = M_PI*double(rand())/double(RAND_MAX);
            _user_state_sum += _users[i];
            _resource_state_sum += _resources[i];
        }
        
    }
    
    
}

int CPRMContinuousModel::mod(int a, int b) {
    int result = a%b;
    
    if (result < 0)
        return result + b;
    else
        return result;
}

void CPRMContinuousModel::print_states(bool spin) {
    std::cout << "{{" << std::endl;
    for (int i = 0; i<_N; i++) {
        if (spin) {
            std::cout << std::setw(7) << std::setprecision(2) << cos(_users[i]);
        } else {
            std::cout << std::setw(7) << std::setprecision(2) << _users[i];
        }
        if (i != _N-1) {
            std::cout << ",";
        }
        if ((i+1)%_n==0) {
            std::cout << std::endl;
        }
    }
    std::cout << "},{" << std::endl;
    
    for (int i = 0; i<_N; i++) {
        if (spin) {
            std::cout << std::setw(7) << std::setprecision(2) << cos(_resources[i]);
        } else {
            std::cout << std::setw(7) << std::setprecision(2) << _resources[i];
        }
        if (i != _N-1) {
            std::cout << ",";
        }
        if ((i+1)%_n==0) {
            std::cout << std::endl;
        }
    }
    std::cout << "}}," << std::endl;
}

std::map<std::string,double> CPRMContinuousModel::cost_function_change(int try_index, ContinuousUserState user, ResourceState resource, ContinuousUserState new_user, ResourceState new_resource) {
    ContinuousUserState               user_neighbors[4]               = { 
        _users[mod(try_index+1, _N)], 
        _users[mod(try_index-1, _N)], 
        _users[mod(try_index+_n, _N)], 
        _users[mod(try_index-_n, _N)]
    };
    double                          user_cost_function_initial      = 0.0;
    double                          user_cost_function_final        = 0.0;
    double                          resource_cost_function_initial  = 0.0;
    double                          resource_cost_function_final    = 0.0;
    std::map<std::string,double>    return_value;
    
    for (int i=0; i < 4; i++) {
        user_cost_function_initial -= cos(user - user_neighbors[i]);
        //std::cout << cos(user - user_neighbors[i]) << " ";
    }
    
    user_cost_function_initial      -= cos(user - resource);
    //std::cout << cos(user - resource) << " ";
    resource_cost_function_initial  += cos(user - resource);
    
    for (int i=0; i < 4; i++) {
        user_cost_function_final -= cos(new_user - user_neighbors[i]);
        //std::cout << cos(new_user - user_neighbors[i]) << " ";
    }
    
    user_cost_function_final      -= cos(new_user - new_resource);
    //std::cout << cos(new_user - new_resource) << " ";
    resource_cost_function_final  += cos(new_user - new_resource);
    
    //std::cout << user_cost_function_final << " " << user_cost_function_initial << " " << resource_cost_function_final << " " << resource_cost_function_initial << " ";
    
    return_value["user"] =  user_cost_function_final - user_cost_function_initial;
    return_value["resource"] = resource_cost_function_final - resource_cost_function_initial;
    
    return return_value;
}

CPRMStateChange CPRMContinuousModel::try_new_state() {
    
    int                             try_index       = rand() % _N;
    ContinuousUserState             user            = _users[try_index];
    ResourceState                   resource        = _resources[try_index];
    //ContinuousUserState           new_user        = rand() % 3 + 1;
    ContinuousUserState             new_user        = 2.0*M_PI*double(rand())/double(RAND_MAX);
    ResourceState                   new_resource    = 2.0*M_PI*double(rand())/double(RAND_MAX);
    //ResourceState                 new_resource    = -double(rand())/double(RAND_MAX);
    std::map<std::string,double>    dCF             = cost_function_change(try_index, user, resource, new_user, new_resource);
    
    //std::cout << cos(user) << ", " << cos(resource) << ", " << cos(new_user) << ", " << cos(new_resource) << ", " << 2.0*M_PI*double(rand())/double(RAND_MAX);
    
    if (dCF["user"]+dCF["resource"] <= 0.0) {
        _users[try_index] = new_user;
        _user_state_sum += new_user - user;
        _resources[try_index] = new_resource;
        _resource_state_sum += new_resource - resource;
    }
    else {
        if (float(rand())/float(RAND_MAX) < exp(-1 * (_beta1 * dCF["user"] + _beta2 * dCF["resource"]))) {
            _users[try_index] = new_user;
            _user_state_sum += new_user - user;
            _resources[try_index] = new_resource;
            _resource_state_sum += new_resource - resource;

            //std::cout << " did user flip! ";
        }
    }
    
    //std::cout << std::endl;
    
    //if (dCF["user"] <= 0.0) {
    //    _users[try_index] = new_user;
    //    _user_state_sum += new_user - user;
   //}
    //else {
    //    if (float(rand())/float(RAND_MAX) < exp(-1 * (_beta1 * dCF["user"] + _beta2 * dCF["resource"]))) {
    //        _users[try_index] = new_user;
    //        _user_state_sum += new_user - user;
    //        //std::cout << "did user flip! ";
    //    }
    //}
    
    //if (dCF["resource"] <= 0.0) {
    //    _resources[try_index] = new_resource;
    //    _resource_state_sum += new_resource - resource;
    //}
    //else {
    //    if (float(rand())/float(RAND_MAX) < exp(-1 * (_beta1 * dCF["user"] + _beta2 * dCF["resource"]))) {
    //        _resources[try_index] = new_resource;
    //        _resource_state_sum += new_resource - resource;
    //        //std::cout << "did resource flip! ";
    //    }
    //}
     
    
    //std::cout << average_user_state() << ", " << average_resource_state() << std::endl;
    
    
    
    
     //float try_dE = dE(try_index);
     //std::vector<int> ret (3);
     
     //if (try_dE <= 0.0) {
     //ret[0] = try_index;
     //ret[1] = states[try_index];
     //states[try_index] = -1*states[try_index];
     //ret[2] = states[try_index];
     //std::cout << 1 << "\n";
     //E += try_dE;
     //M += 2.0*states[try_index];
     //return ret;
     //}
     //else {
     //std::cout << float(rand())/float(RAND_MAX) << "," << exp(-1*try_dE/kbT) << std::endl;
     //if (float(rand())/float(RAND_MAX) < exp(-1*try_dE/kbT)) {
     //ret[0] = try_index;
     //ret[1] = states[try_index];
     //states[try_index] = -1*states[try_index];
     //ret[2] = states[try_index];
     //std::cout << 0 << "\n";
     //E += try_dE;
     //M += 2.0*states[try_index];
     //return ret;
     //}
     //}
     //ret[0] = -1;
     //ret[1] = 0;
     //ret[2] = 0;
     //return ret;
     
    
    CPRMStateChange return_value(my_type);
    
    return return_value;
    
    
}
*/

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Discrete case
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void CPRMDiscreteModel::initilize(int in_n) {
    _n = in_n;
    _N = _n*_n;
    
    _users = new (std::nothrow) DiscreteUserState[_N];
    _resources = new (std::nothrow) DiscreteResourceState[_N];
    
    _user_field_strength = 1.0;
    _resource_field_strength = 1.0;
    
    _resource_field_increment = 0.1;
    
    if (_users == 0 || _resources == 0)
        std::cout << "Error: memory could not be allocated" << std::endl;
    else
    {
        randomize();
    }
}

void CPRMDiscreteModel::randomize() {
    
    _user_state_sum = 0;
    _resource_state_sum = 0;
    
    srand ( time(NULL) );
    
    for (int i = 0; i<_N; i++) {
        //_users[i] = rand() % 3 + 1;
        int user_state = rand() % 3;
        switch (user_state) {
            case 0:
                _users[i] = DEPLETE;
                _user_state_sum -= 1;
                break;
            case 1:
                _users[i] = NEUTRAL;
                break;
            case 2:
                _users[i] = CONSERVE;
                _user_state_sum += 1;
                break;
            default:
                break;
        }
        _resources[i].strength = _resource_field_increment * double(rand())/double(RAND_MAX);
        int resource_state = rand() % 3;
        switch (resource_state) {
            case 0:
                _resources[i].state = DEPLETE;
                _resource_state_sum -= _resources[i].strength;
                break;
            case 1:
                _resources[i].state = NEUTRAL;
                break;
            case 2:
                _resources[i].state = CONSERVE;
                _resource_state_sum += _resources[i].strength;
                break;
            default:
                break;
        }
    }

}

int CPRMDiscreteModel::mod(int a, int b) {
    int result = a%b;
    
    if (result < 0)
        return result + b;
    else
        return result;
}

void CPRMDiscreteModel::print_states() {
    for (int i = 0; i<_N; i++) {
        switch (_users[i]) {
            case DEPLETE:
                std::cout << std::setw(7) << -1;
                break;
            case NEUTRAL:
                std::cout << std::setw(7) << 0;
                break;
            case CONSERVE:
                std::cout << std::setw(7) << 1;
                break;
            default:
                break;
        }
        if ((i+1)%_n==0) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl << std::endl;
    
    for (int i = 0; i<_N; i++) {
        switch ( _resources[i].state) {
            case DEPLETE:
                std::cout << std::setw(7) << std::setprecision(2) << -_resources[i].strength;
                break;
            case NEUTRAL:
                std::cout << std::setw(7) << std::setprecision(2) << 0;
                break;
            case CONSERVE:
                std::cout << std::setw(7) << std::setprecision(2) << _resources[i].strength;
                break;
            default:
                break;
        }
        if ((i+1)%_n==0) {
            std::cout << std::endl;
        }
    }
}

double CPRMDiscreteModel::average_states(double &average_user_state, double &user_conserve_fraction, double &user_neutral_fraction, double &user_deplete_fraction, double &average_resource_state, double &resource_conserve_fraction, double &resource_neutral_fraction, double &resource_deplete_fraction) {
    
        double num_user_deplete = 0.0;
        double num_user_neutral = 0.0;
        double num_user_conserve = 0.0;
        double user_sum = 0.0;
        double num_resource_deplete = 0.0;
        double num_resource_neutral = 0.0;
        double num_resource_conserve = 0.0;
        double resource_sum = 0.0;
        double parity_sum = 0.0;
        
        DiscreteState user_state, resource_state;
        
        for (int i = 0; i < _N; i++) {
            user_state = _users[i];
            resource_state = _resources[i].state;
            
            if (user_state == resource_state) {
                parity_sum += 1.0;
            }
            
            switch (user_state) {
                case DEPLETE:
                    user_sum -= 1;
                    num_user_deplete += 1;
                    break;
                case CONSERVE:
                    user_sum += 1;
                    num_user_conserve += 1;
                    break;
                case NEUTRAL:
                    num_user_neutral += 1;
                    break;
                default:
                    break;
            }
            
            switch (resource_state) {
                case DEPLETE:
                    resource_sum -= _resources[i].strength;
                    num_resource_deplete += 1;
                    break;
                case CONSERVE:
                    resource_sum += _resources[i].strength;
                    num_resource_conserve += 1;
                    break;
                case NEUTRAL:
                    num_resource_neutral += 1;
                    break;
                default:
                    break;
            }
        }
        
        user_conserve_fraction = num_user_conserve/double(_N);
        user_neutral_fraction = num_user_neutral/double(_N);
        user_deplete_fraction = num_user_deplete/double(_N);
        average_user_state = user_sum / double(_N);
        
        resource_conserve_fraction = num_resource_conserve/double(_N);
        resource_neutral_fraction = num_resource_neutral/double(_N);
        resource_deplete_fraction = num_resource_deplete/double(_N);
        average_resource_state = resource_sum / double(_N);
        
        return parity_sum / double(_N);


}
void CPRMDiscreteModel::print_states_to_file(std::ofstream & states_file) {
    
    states_file << "u ";
    for (int i = 0; i<_N; i++) {
        switch (_users[i]) {
            case DEPLETE:
                states_file << -1 << " ";
                break;
            case NEUTRAL:
                states_file << 0 << " ";
                break;
            case CONSERVE:
                states_file << 1 << " ";
                break;
            default:
                break;
        }
    }
    states_file << std::endl;
    
    states_file << "r ";
    for (int i = 0; i<_N; i++) {
        switch ( _resources[i].state) {
            case DEPLETE:
                states_file << -_resources[i].strength << " ";
                break;
            case NEUTRAL:
                states_file << 0 << " ";
                break;
            case CONSERVE:
                states_file << _resources[i].strength << " ";
                break;
            default:
                break;
        }
    }
    states_file << std::endl;

}

double CPRMDiscreteModel::U(DiscreteUserState user, DiscreteResourceState resource) {
    double cc = 1.0;
    double cn = 0.5;
    double cd = -1.0;
    double nc = 0.5;
    double nn = -0.5;
    double nd = 0.5;
    double dc = -1.0;
    double dn = 0.5;
    double dd = 1.0;
    
    if (user == CONSERVE) {
        if (resource.state == CONSERVE) {
            return cc - 0.5;
        } else if (resource.state == NEUTRAL) {
            return cn - 0.5;
        } else if (resource.state == DEPLETE) {
            return cd - 0.5;
        }
    } else if (user == NEUTRAL) {
        if (resource.state == CONSERVE) {
            return nc - 0.5;
        } else if (resource.state == NEUTRAL) {
            return nn - 0.5;
        } else if (resource.state == DEPLETE) {
            return nd - 0.5;
        }
    } else if (user == DEPLETE) {
        if (resource.state == CONSERVE) {
            return dc - 0.5;
        } else if (resource.state == NEUTRAL) {
            return dn - 0.5;
        } else if (resource.state == DEPLETE) {
            return dd - 0.5;
        }
    }
}

double CPRMDiscreteModel::R(DiscreteUserState user, DiscreteResourceState resource) {
    double cc = -1.0;
    double cn = 0.5;
    double cd = 1.0;
    double nc = 0.0;
    double nn = 0.0;
    double nd = 0.0;
    double dc = 1.0;
    double dn = 0.5;
    double dd = -1.0;
    
    if (user == CONSERVE) {
        if (resource.state == CONSERVE) {
            return cc - 0.5;
        } else if (resource.state == NEUTRAL) {
            return cn - 0.5;
        } else if (resource.state == DEPLETE) {
            return cd - 0.5;
        }
    } else if (user == NEUTRAL) {
        if (resource.state == CONSERVE) {
            return nc - 0.5;
        } else if (resource.state == NEUTRAL) {
            return nn - 0.5;
        } else if (resource.state == DEPLETE) {
            return nd - 0.5;
        }
    } else if (user == DEPLETE) {
        if (resource.state == CONSERVE) {
            return dc - 0.5;
        } else if (resource.state == NEUTRAL) {
            return dn - 0.5;
        } else if (resource.state == DEPLETE) {
            return dd - 0.5;
        }
    }
}

HInfo CPRMDiscreteModel::H(LatticeIndex index, DiscreteUserState user, DiscreteResourceState resource) {
    DiscreteUserState   user_neighbors[4] =   {
                                                _users[mod(index+1, _N)], 
                                                _users[mod(index-1, _N)], 
                                                _users[mod(index+_n, _N)], 
                                                _users[mod(index-_n, _N)]
                                            };
    DiscreteResourceState   resource_neighbors[4] =   {
                                                _resources[mod(index+1, _N)], 
                                                _resources[mod(index-1, _N)], 
                                                _resources[mod(index+_n, _N)], 
                                                _resources[mod(index-_n, _N)]
                                            };
    HInfo               the_H;
    
    the_H.H_user = 0.0;
    the_H.H_resource = 0.0;
    
    for (int i=0; i < 4; i++) {
        if (user == user_neighbors[i]) {
            the_H.H_user --;
        }
        if (resource.state == resource_neighbors[i].state) {
            the_H.H_resource --;
        }
    }
    
    the_H.H_user      += _user_field_strength * U(user, resource) * resource.strength;
    the_H.H_resource  += _resource_field_strength * R(user, resource) * resource.strength;
    //the_H.H_resource  += _resource_field_strength * R(user, resource);
    
    return the_H;
}

HInfo CPRMDiscreteModel::cost_function_change(LatticeIndex try_index, DiscreteUserState old_user, DiscreteResourceState old_resource, DiscreteUserState new_user, DiscreteResourceState new_resource) {
    HInfo                           dH;
    HInfo                           initial_H;
    HInfo                           final_H;
    
    initial_H = H(try_index, old_user, old_resource);
    final_H = H(try_index,new_user,new_resource);
    
    dH.H_user       = final_H.H_user - initial_H.H_user;
    dH.H_resource   = final_H.H_resource - initial_H.H_resource;
    
    return dH;
}

void CPRMDiscreteModel::change_state(LatticeIndex index, DiscreteUserState new_user, DiscreteResourceState new_resource) {
    _users[index]       = new_user;
    _resources[index]   = new_resource;
    
    /*
    switch (new_user) {
        case DEPLETE:
            _user_state_sum -= 1;
            break;
        case CONSERVE:
            _user_state_sum += 1;
            break;
        default:
            break;
    }
    
    switch (new_resource.state) {
        case DEPLETE:
            _resource_state_sum -= new_resource.strength;
            break;
        case CONSERVE:
            _resource_state_sum += new_resource.strength;
            break;
        default:
            break;
    }
    */
}

StateChangeInfo CPRMDiscreteModel::try_new_state() {
    LatticeIndex            try_index;
    DiscreteUserState       user;
    DiscreteResourceState   resource;
    DiscreteUserState       new_user;
    DiscreteResourceState   new_resource;
    HInfo                   dH;
    StateChangeInfo         state_change;
    
    try_index   = rand() % _N;
    user        = _users[try_index];
    resource    = _resources[try_index];
    switch (rand() % 3) {
        case 0:
            new_user = DEPLETE;
            break;
        case 1:
            new_user = NEUTRAL;
            break;
        case 2:
            new_user = CONSERVE;
            break;
        default:
            break;
    }
    switch (rand() % 3) {
        case 0:
            new_resource.state = DEPLETE;
            if (resource.state == DEPLETE) {
                if (resource.strength <= 4.0 )
                    new_resource.strength = resource.strength + _resource_field_increment * double(rand())/double(RAND_MAX);
            } else {
                new_resource.strength = _resource_field_increment * double(rand())/double(RAND_MAX);
            }
            break;
        case 1:
            new_resource.state = NEUTRAL;
            if (resource.state == NEUTRAL) {
                if (resource.strength <= 4.0 )
                    new_resource.strength = resource.strength + _resource_field_increment * double(rand())/double(RAND_MAX);
            } else {
                new_resource.strength = _resource_field_increment * double(rand())/double(RAND_MAX);
            }
            break;
        case 2:
            new_resource.state = CONSERVE;
            if (resource.state == CONSERVE) {
                if (resource.strength <= 4.0 )
                    new_resource.strength = resource.strength + _resource_field_increment * double(rand())/double(RAND_MAX);
            } else {
                new_resource.strength = _resource_field_increment * double(rand())/double(RAND_MAX);
            }
            break;
        default:
            break;
    }
    
    dH  = cost_function_change(try_index, user, resource, new_user, new_resource);
    
    //    = 2.0*double(rand())/double(RAND_MAX)-1.0;

    
    //std::cout << dCF["user"] << ", " << dCF["resource"] << ", ";
    
    if (dH.H_user + dH.H_resource <= 0.0) {
        change_state(try_index, new_user, new_resource);
        
        state_change.D_user_state = new_user;
        state_change.D_resource_state = new_resource;
        state_change.state_changed = true;
        return state_change;
    } else {
        if (float(rand())/float(RAND_MAX) < exp(-1 * (_beta1 * dH.H_user + _beta2 * dH.H_resource))) {
        //if (float(rand())/float(RAND_MAX) < exp(-1 * (dH.H_user + _beta2 * dH.H_resource))) {
            change_state(try_index, new_user, new_resource);
            
            state_change.D_user_state = new_user;
            state_change.D_resource_state = new_resource;
            state_change.state_changed = true;
            return state_change;
            //std::cout << "did user flip! ";
        } else {
            state_change.state_changed = false;
            return state_change;
        }
    }
    
    /*
    if (dCF["user"] <= 0.0) {
        _users[try_index] = new_user;
        _user_state_sum += new_user - user;
    }
    else {
        if (float(rand())/float(RAND_MAX) < exp(-1 * (_beta1 * dCF["user"] + _beta2 * dCF["resource"]))) {
            _users[try_index] = new_user;
            _user_state_sum += new_user - user;
            //std::cout << "did user flip! ";
        }
    }
    
    if (dCF["resource"] <= 0.0) {
        _resources[try_index] = new_resource;
        _resource_state_sum += new_resource - resource;
    }
    else {
        if (float(rand())/float(RAND_MAX) < exp(-1 * (_beta1 * dCF["user"] + _beta2 * dCF["resource"]))) {
            _resources[try_index] = new_resource;
            _resource_state_sum += new_resource - resource;
            //std::cout << "did resource flip! ";
        }
    }
    */
    //std::cout << average_user_state() << ", " << average_resource_state() << std::endl;


    
    /*
    float try_dE = dE(try_index);
    std::vector<int> ret (3);
    
    if (try_dE <= 0.0) {
        ret[0] = try_index;
        ret[1] = states[try_index];
        states[try_index] = -1*states[try_index];
        ret[2] = states[try_index];
        //std::cout << 1 << "\n";
        E += try_dE;
        M += 2.0*states[try_index];
        return ret;
    }
    else {
        //std::cout << float(rand())/float(RAND_MAX) << "," << exp(-1*try_dE/kbT) << std::endl;
        if (float(rand())/float(RAND_MAX) < exp(-1*try_dE/kbT)) {
            ret[0] = try_index;
            ret[1] = states[try_index];
            states[try_index] = -1*states[try_index];
            ret[2] = states[try_index];
            //std::cout << 0 << "\n";
            E += try_dE;
            M += 2.0*states[try_index];
            return ret;
        }
    }
    ret[0] = -1;
    ret[1] = 0;
    ret[2] = 0;
    return ret;
    */
}

void CPRMDiscreteModel::set_parameter(double parameter_value, Parameter parameter) {
    switch (parameter) {
        case BETA1:
            _beta1 = parameter_value;
            break;
        case BETA2:
            _beta2 = parameter_value;
            break;
        case USER_FIELD_STRENGTH:
            _user_field_strength = parameter_value;
            break;
        case RESOURCE_FIELD_STRENGTH:
            _resource_field_strength = parameter_value;
            break;
        case RESOURCE_FIELD_INCREMENT:
            _resource_field_increment = parameter_value;
            break;
        default:
            break;
    }
}

double CPRMDiscreteModel::get_parameter(Parameter parameter) {
    switch (parameter) {
        case BETA1:
            return _beta1;
            break;
        case BETA2:
            return _beta2;
            break;
        case USER_FIELD_STRENGTH:
            return _user_field_strength;
            break;
        case RESOURCE_FIELD_STRENGTH:
            return _resource_field_strength;
            break;
        case RESOURCE_FIELD_INCREMENT:
            return _resource_field_increment;
            break;
        default:
            return _beta1;
            break;
    }
}